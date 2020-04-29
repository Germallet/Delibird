#include "net.h"
#include "socket.h"
#include <pthread.h>
#include <stdlib.h>

// ===== Escuchar nuevos mensajes =====

static void EscucharMensajes(Cliente* cliente)
{
	while(cliente->thread != NULL)
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

	DestruirCliente(cliente);
}

// ===== Escuchar nuevas conexiones =====

static void EscucharConexiones(Servidor* servidor)
{
	while(servidor->thread != NULL)
	{
		Cliente* nuevoCliente = Socket_AceptarConexion(servidor);
		if(nuevoCliente == NULL)
			break;

		if(servidor->eventos->conectado != NULL)
			servidor->eventos->conectado();

		nuevoCliente->thread = malloc(sizeof(pthread_t));
		pthread_create(nuevoCliente->thread, NULL, (void*)EscucharMensajes, nuevoCliente);
		pthread_detach(*(nuevoCliente->thread));
	}

	DestruirServidor(servidor);
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

	cliente->thread = malloc(sizeof(pthread_t));
	pthread_create(cliente->thread, NULL, (void*)EscucharMensajes, cliente);
	pthread_detach(*(cliente->thread));

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
	{
		servidor->thread = malloc(sizeof(pthread_t));
		pthread_create(servidor->thread, NULL, (void*)EscucharConexiones, servidor);
		pthread_detach(*(servidor->thread));
	}

	return servidor;
}

void DestruirCliente(Cliente* cliente)
{
	if (cliente->thread != NULL)
	{
		cliente->thread = NULL;
		Socket_Cerrar(cliente->socket);
	}
	else
	{
		Socket_Destruir(cliente->socket);
		freeaddrinfo((struct addrinfo*)cliente->direccion);
		free(cliente);
	}
}

void DestruirServidor(Servidor* servidor)
{
	if (servidor->thread != NULL)
	{
		servidor->thread = NULL;
		Socket_Cerrar(servidor->socket);
	}
	else
	{
		Socket_Destruir(servidor->socket);
		Eventos_Destruir(servidor->eventos);
		free(servidor);
	}
}

