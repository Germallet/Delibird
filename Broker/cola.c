#include "cola.h"
#include "../Utils/dictionaryInt.h"
#include <stdlib.h>

t_dictionaryInt* diccionarioColas;
Cola *cola_NEW_POKEMON, *cola_APPEARED_POKEMON, *cola_CATCH_POKEMON, *cola_CAUGHT_POKEMON, *cola_GET_POKEMON, *cola_LOCALIZED_POKEMON;

static void CrearCola(CodigoDeCola codigo, Cola* cola)
{
	cola = malloc(sizeof(Cola));
	cola->lista = list_create();
}
void CrearColas()
{
	diccionarioColas = dictionaryInt_create();
	dictionaryInt_put(diccionarioColas, COLA_NEW_POKEMON, cola_NEW_POKEMON);
	dictionaryInt_put(diccionarioColas, COLA_APPEARED_POKEMON, cola_APPEARED_POKEMON);
	dictionaryInt_put(diccionarioColas, COLA_CATCH_POKEMON, cola_CATCH_POKEMON);
	dictionaryInt_put(diccionarioColas, COLA_CAUGHT_POKEMON, cola_CAUGHT_POKEMON);
	dictionaryInt_put(diccionarioColas, COLA_GET_POKEMON, cola_GET_POKEMON);
	dictionaryInt_put(diccionarioColas, COLA_LOCALIZED_POKEMON, cola_LOCALIZED_POKEMON);
	dictionaryInt_iterator(diccionarioColas, (void(*)(uint32_t,void*))&CrearCola);
}

static void DestruirCola(CodigoDeCola codigo, Cola* cola)
{
	list_destroy(cola->lista);
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
	list_add(cola->lista, clienteBroker);
}

void RemoverSuscriptor(ClienteBroker* clienteBroker)
{
	bool(SonIguales)(void* clienteBroker2) { return (ClienteBroker*)clienteBroker2 == clienteBroker; }
	list_remove_by_condition(cola_NEW_POKEMON->lista, &SonIguales);
	list_remove_by_condition(cola_APPEARED_POKEMON->lista, &SonIguales);
	list_remove_by_condition(cola_CATCH_POKEMON->lista, &SonIguales);
	list_remove_by_condition(cola_CAUGHT_POKEMON->lista, &SonIguales);
	list_remove_by_condition(cola_GET_POKEMON->lista, &SonIguales);
	list_remove_by_condition(cola_LOCALIZED_POKEMON->lista, &SonIguales);
}
