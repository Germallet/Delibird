#include "mensaje.h"
#include "clienteBroker.h"
#include "broker.h"
#include "../Utils/socket.h"
#include "../Utils/protocolo.h"
#include "../Utils/serializacion.h"
#include <stdlib.h>
#include <pthread.h>

t_dictionaryInt* mensajes;
pthread_mutex_t mutexMensajes;

uint32_t siguienteIDMensaje = 0;
pthread_mutex_t mutexIDMensaje;

void IniciarMensajes()
{
	mensajes = dictionaryInt_create();
	pthread_mutex_init(&mutexMensajes, NULL);
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

Mensaje* CrearMensaje(CodigoDeCola tipoDeMensaje, uint32_t id, uint32_t idCorrelativo, size_t tamanio)
{
	Mensaje* mensaje = malloc(sizeof(Mensaje));
	mensaje->id = id;
	mensaje->idCorrelativo = idCorrelativo;
	mensaje->tamanio = tamanio;
	mensaje->clientesEnviados = list_create();
	mensaje->clientesACK = list_create();
	pthread_mutex_init(&(mensaje->mutexMensaje), NULL);

	pthread_mutex_lock(&mutexMensajes);
	dictionaryInt_put(mensajes, mensaje->id, mensaje);
	pthread_mutex_unlock(&mutexMensajes);

	return mensaje;
}

bool RegistrarACK(uint32_t idMensaje, void* clienteBroker)
{
	bool SonIguales(void* clienteBroker2) { return (ClienteBroker*)clienteBroker2 == clienteBroker; }

	pthread_mutex_lock(&mutexIDMensaje);
	Mensaje* mensaje = dictionaryInt_get(mensajes, idMensaje);
	pthread_mutex_unlock(&mutexIDMensaje);

	if (mensaje == NULL)
	{
		log_error(logger, "ERROR: ACK de mensaje inexistente");
		return false;
	}

	int modificacion = false;
	pthread_mutex_lock(&(mensaje->mutexMensaje));
	if(!list_any_satisfy(mensaje->clientesACK, &SonIguales))
	{
		list_add(mensaje->clientesEnviados, clienteBroker);
		return true;
	}
	pthread_mutex_unlock(&(mensaje->mutexMensaje));
	return modificacion;
}

bool Mensaje_SeLeEnvioA(Mensaje* mensaje, void* clienteBroker)
{
	bool sonIguales(void* clienteBroker2) { return (ClienteBroker*)clienteBroker2 == clienteBroker; }
	return list_any_satisfy(mensaje->clientesACK, &sonIguales);
}

void Mensaje_EnviarA(Mensaje* mensaje, void* contenido, Cliente* cliente)
{
	bool SonIguales(void* cliente2) { return (Cliente*)cliente2 == cliente; }

	if(cliente == NULL)
		return;

	int resultado = -1;
	Stream* streamLectura = Stream_CrearLectura(contenido, mensaje->tamanio);

	Stream* streamEscritura = Stream_CrearEscrituraNueva(sizeof(uint32_t) + mensaje->tamanio);
	Serializar_uint32(streamEscritura, mensaje->id);

	if(mensaje->tipoDeMensaje == COLA_NEW_POKEMON)
	{
		DATOS_NEW_POKEMON datosDeserializados = Deserializar_NEW_POKEMON(streamLectura);
		Serializar_NEW_POKEMON(streamEscritura, &datosDeserializados);
		resultado = Socket_Enviar(NEW_POKEMON, streamEscritura->base, streamEscritura->tamanio, cliente->socket);
	}
	else if(mensaje->tipoDeMensaje == COLA_APPEARED_POKEMON)
	{
		DATOS_APPEARED_POKEMON datosDeserializados = Deserializar_APPEARED_POKEMON(streamLectura);
		Serializar_uint32(streamEscritura, mensaje->idCorrelativo);
		Serializar_APPEARED_POKEMON(streamEscritura, &datosDeserializados);
		resultado = Socket_Enviar(APPEARED_POKEMON, streamEscritura->base, streamEscritura->tamanio, cliente->socket);
	}
	else if(mensaje->tipoDeMensaje == COLA_CATCH_POKEMON)
	{
		DATOS_CATCH_POKEMON datosDeserializados = Deserializar_CATCH_POKEMON(streamLectura);
		Serializar_CATCH_POKEMON(streamEscritura, &datosDeserializados);
		resultado = Socket_Enviar(CATCH_POKEMON, streamEscritura->base, streamEscritura->tamanio, cliente->socket);
	}
	else if(mensaje->tipoDeMensaje == COLA_CAUGHT_POKEMON)
	{
		DATOS_CAUGHT_POKEMON datosDeserializados = Deserializar_CAUGHT_POKEMON(streamLectura);
		Serializar_uint32(streamEscritura, mensaje->idCorrelativo);
		Serializar_CAUGHT_POKEMON(streamEscritura, &datosDeserializados);
		resultado = Socket_Enviar(CAUGHT_POKEMON, streamEscritura->base, streamEscritura->tamanio, cliente->socket);
	}
	else if(mensaje->tipoDeMensaje == COLA_GET_POKEMON)
	{
		DATOS_GET_POKEMON datosDeserializados = Deserializar_GET_POKEMON(streamLectura);
		Serializar_GET_POKEMON(streamEscritura, &datosDeserializados);
		resultado = Socket_Enviar(GET_POKEMON, streamEscritura->base, streamEscritura->tamanio, cliente->socket);
	}
	else if(mensaje->tipoDeMensaje == COLA_LOCALIZED_POKEMON)
	{
		DATOS_LOCALIZED_POKEMON datosDeserializados = Deserializar_LOCALIZED_POKEMON(streamLectura);
		Serializar_uint32(streamEscritura, mensaje->idCorrelativo);
		Serializar_LOCALIZED_POKEMON(streamEscritura, &datosDeserializados);
		resultado = Socket_Enviar(LOCALIZED_POKEMON, streamEscritura->base, streamEscritura->tamanio, cliente->socket);
	}
	Stream_Destruir(streamLectura);
	Stream_DestruirConBuffer(streamEscritura);

	log_info(logger, "Mensaje enviado (id: %d, cola: %s, cliente: %d)", mensaje->id, CodigoDeColaAString(mensaje->tipoDeMensaje), ((ClienteBroker*)cliente->info)->id);

	if (resultado > 0)
		list_add(mensaje->clientesEnviados, cliente);
	else
		log_error(logger, "Error al enviar mensaje (cola: %s, cliente: %d)", CodigoDeColaAString(mensaje->tipoDeMensaje), ((ClienteBroker*)cliente->info)->id);
}
