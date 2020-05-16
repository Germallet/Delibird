#include "broker.h"
#include "mensaje.h"
#include "clienteBroker.h"
#include "../Utils/dictionaryInt.h"
#include <stdlib.h>
#include <pthread.h>

t_dictionaryInt* mensajes;
pthread_mutex_t mutexMensajes;

void IniciarMemoria()
{
	mensajes = dictionaryInt_create();
	pthread_mutex_init(&mutexMensajes, NULL);
}

void GuardarMensaje(Mensaje* mensaje, Stream* contenido)
{
	pthread_mutex_lock(&mutexMensajes);
	dictionaryInt_put(mensajes, mensaje->id, mensaje);
	pthread_mutex_unlock(&mutexMensajes);

	mensaje->contenido = malloc(contenido->tamanio);
	memcpy(mensaje->contenido, contenido->base, contenido->tamanio);
}

void* ObtenerContenidoMensaje(Mensaje* mensaje)
{
	return mensaje->contenido;
}
