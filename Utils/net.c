#include "net.h"
#include "socket.h"
#include <sys/socket.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <netdb.h>

// ===== Escuchar nuevos mensajes =====

static void EscucharMensajes(Cliente* cliente)
{
	while(1)
	{
		Paquete* paqueteRecibido = NULL;
		if (Socket_RecibirPaquete(cliente->socket, &paqueteRecibido) == -1)
		{
			if(cliente->eventos.error != NULL)
				(cliente->eventos.error)(ERROR_RECIBIR, paqueteRecibido);
			Paquete_Liberar(paqueteRecibido);
			break;
		}
		if (!Eventos_TieneOperacion(&(cliente->eventos), paqueteRecibido->codigoOperacion))
		{
			if(cliente->eventos.error != NULL)
				(cliente->eventos.error)(ERROR_OPERACION_INVALIDA, paqueteRecibido);
			break;
		}
		if (Paquete_Procesar(cliente->socket, paqueteRecibido) == -1)
		{
			if(cliente->eventos.error != NULL)
				(cliente->eventos.error)(ERROR_PROCESAR_PAQUETE, paqueteRecibido);
			break;
		}
		Eventos_ObtenerOperacion(&(cliente->eventos), paqueteRecibido->codigoOperacion)(cliente, paqueteRecibido);

		Paquete_Liberar(paqueteRecibido);
	}

	if(cliente->eventos.error != NULL) (cliente->eventos.desconectado)();

	Socket_Destruir(cliente->socket);
	free(cliente->direccion);
	free(cliente);
}

// ===== Escuchar nuevas conexiones =====

static void EscucharConexiones(Servidor* servidor)
{
	while(1)
	{
		Cliente* nuevoCliente = Socket_AceptarConexion(servidor);
		if(nuevoCliente == NULL)
			break;

		if(servidor->eventos->conectado != NULL)
			servidor->eventos->conectado();

		pthread_create(&(nuevoCliente->thread), NULL, (void*)EscucharMensajes, nuevoCliente);
	}
	Eventos_Destruir(servidor->eventos);
}

// ===== Creación de sockets =====
Cliente* CrearCliente(char *ip, char* puerto, Eventos* eventos)
{
	int socketCliente = Socket_Crear(AF_UNSPEC, SOCK_STREAM, AI_PASSIVE);

	struct sockaddr_in* direccion = Socket_Conectar(socketCliente, ip, puerto);
	if (direccion == NULL)
	{
		freeaddrinfo((struct addrinfo*)direccion);
		Eventos_Destruir(eventos);
		Socket_Destruir(socketCliente);
		return NULL;
	}

	Cliente* cliente = malloc(sizeof(Cliente));
	cliente->socket = socketCliente;
	cliente->direccion = direccion;
	cliente->eventos = *eventos;
	pthread_create(&(cliente->thread), NULL, (void*)EscucharMensajes, cliente);

	Eventos_Destruir(eventos);
	return cliente;
}

Servidor* CrearServidor(uint16_t puerto, Eventos* eventos)
{
	int socketDeEscucha = Socket_Crear(AF_INET, SOCK_STREAM, 0);
	if (socketDeEscucha == -1)
		return NULL;

	Servidor* servidor = Socket_Escuchar(socketDeEscucha, puerto, eventos);
	if (servidor->socket != -1)
		pthread_create(&(servidor->thread), NULL, (void*)EscucharConexiones, servidor);

	return servidor;
}

