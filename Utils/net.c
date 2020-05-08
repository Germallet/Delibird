#include "net.h"
#include "socket.h"
#include <pthread.h>
#include <stdlib.h>

// ===== Escuchar nuevos mensajes =====

static void EscucharMensajes(Cliente* cliente)
{
	while(true)
	{
		Paquete* paqueteRecibido = NULL;
		if (Socket_RecibirPaquete(cliente->socket, &paqueteRecibido) <= 0)
		{
			if (paqueteRecibido == NULL)
			{
				if(cliente->eventos->desconectado != NULL) (cliente->eventos->desconectado)(cliente);
				DestruirCliente(cliente);
			}
			else
				if(cliente->eventos->error != NULL)
					(cliente->eventos->error)(ERROR_RECIBIR, paqueteRecibido);
			Paquete_Liberar(paqueteRecibido);
			break;
		}
		if (!Eventos_TieneOperacion(cliente->eventos, paqueteRecibido->codigoOperacion))
		{
			if(cliente->eventos->error != NULL)
				(cliente->eventos->error)(ERROR_OPERACION_INVALIDA, paqueteRecibido);
			break;
		}
		if (Paquete_Procesar(cliente->socket, paqueteRecibido) == -1)
		{
			if(cliente->eventos->error != NULL)
				(cliente->eventos->error)(ERROR_PROCESAR_PAQUETE, paqueteRecibido);
			if(cliente->eventos->desconectado != NULL) (cliente->eventos->desconectado)(cliente);
			DestruirCliente(cliente);
			break;
		}
		Eventos_ObtenerOperacion(cliente->eventos, paqueteRecibido->codigoOperacion)(cliente, paqueteRecibido);

		Paquete_Liberar(paqueteRecibido);
	}
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
			servidor->eventos->conectado(nuevoCliente);

		nuevoCliente->thread = malloc(sizeof(pthread_t));
		pthread_create(nuevoCliente->thread, NULL, (void*)EscucharMensajes, nuevoCliente);
		pthread_detach(*(nuevoCliente->thread));
	}

	DestruirServidor(servidor);
}

// ===== Creación de sockets =====
Cliente* CrearCliente(char *ip, uint16_t puerto, Eventos* eventos)
{
	int socketCliente = Socket_Crear(ip, puerto);
	if (socketCliente == -1)
	{
		Eventos_Destruir(eventos);
		return NULL;
	}

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
	cliente->eventos = eventos;
	cliente->thread = malloc(sizeof(pthread_t));
	cliente->info = NULL;
	pthread_mutex_init(&cliente->mx_destruir, NULL);

	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	pthread_create(cliente->thread, &attr, (void*)EscucharMensajes, cliente);
	//pthread_detach(*(cliente->thread));

	return cliente;
}

Servidor* CrearServidor(char* ip, uint16_t puerto, Eventos* eventos)
{
	int socketDeEscucha = Socket_Crear(ip, puerto);
	if (socketDeEscucha == -1)
		return NULL;

	Servidor* servidor = Socket_Escuchar(ip, socketDeEscucha, puerto, eventos);
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
	pthread_mutex_lock(&cliente->mx_destruir);
	pthread_mutex_t copiaMutex = cliente->mx_destruir;
	if (cliente->thread != NULL)
	{
		cliente->thread = NULL;
		Socket_Cerrar(cliente->socket);
	}
	else
	{
		Socket_Destruir(cliente->socket);
		freeaddrinfo((struct addrinfo*)cliente->direccion);
		Eventos_Destruir(cliente->eventos);
		free(cliente);
		cliente = NULL;
	}
	pthread_mutex_unlock(&copiaMutex);
}

void DestruirServidor(Servidor* servidor)
{
	pthread_mutex_lock(&servidor->mx_destruir);
	pthread_mutex_t copiaMutex = servidor->mx_destruir;
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
	pthread_mutex_unlock(&copiaMutex);
}

