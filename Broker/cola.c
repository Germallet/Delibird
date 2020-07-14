#include "cola.h"
#include "memoria.h"
#include "../Utils/dictionaryInt.h"
#include <stdlib.h>

t_dictionaryInt* diccionarioColas;
Cola *cola_NEW_POKEMON, *cola_APPEARED_POKEMON, *cola_CATCH_POKEMON, *cola_CAUGHT_POKEMON, *cola_GET_POKEMON, *cola_LOCALIZED_POKEMON;

static void CrearCola(CodigoDeCola codigo, Cola** cola)
{
	*cola = malloc(sizeof(Cola));
	(*cola)->suscriptores = list_create();
	(*cola)->mensajes = list_create();
	pthread_mutex_init(&((*cola)->mutexSuscriptores), NULL);
	pthread_mutex_init(&((*cola)->mutexMensajes), NULL);
}
void CrearColas()
{
	diccionarioColas = dictionaryInt_create();

	CrearCola(COLA_NEW_POKEMON, &cola_NEW_POKEMON);
	CrearCola(COLA_APPEARED_POKEMON, &cola_APPEARED_POKEMON);
	CrearCola(COLA_CATCH_POKEMON, &cola_CATCH_POKEMON);
	CrearCola(COLA_CAUGHT_POKEMON, &cola_CAUGHT_POKEMON);
	CrearCola(COLA_GET_POKEMON, &cola_GET_POKEMON);
	CrearCola(COLA_LOCALIZED_POKEMON, &cola_LOCALIZED_POKEMON);

	dictionaryInt_put(diccionarioColas, COLA_NEW_POKEMON, cola_NEW_POKEMON);
	dictionaryInt_put(diccionarioColas, COLA_APPEARED_POKEMON, cola_APPEARED_POKEMON);
	dictionaryInt_put(diccionarioColas, COLA_CATCH_POKEMON, cola_CATCH_POKEMON);
	dictionaryInt_put(diccionarioColas, COLA_CAUGHT_POKEMON, cola_CAUGHT_POKEMON);
	dictionaryInt_put(diccionarioColas, COLA_GET_POKEMON, cola_GET_POKEMON);
	dictionaryInt_put(diccionarioColas, COLA_LOCALIZED_POKEMON, cola_LOCALIZED_POKEMON);
}

static void DestruirCola(CodigoDeCola codigo, Cola* cola)
{
	list_destroy(cola->suscriptores);
	list_destroy(cola->mensajes);
	free(cola);
	dictionaryInt_destroy(diccionarioColas);
}
void DestruirColas()
{
	dictionaryInt_iterator(diccionarioColas, (void(*)(uint32_t,void*))&DestruirCola);
}

Cola* ObtenerCola(CodigoDeCola codigo)
{
	return dictionaryInt_get(diccionarioColas, codigo);
}

void AgregarSuscriptor(Cola* cola, ClienteBroker* clienteBroker)
{
	pthread_mutex_lock(&(cola->mutexSuscriptores));
	list_add(cola->suscriptores, clienteBroker);
	pthread_mutex_unlock(&(cola->mutexSuscriptores));
}

void RemoverSuscriptor(Cola* cola, ClienteBroker* clienteBroker)
{
	bool SonIguales(void* clienteBroker2) { return (ClienteBroker*)clienteBroker2 == clienteBroker; }

	pthread_mutex_lock(&(cola->mutexSuscriptores));
	list_remove_by_condition(cola->suscriptores, &SonIguales);
	pthread_mutex_unlock(&(cola->mutexSuscriptores));
}

void RemoverSuscriptorTotal(ClienteBroker* clienteBroker)
{
	RemoverSuscriptor(cola_NEW_POKEMON, clienteBroker);
	RemoverSuscriptor(cola_APPEARED_POKEMON, clienteBroker);
	RemoverSuscriptor(cola_CATCH_POKEMON, clienteBroker);
	RemoverSuscriptor(cola_CAUGHT_POKEMON, clienteBroker);
	RemoverSuscriptor(cola_GET_POKEMON, clienteBroker);
	RemoverSuscriptor(cola_LOCALIZED_POKEMON, clienteBroker);
}

