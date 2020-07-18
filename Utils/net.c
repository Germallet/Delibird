#include "net.h"
#include "socket.h"
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

// ===== Escuchar nuevos mensajes =====

static void EscucharMensajes(Cliente* cliente)
{
	//pthread_mutex_t* mutex = &(cliente->mx_destruir);
	int socket = cliente->socket;
	while(true)
	{
		Paquete* paqueteRecibido = NULL;
		if (Socket_RecibirPaquete(socket, &paqueteRecibido) <= 0)
		{
			/*if (paqueteRecibido == NULL)
			{
				//if(cliente->eventos->desconectado != NULL) (cliente->eventos->desconectado)(cliente);
				//DestruirCliente(cliente);
			}*/
			//else
			//if(cliente->eventos->error != NULL)
			//	(cliente->eventos->error)(ERROR_RECIBIR, paqueteRecibido);
			Paquete_Liberar(paqueteRecibido);
			//DestruirCliente(cliente);
			//cliente->socket = -1;
			break;
		}
		//pthread_mutex_lock(mutex);
		if (!Eventos_TieneOperacion(cliente->eventos, paqueteRecibido->codigoOperacion))
		{
			if(cliente->eventos->error != NULL)
				(cliente->eventos->error)(ERROR_OPERACION_INVALIDA, paqueteRecibido);
			//Paquete_Liberar(paqueteRecibido);
			//pthread_mutex_unlock(mutex);
			break;
		}
		if (Paquete_Procesar(cliente->socket, paqueteRecibido) == -1)
		{
			if(cliente->eventos->error != NULL)
				(cliente->eventos->error)(ERROR_PROCESAR_PAQUETE, paqueteRecibido);
			if(cliente->eventos->desconectado != NULL) (cliente->eventos->desconectado)(cliente);
			//DestruirCliente(cliente);
			//Paquete_Liberar(paqueteRecibido);
			//pthread_mutex_unlock(mutex);
			break;
		}
		Eventos_ObtenerOperacion(cliente->eventos, paqueteRecibido->codigoOperacion)(cliente, paqueteRecibido);
		//pthread_mutex_unlock(mutex);

		Paquete_Liberar(paqueteRecibido);
	}
	//pthread_mutex_destroy(mutex);
	//DestruirCliente2(cliente);
}

void DestruirClienteSV(Cliente* cliente)
{
	pthread_mutex_lock(&cliente->mx_destruir);
	//pthread_mutex_t copiaMutex = cliente->mx_destruir;
	pthread_mutex_t copiaMutex;
	pthread_mutex_init(&copiaMutex, NULL);
	memcpy(&cliente->mx_destruir, &copiaMutex, sizeof(copiaMutex));

	if (cliente->thread != NULL)
	{
		free(cliente->thread);
		Socket_Cerrar(cliente->socket);
		cliente->thread = NULL;
		cliente->socket = -1;
		pthread_mutex_unlock(&cliente->mx_destruir);
	}
	else
	{
		if (cliente->socket != -1)
			Socket_Destruir(cliente->socket);
		freeaddrinfo((struct addrinfo*)cliente->direccion);
		Eventos_Destruir(cliente->eventos);
		free(cliente);
		cliente = NULL;
	}
	pthread_mutex_unlock(&copiaMutex);
}
static void EscucharMensajesSV(Cliente* cliente)
{
	while(true)
	{
		Paquete* paqueteRecibido = NULL;
		if (Socket_RecibirPaquete(cliente->socket, &paqueteRecibido) <= 0)
		{
			if (paqueteRecibido == NULL)
			{
				if(cliente->eventos->desconectado != NULL) (cliente->eventos->desconectado)(cliente);
				//DestruirCliente(cliente);
			}
			//else
			//if(cliente->eventos->error != NULL)
			//	(cliente->eventos->error)(ERROR_RECIBIR, paqueteRecibido);
			Paquete_Liberar(paqueteRecibido);
			//DestruirCliente2(cliente);
			//cliente->socket = -1;
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
			//DestruirCliente2(cliente);
			break;
		}
		Eventos_ObtenerOperacion(cliente->eventos, paqueteRecibido->codigoOperacion)(cliente, paqueteRecibido);

		Paquete_Liberar(paqueteRecibido);
	}
	DestruirClienteSV(cliente);
}

// ===== Escuchar nuevas conexiones =====

