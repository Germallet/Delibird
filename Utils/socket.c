#include "socket.h"
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>

// ===== Escuchar nuevos mensajes =====

static int Recibir(int numSocket, Paquete** paquete)
{
	*paquete = malloc(sizeof(Paquete));
	return recv(numSocket, &((*paquete)->codigoOperacion), sizeof((*paquete)->codigoOperacion), MSG_WAITALL);
}
static void EscucharMensajes(DatosConexion* conexion)
{
	while(1)
	{
		Paquete* paqueteRecibido = NULL;
		if (Recibir(conexion->socket, &paqueteRecibido) == -1)
		{
			if(conexion->eventos.error != NULL)
				(conexion->eventos.error)(ERROR_RECIBIR, paqueteRecibido);
			Paquete_Liberar(paqueteRecibido);
			break;
		}
		if (!Eventos_TieneOperacion(&(conexion->eventos), paqueteRecibido->codigoOperacion))
		{
			if(conexion->eventos.error != NULL)
				(conexion->eventos.error)(ERROR_OPERACION_INVALIDA, paqueteRecibido);
			break;
		}
		if (Paquete_Procesar(conexion->socket, paqueteRecibido) == -1)
		{
			if(conexion->eventos.error != NULL)
				(conexion->eventos.error)(ERROR_PROCESAR_PAQUETE, paqueteRecibido);
			break;
		}
		Eventos_ObtenerOperacion(&(conexion->eventos), paqueteRecibido->codigoOperacion)(conexion, paqueteRecibido);

		Paquete_Liberar(paqueteRecibido);
	}

	if(conexion->eventos.error != NULL) (conexion->eventos.desconectado)();

	Socket_LiberarConexion(conexion->socket);
	free(conexion->direccion);
	free(conexion);
}
static void IniciarThreadEscuchaMensajes(int socket, struct sockaddr_in* direccion, Eventos eventos)
{
	DatosConexion* conexion = malloc(sizeof(DatosConexion));
	conexion->socket = socket;
	conexion->direccion = direccion;
	conexion->eventos = eventos;

	pthread_create(&(conexion->thread), NULL, (void*)EscucharMensajes, conexion);
}

// ===== Escuchar nuevas conexiones =====

typedef struct
{
	int socketDeEscucha;
	uint16_t puerto;
	Eventos* eventos;
} DatosDeEscucha;

static void EscucharConexiones(DatosDeEscucha* datosDeEscucha)
{
	socklen_t tamDireccion = sizeof(struct sockaddr_in);
	int socketCliente;

	while(1)
	{
		struct sockaddr_in* direccionCliente = NULL;
		socketCliente = accept(datosDeEscucha->socketDeEscucha, (struct sockaddr*)direccionCliente, &tamDireccion);
		if (socketCliente == -1)
			break;

		if(datosDeEscucha->eventos->conectado != NULL) datosDeEscucha->eventos->conectado();

		IniciarThreadEscuchaMensajes(socketCliente, direccionCliente, *(datosDeEscucha->eventos));
	}
	Eventos_Destruir(datosDeEscucha->eventos);
}
static void IniciarThreadEscuchaConexiones(int socket, uint16_t puerto, Eventos* eventos)
{
	DatosDeEscucha* datosDeEscucha = malloc(sizeof(DatosDeEscucha));
	datosDeEscucha->socketDeEscucha = socket;
	datosDeEscucha->puerto = puerto;
	datosDeEscucha->eventos = eventos;

	pthread_t threadEscucha;
	pthread_create(&threadEscucha, NULL, (void*)EscucharConexiones, datosDeEscucha);
}

// ===== Creación de sockets =====

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
static int NoBloquearPuerto(int numSocket)
{
	int activado = 1;
	return setsockopt(numSocket, SOL_SOCKET, SO_REUSEADDR, &activado, sizeof(activado));
}

static int ConectarSocket(struct addrinfo* infoServidor, int numSocket)
{
	return connect(numSocket, infoServidor->ai_addr, infoServidor->ai_addrlen);
}
int Socket_CrearCliente(char *ip, char* puerto, Eventos* eventos)
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

	IniciarThreadEscuchaMensajes(numSocket, (struct sockaddr_in*)infoServidor, *eventos);
	Eventos_Destruir(eventos);

	freeaddrinfo(infoServidor);
	return numSocket;
}
int Socket_Enviar(uint32_t codigoOperacion, void* stream, int tamanio, int numSocket)
{
	Paquete* paquete = malloc(sizeof(Paquete));
	paquete->codigoOperacion = codigoOperacion;
	paquete->tamanio = tamanio;
	paquete->stream = stream;

	size_t tamanioFinal;
	void* paqueteSerializado = Paquete_Serializar(paquete, &tamanioFinal);

	int resultado = send(numSocket, paqueteSerializado, tamanioFinal, 0);

	free(paqueteSerializado);
	free(paquete);

	return resultado;
}

int Socket_IniciarEscucha(uint16_t puerto, Eventos* eventos)
{
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

	IniciarThreadEscuchaConexiones(socketDeEscucha, puerto, eventos);

	free(direccionEscucha);
	return socketDeEscucha;
}

void Socket_LiberarConexion(int numSocket)
{
	close(numSocket);
}
