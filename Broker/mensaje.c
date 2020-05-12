#include "mensaje.h"
#include "clienteBroker.h"
#include <stdlib.h>
#include <pthread.h>

uint32_t siguienteIDMensaje = 0;
pthread_mutex_t mutexIDMensaje;
// Que hacer si no llega ACK?
// Loguear mensajes que envio?
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

bool Mensaje_SeLeEnvioA(Mensaje* mensaje, void* cliente)
{
	bool sonIguales(void* cliente2) { return cliente == cliente2; }
	return list_any_satisfy(mensaje->clientesACK, &sonIguales);
}
void Mensaje_EnviarA(Mensaje* mensaje, void* contenido, Cliente* cliente)
{
	if(cliente == NULL)
		return;

	int resultado = -1;
	if(mensaje->tipoDeMensaje == COLA_NEW_POKEMON)
		resultado = EnviarMensajeSinFree(cliente, NEW_POKEMON, contenido, (Serializador)&Serializar_NEW_POKEMON);
	else if(mensaje->tipoDeMensaje == COLA_GET_POKEMON)
		resultado = EnviarMensajeSinFree(cliente, GET_POKEMON, contenido, (Serializador)&Serializar_GET_POKEMON);
	else if(mensaje->tipoDeMensaje == COLA_CATCH_POKEMON)
		resultado = EnviarMensajeSinFree(cliente, CATCH_POKEMON, contenido, (Serializador)&Serializar_CATCH_POKEMON);
	else if(mensaje->tipoDeMensaje == COLA_CAUGHT_POKEMON)
		resultado = EnviarMensajeSinFree(cliente, CAUGHT_POKEMON, contenido, (Serializador)&Serializar_CAUGHT_POKEMON_ID);
	else if(mensaje->tipoDeMensaje == COLA_APPEARED_POKEMON)
		resultado = EnviarMensajeSinFree(cliente, APPEARED_POKEMON, contenido, (Serializador)&Serializar_APPEARED_POKEMON);
	else if(mensaje->tipoDeMensaje == COLA_LOCALIZED_POKEMON)
		resultado = EnviarMensajeSinFree(cliente, LOCALIZED_POKEMON, contenido, (Serializador)&Serializar_LOCALIZED_POKEMON_ID);

	if (resultado > 0)
		list_add(mensaje->clientesEnviados, cliente);
}
