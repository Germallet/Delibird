#include "socket.h"
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>

static int GenerarDirecciones(char* ip, char* puerto, struct addrinfo** infoDireccion)
{
	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	return getaddrinfo(ip, puerto, &hints, infoDireccion);
}
static struct sockaddr_in* GenerarDireccionEscucha(uint16_t puerto)
{
	struct sockaddr_in* direccionEscucha = calloc(1, sizeof(struct sockaddr_in));
	memset(direccionEscucha, 0, sizeof(struct sockaddr_in));
	direccionEscucha->sin_family = AF_INET;
	direccionEscucha->sin_port = htons(puerto);
	direccionEscucha->sin_addr.s_addr = INADDR_ANY;
	return direccionEscucha;
}

static int ConectarSocket(struct addrinfo* infoServidor, int numSocket)
{
	return connect(numSocket, infoServidor->ai_addr, infoServidor->ai_addrlen);
}
int NoBloquearPuerto(int numSocket)
{
	int activado = 1;
	return setsockopt(numSocket, SOL_SOCKET, SO_REUSEADDR, &activado, sizeof(activado));
}
int Socket_Crear(char *ip, char* puerto)
{
	struct addrinfo* infoServidor = NULL;
	if (GenerarDirecciones(ip, puerto, &infoServidor) != 0)
	{
		freeaddrinfo(infoServidor);
		return -1;
	}

	int numSocket = socket(infoServidor->ai_family, infoServidor->ai_socktype, infoServidor->ai_protocol);
	if (numSocket == -1)
	{
		freeaddrinfo(infoServidor);
		return -1;
	}

	if (NoBloquearPuerto(numSocket) == -1)
	{
		freeaddrinfo(infoServidor);
		return -1;
	}

	if (ConectarSocket(infoServidor, numSocket) != 0)
		numSocket = -1;

	freeaddrinfo(infoServidor);
	return numSocket;
}

static size_t TamanioDePaquete(Paquete* paquete)
{
	return sizeof(paquete->codigoOperacion) + paquete->tamanio + sizeof(paquete->tamanio);
}
static void* SerializarPaquete(Paquete* paquete, size_t* tamanioFinal)
{
	*tamanioFinal = TamanioDePaquete(paquete);
	void* paqueteSerializado = malloc(*tamanioFinal);

	int desplazamiento = 0;
	memcpy(paqueteSerializado, &(paquete->codigoOperacion), sizeof(paquete->codigoOperacion));
	desplazamiento += sizeof(paquete->codigoOperacion);
	memcpy(paqueteSerializado + desplazamiento, &(paquete->tamanio), sizeof(paquete->tamanio));
	desplazamiento += sizeof(paquete->tamanio);
	memcpy(paqueteSerializado + desplazamiento, paquete->stream, paquete->tamanio);
	desplazamiento += paquete->tamanio;

	return paqueteSerializado;

}
int Socket_Enviar(uint32_t codigoOperacion, void* stream, int tamanio, int numSocket)
{
	Paquete* paquete = malloc(sizeof(Paquete));
	paquete->codigoOperacion = codigoOperacion;
	paquete->tamanio = tamanio;
	paquete->stream = stream;

	size_t tamanioFinal;
	void* paqueteSerializado = SerializarPaquete(paquete, &tamanioFinal);

	int resultado = send(numSocket, paqueteSerializado, tamanioFinal, 0);

	free(paqueteSerializado);
	free(paquete);

	return resultado;
}

int Socket_Recibir(int numSocket, Paquete** paquete)
{
	*paquete = malloc(sizeof(Paquete));
	return recv(numSocket, &((*paquete)->codigoOperacion), sizeof((*paquete)->codigoOperacion), MSG_WAITALL);
}

int Socket_ProcesarPaquete(int numSocket, Paquete* paquete)
{
	if (recv(numSocket, &(paquete->tamanio), sizeof(paquete->tamanio), MSG_WAITALL) == -1)
		return -1;
	paquete->stream = malloc(paquete->tamanio);
	if (recv(numSocket, paquete->stream, paquete->tamanio, MSG_WAITALL) == -1)
		return -1;
	return 0;
}

void Socket_LiberarPaquete(Paquete* paquete)
{
	free(paquete->stream);
	free(paquete);
}

void Socket_LiberarConexion(int numSocket)
{
	close(numSocket);
}

typedef struct
{
	int socketDeEscucha;
	uint16_t puerto;
	Eventos eventos;
} DatosDeEscucha;

typedef void (*eventoOperacion)(DatosConexion*, Paquete*);

