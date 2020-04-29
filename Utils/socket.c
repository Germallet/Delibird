#include "socket.h"
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

// ========== Crear ==========
static int NoBloquearPuerto(int numSocket)
{
	int activado = 1;
	return setsockopt(numSocket, SOL_SOCKET, SO_REUSEADDR, &activado, sizeof(activado));
}
int Socket_Crear(int domain, int type, int protocol)
{
	int nuevoSocket = socket(domain, type, protocol);
	if (nuevoSocket == -1 || NoBloquearPuerto(nuevoSocket) == -1)
		return -1;
	return nuevoSocket;
}

// ========== Conectar ==========
static int GenerarDirecciones(char* ip, char* puerto, struct addrinfo** infoDireccion)
{
	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	return getaddrinfo(ip, puerto, &hints, infoDireccion);
}
struct sockaddr_in* Socket_Conectar(int socket, char* ip, char* puerto)
{
	struct addrinfo* infoServidor = NULL;
	if (GenerarDirecciones(ip, puerto, &infoServidor) != 0)
	{
		freeaddrinfo(infoServidor);
		return NULL;
	}
	if (connect(socket, infoServidor->ai_addr, infoServidor->ai_addrlen) == -1)
	{
		freeaddrinfo(infoServidor);
		return NULL;
	}
	return (struct sockaddr_in*)infoServidor;
}

// ========== Escuchar ==========
static struct sockaddr_in* GenerarDireccionEscucha(uint16_t puerto)
{
	struct sockaddr_in* direccionEscucha = calloc(1, sizeof(struct sockaddr_in));
	memset(direccionEscucha, 0, sizeof(struct sockaddr_in));
	direccionEscucha->sin_family = AF_INET;
	direccionEscucha->sin_port = htons(puerto);
	direccionEscucha->sin_addr.s_addr = INADDR_ANY;
	return direccionEscucha;
}
Servidor* Socket_Escuchar(int socket, uint16_t puerto, Eventos* eventos)
{
	struct sockaddr_in* direccionEscucha = GenerarDireccionEscucha(puerto);
	if (bind(socket, (struct sockaddr *)direccionEscucha, sizeof(struct sockaddr)) == -1)
	{
		Socket_Destruir(socket);
		socket = -1;
	}
	else if (listen(socket, SOMAXCONN) == -1)
	{
		Socket_Destruir(socket);
		socket = -1;
	}

	Servidor* servidor = malloc(sizeof(Servidor));
	servidor->socket = socket;
	servidor->puerto = puerto;
	servidor->eventos = eventos;
	servidor->thread = NULL;

	free(direccionEscucha);
	return servidor;
}

// ========== Comunicación ==========
int Socket_RecibirPaquete(int numSocket, Paquete** paquete)
{
	*paquete = malloc(sizeof(Paquete));
	return recv(numSocket, &((*paquete)->codigoOperacion), sizeof((*paquete)->codigoOperacion), MSG_WAITALL);
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
Cliente* Socket_AceptarConexion(Servidor* servidor)
{
	socklen_t tamDireccion = sizeof(struct sockaddr_in);
	struct sockaddr_in* direccionCliente = NULL;

	int socketCliente = accept(servidor->socket, (struct sockaddr*)direccionCliente, &tamDireccion);
	if (socketCliente == -1)
	{
		freeaddrinfo((struct addrinfo*)direccionCliente);
		return NULL;
	}

	Cliente* cliente = malloc(sizeof(Cliente));
	cliente->socket = socketCliente;
	cliente->direccion = direccionCliente;
	cliente->eventos = *(servidor->eventos);
	cliente->thread = NULL;
	return cliente;
}

// ========== Finalizar ==========
void Socket_Cerrar(int socket)
{
	shutdown(socket, SHUT_RDWR);
}
void Socket_Destruir(int socket)
{
	close(socket);
}
