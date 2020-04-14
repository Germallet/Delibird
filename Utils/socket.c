#include "socket.h"
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include<unistd.h>

static struct addrinfo* GenerarDireccion(char* ip, char* puerto)
{
	struct addrinfo hints;
	struct addrinfo *infoDireccion;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip, puerto, &hints, &infoDireccion);
	return infoDireccion;
}
static int ConectarSocket(struct addrinfo* infoServidor, int numSocket)
{
	return connect(numSocket, infoServidor->ai_addr, infoServidor->ai_addrlen);
}
static void NoBloquearPuerto(int numSocket)
{
	int activado = 1;
	setsockopt(numSocket, SOL_SOCKET, SO_REUSEADDR, &activado, sizeof(activado));
}
int Socket_Crear(char *ip, char* puerto)
{
	struct addrinfo* infoServidor = GenerarDireccion(ip, puerto);

	int numSocket = socket(infoServidor->ai_family, infoServidor->ai_socktype, infoServidor->ai_protocol);
	NoBloquearPuerto(numSocket);

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