static bool TieneEvento(t_dictionary* operaciones, uint32_t codigoOperacion)
{
	char codigoChar[] = {codigoOperacion, '\0'};
	return dictionary_has_key(operaciones, codigoChar);
}
static eventoOperacion ObtenerEvento(t_dictionary* operaciones, uint32_t codigoOperacion)
{
	char codigoChar = (char)codigoOperacion;
	return (eventoOperacion)dictionary_get(operaciones, &codigoChar);
}

static void EscucharMensajes(DatosConexion* conexion)
{
	while(1)
	{
		Paquete* paqueteRecibido = NULL;
		if (Socket_Recibir(conexion->socket, &paqueteRecibido) == -1)
		{
			if(conexion->eventos.error != NULL)
				(conexion->eventos.error)(ERROR_RECIBIR, paqueteRecibido);
			Socket_LiberarPaquete(paqueteRecibido);
			break;
		}
		if (!TieneEvento(conexion->eventos.operaciones, paqueteRecibido->codigoOperacion))
		{
			if(conexion->eventos.error != NULL)
				(conexion->eventos.error)(ERROR_OPERACION_INVALIDA, paqueteRecibido);
			break;
		}
		if (Socket_ProcesarPaquete(conexion->socket, paqueteRecibido) == -1)
		{
			if(conexion->eventos.error != NULL)
				(conexion->eventos.error)(ERROR_PROCESAR_PAQUETE, paqueteRecibido);
			break;
		}
		ObtenerEvento(conexion->eventos.operaciones, paqueteRecibido->codigoOperacion)(conexion, paqueteRecibido);

		Socket_LiberarPaquete(paqueteRecibido);
	}

	if(conexion->eventos.error != NULL)
		(conexion->eventos.desconectado)();

	Socket_LiberarConexion(conexion->socket);
	free(conexion->direccion);
	free(conexion);
}
static void EscuchaConexiones(DatosDeEscucha* datosDeEscucha)
{
	socklen_t tamDireccion = sizeof(struct sockaddr_in);
	int socketCliente;

	while(1)
	{
		struct sockaddr_in* direccionCliente = NULL;
		socketCliente = accept(datosDeEscucha->socketDeEscucha, (struct sockaddr*)direccionCliente, &tamDireccion);
		if (socketCliente == -1)
			break;

		// Genero datos de conexión
		DatosConexion* conexion = malloc(sizeof(DatosConexion));
		conexion->socket = socketCliente;
		conexion->direccion = direccionCliente;
		conexion->eventos = datosDeEscucha->eventos;

		// Ejecuto evento correspondiente
		if(datosDeEscucha->eventos.conectado != NULL)
			datosDeEscucha->eventos.conectado(/*connection*/);

		// Escucho nuevos mensajes
		pthread_create(&(conexion->thread), NULL, (void*)EscucharMensajes, conexion);
	}
}

int Socket_IniciarEscucha(uint16_t puerto, void(*eventoConectado)(), void(*eventoDesconectado)(), eventoError eventoError, t_dictionary* operaciones)
{
	// Iniciar socket de escucha
	int socketDeEscucha;
	if ((socketDeEscucha = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		return -1;
	if (NoBloquearPuerto(socketDeEscucha) == -1)
		return -1;

	struct sockaddr_in* direccionEscucha = GenerarDireccionEscucha(puerto);
	if (bind(socketDeEscucha, (struct sockaddr *)direccionEscucha, sizeof(struct sockaddr)) == -1)
	{
		Socket_LiberarConexion(socketDeEscucha);
		return -1;
	}
	if (listen(socketDeEscucha, SOMAXCONN) == -1)
	{
		Socket_LiberarConexion(socketDeEscucha);
		return -1;
	}

	// Crear DatosDeEscucha
	DatosDeEscucha* datosDeEscucha = malloc(sizeof(DatosDeEscucha));
	datosDeEscucha->socketDeEscucha = socketDeEscucha;
	datosDeEscucha->puerto = puerto;
	datosDeEscucha->eventos.conectado = eventoConectado;
	datosDeEscucha->eventos.desconectado = eventoDesconectado;
	datosDeEscucha->eventos.error = eventoError;
	datosDeEscucha->eventos.operaciones = operaciones;

	// Iniciar thread
	pthread_t threadEscucha;
	pthread_create(&threadEscucha, NULL, (void*)EscuchaConexiones, datosDeEscucha);

	// Liberar recursos
	free(direccionEscucha);
	return socketDeEscucha;
}




