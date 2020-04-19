#include "socket.h"
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

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
//	*tamanioFinal = TamanioDePaquete(paquete);
//	void* paqueteSerializado = malloc(*tamanioFinal);
//
//	int desplazamiento = 0;
//	memcpy(paqueteSerializado, &(paquete->codigoOperacion), sizeof(paquete->codigoOperacion));
//	desplazamiento += sizeof(paquete->codigoOperacion);
//	memcpy(paqueteSerializado + desplazamiento, &(paquete->tamanio), sizeof(paquete->tamanio));
//	desplazamiento += sizeof(paquete->tamanio);
//	memcpy(paqueteSerializado + desplazamiento, paquete->stream, paquete->tamanio);
//	desplazamiento += paquete->tamanio;
//
//	return paqueteSerializado;

	*tamanioFinal = TamanioDePaquete(paquete);
	void* paqueteSerializado = malloc(*tamanioFinal);

	if (paquete->codigoOperacion == 2) {
		SerializarNew(paqueteSerializado, &tamanioFinal);
	} else if (paquete->codigoOperacion == 3) {
		SerializarAppeared(paqueteSerializado, &tamanioFinal);
	} else if (paquete->codigoOperacion == 4) {
		SerializarCatch(paqueteSerializado, &tamanioFinal);
	} else if (paquete->codigoOperacion == 5) {
		SerializarCaught(paqueteSerializado, &tamanioFinal);
	} else if (paquete->codigoOperacion == 6) {
		SerializarGet(paqueteSerializado, &tamanioFinal);
	} else if (paquete->codigoOperacion == 7) {
		SerializarLocalized(paqueteSerializado, &tamanioFinal);
	}
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
	uint16_t puerto;
	int socketDeEscucha;
	void(*EventoNuevoCliente)();
	/*t_dictionary * fns_receipts;
	void(*fn_connectionClosed)();
	void * data;*/
} DatosDeEscucha;


static void Escucha(DatosDeEscucha* datosDeEscucha)
{
	struct sockaddr_in direccionCliente;
	socklen_t tamDireccion = sizeof(struct sockaddr_in);
	int socketCliente;

	while(1)
	{
		socketCliente = accept(datosDeEscucha->socketDeEscucha, (struct sockaddr*)&direccionCliente, &tamDireccion);
		if (socketCliente == -1)
			break;

		//Creo estructura connection
		/*socket_connection * connection = malloc(sizeof(socket_connection));
		connection->socket = newConnection;
		connection->ip = string_duplicate(inet_ntoa(addr_client.sin_addr));
		connection->port = args->port;
		connection->data = args->data;*/

		//aviso que se conecto un socket
		if(datosDeEscucha->EventoNuevoCliente != NULL)
			datosDeEscucha->EventoNuevoCliente(/*connection*/);

		//creo receptor de mensajes
		/*args_receiptMessage *args_rm = malloc(sizeof(args_receiptMessage));
		args_rm->connection = connection;
		args_rm->fns_receipts = args->fns_receipts;
		args_rm->fn_connectionClosed = args->fn_connectionClosed;
		pthread_create(&th_receiptMessage, NULL, (void*) receiptMessage, args_rm);*/
	}
}

int Socket_IniciarEscucha(uint16_t puerto, void(*EventoNuevoCliente)())
{
	int socketDeEscucha;
	if ((socketDeEscucha = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		return -1;
	}

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

	DatosDeEscucha* datosDeEscucha = malloc(sizeof(DatosDeEscucha));
	datosDeEscucha->puerto = puerto;
	datosDeEscucha->socketDeEscucha = socketDeEscucha;
	datosDeEscucha->EventoNuevoCliente = EventoNuevoCliente;
	/*args->fns_receipts = fns;
	args->fn_connectionClosed = fn_connectionClosed;
	args->data = data;*/

	pthread_t threadEscucha;
	pthread_create(&threadEscucha, NULL, (void*)Escucha, datosDeEscucha);

	free(direccionEscucha);
	return socketDeEscucha;
}

static void* SerializarAppeared(Paquete* paquete, size_t* tamanioFinal)
{
	int desplazamiento = 0;
	memcpy(paqueteSerializado, &(paquete->codigoOperacion), sizeof(paquete->codigoOperacion));
	desplazamiento += sizeof(paquete->codigoOperacion);
	memcpy(paqueteSerializado + desplazamiento, &(paquete->tamanio), sizeof(paquete->tamanio));
	desplazamiento += sizeof(paquete->tamanio);
	memcpy(paqueteSerializado + desplazamiento, paquete->stream, paquete->tamanio);
	desplazamiento += paquete->tamanio;

	return paqueteSerializado;
}

static void* SerializarNew(Paquete* paquete, size_t* tamanioFinal) {

}

static void* SerializarCatch(Paquete* paquete, size_t* tamanioFinal) {

}

static void* SerializarGet(Paquete* paquete, size_t* tamanioFinal) {

}

static void* SerializarCaught(Paquete* paquete, size_t* tamanioFinal) {

}

static void* SerializarLocalized(Paquete* paquete, size_t* tamanioFinal) {

}






