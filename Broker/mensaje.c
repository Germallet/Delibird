#include "mensaje.h"
#include <stdlib.h>
#include <pthread.h>

uint32_t siguienteIDMensaje = 0;
pthread_mutex_t mutexIDMensaje;

void IniciarMensajes()
{
	pthread_mutex_init(&mutexIDMensaje, NULL);
}

Mensaje* CrearMensaje(CodigoDeCola tipoDeMensaje, void* contenido)
{
	Mensaje* mensaje = malloc(sizeof(Mensaje));
	mensaje->tipoDeMensaje = tipoDeMensaje;
	mensaje->contenido = contenido;
	mensaje->clientesEnviados = list_create();
	mensaje->clientesACK = list_create();

	pthread_mutex_lock(&mutexIDMensaje);
	mensaje->id = siguienteIDMensaje;
	siguienteIDMensaje++;
	pthread_mutex_unlock(&mutexIDMensaje);

	return mensaje;
}
