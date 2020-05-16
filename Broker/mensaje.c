#include "mensaje.h"
#include "clienteBroker.h"
#include "broker.h"
#include "../Utils/socket.h"
#include <stdlib.h>
#include <pthread.h>

uint32_t siguienteIDMensaje = 0;
pthread_mutex_t mutexIDMensaje;

void IniciarMensajes()
{
	pthread_mutex_init(&mutexIDMensaje, NULL);
}

uint32_t GenerarIDMensaje()
{
	pthread_mutex_lock(&mutexIDMensaje);
	uint32_t nuevaID = siguienteIDMensaje;
	siguienteIDMensaje++;
	pthread_mutex_unlock(&mutexIDMensaje);
	return nuevaID;
}

Mensaje* CrearMensaje(CodigoDeCola tipoDeMensaje, uint32_t id, size_t tamanio)
{
	Mensaje* mensaje = malloc(sizeof(Mensaje));
	mensaje->id = id;
	mensaje->tipoDeMensaje = tipoDeMensaje;
	mensaje->tamanio = tamanio;
	mensaje->clientesEnviados = list_create();
	mensaje->clientesACK = list_create();
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
		resultado = Socket_Enviar(NEW_POKEMON, contenido, mensaje->tamanio, cliente->socket);
	else if(mensaje->tipoDeMensaje == COLA_GET_POKEMON)
		resultado = Socket_Enviar(GET_POKEMON, contenido, mensaje->tamanio, cliente->socket);
	else if(mensaje->tipoDeMensaje == COLA_CATCH_POKEMON)
		resultado = Socket_Enviar(CATCH_POKEMON, contenido, mensaje->tamanio, cliente->socket);
	else if(mensaje->tipoDeMensaje == COLA_CAUGHT_POKEMON)
		resultado = Socket_Enviar(CAUGHT_POKEMON, contenido, mensaje->tamanio, cliente->socket);
	else if(mensaje->tipoDeMensaje == COLA_APPEARED_POKEMON)
		resultado = Socket_Enviar(APPEARED_POKEMON, contenido, mensaje->tamanio, cliente->socket);
	else if(mensaje->tipoDeMensaje == COLA_LOCALIZED_POKEMON)
		resultado = Socket_Enviar(LOCALIZED_POKEMON, contenido, mensaje->tamanio, cliente->socket);

	log_info(logger, "Mensaje enviado (cola: %s, cliente: %d)", CodigoDeColaAString(mensaje->tipoDeMensaje), ((ClienteBroker*)cliente->info)->id);

	if (resultado > 0)
		list_add(mensaje->clientesEnviados, cliente);
	else
		log_error(logger, "Error al enviar mensaje (cola: %s, cliente: %d)", CodigoDeColaAString(mensaje->tipoDeMensaje), ((ClienteBroker*)cliente->info)->id);

	return;

	// ?

	if(mensaje->tipoDeMensaje == COLA_NEW_POKEMON)
		resultado = EnviarMensajeSinFree(cliente, NEW_POKEMON, contenido, (Serializador)&SerializarM_NEW_POKEMON_ID);
	else if(mensaje->tipoDeMensaje == COLA_GET_POKEMON)
		resultado = EnviarMensajeSinFree(cliente, GET_POKEMON, contenido, (Serializador)&SerializarM_GET_POKEMON_ID);
	else if(mensaje->tipoDeMensaje == COLA_CATCH_POKEMON)
		resultado = EnviarMensajeSinFree(cliente, CATCH_POKEMON, contenido, (Serializador)&SerializarM_CATCH_POKEMON_ID);
	else if(mensaje->tipoDeMensaje == COLA_CAUGHT_POKEMON)
		resultado = EnviarMensajeSinFree(cliente, CAUGHT_POKEMON, contenido, (Serializador)&SerializarM_CAUGHT_POKEMON_ID);
	else if(mensaje->tipoDeMensaje == COLA_APPEARED_POKEMON)
		resultado = EnviarMensajeSinFree(cliente, APPEARED_POKEMON, contenido, (Serializador)&SerializarM_APPEARED_POKEMON_ID);
	else if(mensaje->tipoDeMensaje == COLA_LOCALIZED_POKEMON)
		resultado = EnviarMensajeSinFree(cliente, LOCALIZED_POKEMON, contenido, (Serializador)&SerializarM_LOCALIZED_POKEMON_ID);

	log_info(logger, "Mensaje enviado (cola: %s, cliente: %d)", CodigoDeColaAString(mensaje->tipoDeMensaje), ((ClienteBroker*)cliente->info)->id);

	if (resultado > 0)
		list_add(mensaje->clientesEnviados, cliente);
}