bool TieneSuscriptor(Cola* cola, ClienteBroker* clienteBroker)
{
	bool SonIguales(void* clienteBroker2) { return (ClienteBroker*)clienteBroker2 == clienteBroker; }

	pthread_mutex_lock(&(cola->mutexSuscriptores));
	bool existe = list_any_satisfy(cola->suscriptores, &SonIguales);
	pthread_mutex_unlock(&(cola->mutexSuscriptores));

	return existe;
}

bool CorresponderRecibirRespuesta(CodigoDeCola codigo, uint32_t idCorrelativo)
{
	if (idCorrelativo == -1)
		return true;

	bool MismoCorrelativo(void* mensaje) { return ((Mensaje*)mensaje)->idCorrelativo == idCorrelativo;}
	Cola* cola = ObtenerCola(codigo);
	bool resultado;

	pthread_mutex_lock(&(cola->mutexMensajes));
	resultado = !list_any_satisfy(cola->mensajes, &MismoCorrelativo);
	pthread_mutex_unlock(&(cola->mutexMensajes));

	return resultado;
}

static void BroadcastMensajeSinChequeo(Cola* cola, CodigoDeCola codigoDeCola, Mensaje* mensaje)
{
	if (mensaje->particion == NULL)
		return;

	void* contenido = ObtenerContenidoMensaje(mensaje);
	void EnviarMensajeA(void* cliente) { Mensaje_EnviarA(mensaje, codigoDeCola, contenido, ((ClienteBroker*)cliente)->cliente); }

	pthread_mutex_lock(&(cola->mutexSuscriptores));
	list_iterate(cola->suscriptores, &EnviarMensajeA);
	pthread_mutex_unlock(&(cola->mutexSuscriptores));

	free(contenido);
}
void Cola_ProcesarNuevoMensaje(CodigoDeCola codigoDeCola, Mensaje* mensaje)
{
	Cola* cola = ObtenerCola(codigoDeCola);

	void* contenido = ObtenerContenidoMensaje(mensaje);
	void EnviarMensajeA(void* cliente) { Mensaje_EnviarA(mensaje, codigoDeCola, contenido, ((ClienteBroker*)cliente)->cliente); }

	pthread_mutex_lock(&(cola->mutexMensajes));
	list_add(cola->mensajes, mensaje);
	pthread_mutex_unlock(&(cola->mutexMensajes));

	BroadcastMensajeSinChequeo(cola, codigoDeCola, mensaje);
	free(contenido);
}

void Cola_EnviarMensajesRestantesSiCorrespondeA(Cola* cola, CodigoDeCola codigoDeCola, ClienteBroker* cliente)
{
	void EnviarMensajeSiCorresponde(void* mensaje)
	{
		if (((Mensaje*)mensaje)->particion == NULL)
			return;
		if (Mensaje_SeLeEnvioA(mensaje, cliente))
			return;
		void* contenido = ObtenerContenidoMensaje(mensaje);
		Mensaje_EnviarA(mensaje, codigoDeCola, contenido, cliente->cliente);
		free(contenido);
	}

	if (!TieneSuscriptor(cola, cliente))
		return;

	pthread_mutex_lock(&(cola->mutexMensajes));
	list_iterate(cola->mensajes, &EnviarMensajeSiCorresponde);
	pthread_mutex_unlock(&(cola->mutexMensajes));
}

void Colas_EnviarMensajesRestantesSiCorrespondeA(ClienteBroker* cliente)
{
	Cola_EnviarMensajesRestantesSiCorrespondeA(cola_NEW_POKEMON, COLA_NEW_POKEMON, cliente);
	Cola_EnviarMensajesRestantesSiCorrespondeA(cola_APPEARED_POKEMON, COLA_APPEARED_POKEMON, cliente);
	Cola_EnviarMensajesRestantesSiCorrespondeA(cola_CATCH_POKEMON, COLA_CATCH_POKEMON, cliente);
	Cola_EnviarMensajesRestantesSiCorrespondeA(cola_CAUGHT_POKEMON, COLA_CAUGHT_POKEMON, cliente);
	Cola_EnviarMensajesRestantesSiCorrespondeA(cola_GET_POKEMON, COLA_GET_POKEMON, cliente);
	Cola_EnviarMensajesRestantesSiCorrespondeA(cola_LOCALIZED_POKEMON, COLA_LOCALIZED_POKEMON, cliente);
}
