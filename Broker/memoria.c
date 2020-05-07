#include "broker.h"
#include "mensaje.h"
#include "../Utils/dictionaryInt.h"
#include <stdlib.h>
#include <pthread.h>

t_dictionaryInt* mensajes;
pthread_mutex_t mutexMensajes;

uint32_t siguienteIDMensaje = 0;
pthread_mutex_t mutexIDMensaje;

void IniciarMemoria()
{
	mensajes = dictionaryInt_create();
	pthread_mutex_init(&mutexMensajes, NULL);
	pthread_mutex_init(&mutexIDMensaje, NULL);
	log_info(logger, "Hola, soy la memoria y me inicié");
}

void CrearMensaje(CodigoDeCola tipoDeMensaje, void* contenido)
{
	Mensaje* mensaje = malloc(sizeof(Mensaje));

	pthread_mutex_lock(&mutexIDMensaje);
	mensaje->id = siguienteIDMensaje;
	siguienteIDMensaje++;
	pthread_mutex_unlock(&mutexIDMensaje);

	mensaje->tipoDeMensaje = tipoDeMensaje;
	mensaje->contenido = contenido;

	pthread_mutex_lock(&mutexMensajes);
	dictionaryInt_put(mensajes, mensaje->id, mensaje);
	pthread_mutex_unlock(&mutexMensajes);
}