static void EscucharConexiones(Servidor* servidor)
{
	while(true)
	{
		Cliente* nuevoCliente = Socket_AceptarConexion(servidor);
		if(nuevoCliente == NULL)
			break;

		if(servidor->eventos->conectado != NULL)
			servidor->eventos->conectado(nuevoCliente);

		nuevoCliente->thread = malloc(sizeof(pthread_t));
		pthread_create(nuevoCliente->thread, NULL, (void*)EscucharMensajesSV, nuevoCliente);
		//pthread_detach(*(nuevoCliente->thread));
	}

	//DestruirServidor(servidor);
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
		if(eventos != NULL)
		{
			Eventos_Destruir(eventos);
			eventos = NULL;
		}
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
	pthread_detach(*(cliente->thread));

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
		//pthread_detach(*(servidor->thread));
	}

	return servidor;
}

void DestruirCliente(Cliente* cliente)
{
	pthread_mutex_t mutex = cliente->mx_destruir;
	pthread_mutex_lock(&mutex);
	Socket_Cerrar(cliente->socket);
	Socket_Destruir(cliente->socket);
	pthread_join(*cliente->thread, NULL);
	Eventos_Destruir(cliente->eventos);
	freeaddrinfo((struct addrinfo*)cliente->direccion);
	free(cliente->thread);
	free(cliente);
	pthread_mutex_destroy(&mutex);

	/*pthread_mutex_lock(&cliente->mx_destruir);
	pthread_mutex_t copiaMutex = cliente->mx_destruir;
	if (cliente->thread != NULL)
	{
		free(cliente->thread);
		Socket_Cerrar(cliente->socket);
		cliente->thread = NULL;
		cliente->socket = -1;
	}
	else
	{
		Socket_Destruir(cliente->socket);
		freeaddrinfo((struct addrinfo*)cliente->direccion);
		Eventos_Destruir(cliente->eventos);
		free(cliente);
		cliente = NULL;
	}
	pthread_mutex_unlock(&copiaMutex);*/
}

void DestruirCliente2(Cliente* cliente)
{
	/*pthread_mutex_lock(&cliente->mx_destruir);
	//pthread_mutex_t copiaMutex = cliente->mx_destruir;
	pthread_mutex_t copiaMutex;
	pthread_mutex_init(&copiaMutex, NULL);
	memcpy(&cliente->mx_destruir, &copiaMutex, sizeof(copiaMutex));

	if (cliente->thread != NULL)
	{
		free(cliente->thread);
		Socket_Cerrar(cliente->socket);
		cliente->thread = NULL;
		cliente->socket = -1;
		pthread_mutex_unlock(&cliente->mx_destruir);
	}
	else
	{
		if (cliente->socket != -1)
			Socket_Destruir(cliente->socket);
		freeaddrinfo((struct addrinfo*)cliente->direccion);
		Eventos_Destruir(cliente->eventos);
		free(cliente);
		cliente = NULL;
	}
	pthread_mutex_unlock(&copiaMutex);*/


	if (cliente->socket != -1)
	{
		Socket_Cerrar(cliente->socket);
		Socket_Destruir(cliente->socket);
		cliente->socket = -1;
	}
	free(cliente->thread);
	Eventos_Destruir(cliente->eventos);
	freeaddrinfo((struct addrinfo*)cliente->direccion);
	free(cliente);
}

void DestruirServidor(Servidor* servidor)
{
	/*pthread_mutex_lock(&servidor->mx_destruir);
	pthread_mutex_t copiaMutex = servidor->mx_destruir;
	if (servidor->thread != NULL)
	{
		free(servidor->thread);
		Socket_Cerrar(servidor->socket);
		servidor->thread = NULL;
	}
	else
	{
		Socket_Destruir(servidor->socket);
		Eventos_Destruir(servidor->eventos);
		free(servidor);
	}
	pthread_mutex_unlock(&copiaMutex);*/

	Socket_Cerrar(servidor->socket);
	Socket_Destruir(servidor->socket);
	pthread_join(*servidor->thread, NULL);
	Eventos_Destruir(servidor->eventos);
	pthread_mutex_destroy(&servidor->mx_destruir);
	free(servidor->thread);
	free(servidor);
}

int EnviarMensaje(Cliente* cliente, CodigoDeOperacion codigoDeOperacion, void* datos, Serializador serializador)
{
	Stream* stream = serializador(datos);
	int respuesta = Socket_Enviar(codigoDeOperacion, stream->base, stream->tamanio, cliente->socket);
	Stream_DestruirConBuffer(stream);
	return respuesta;
}
int EnviarMensajeSinFree(Cliente* cliente, CodigoDeOperacion codigoDeOperacion, void* datos, Serializador serializador)
{
	Stream* stream = serializador(datos);
	int respuesta = Socket_Enviar(codigoDeOperacion, stream->base, stream->tamanio, cliente->socket);
	Stream_Destruir(stream);
	return respuesta;
}
