#include "broker.h"
#include "mensaje.h"
#include "clienteBroker.h"
#include "../Utils/dictionaryInt.h"
#include <stdlib.h>
#include <pthread.h>

void* memoria;
pthread_mutex_t mutexMemoria;

void IniciarMemoria(int tamanioMemoria)
{
	memoria = malloc(tamanioMemoria);
	pthread_mutex_init(&mutexMemoria, NULL);
}

void GuardarMensaje(Mensaje* mensaje, Stream* contenido)
{
	mensaje->contenido = malloc(contenido->tamanio);
	memcpy(mensaje->contenido, contenido->base, contenido->tamanio);
}

void* ObtenerContenidoMensaje(Mensaje* mensaje)
{
	return mensaje->contenido;
}

void DestruirMemoria()
{
	free(memoria);
}
