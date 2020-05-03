#include "cola.h"
#include <stdlib.h>

Cola *cola_NEW_POKEMON, *cola_APPEARED_POKEMON, *cola_CATCH_POKEMON, *cola_CAUGHT_POKEMON, *cola_GET_POKEMON, *cola_LOCALIZED_POKEMON;

static void CrearCola(Cola* cola)
{
	cola = malloc(sizeof(Cola));
	cola->lista = list_create();
}
void CrearColas()
{
	CrearCola(cola_NEW_POKEMON);
	CrearCola(cola_APPEARED_POKEMON);
	CrearCola(cola_CATCH_POKEMON);
	CrearCola(cola_CAUGHT_POKEMON);
	CrearCola(cola_GET_POKEMON);
	CrearCola(cola_LOCALIZED_POKEMON);
}

static void DestruirCola(Cola* cola)
{
	list_destroy(cola->lista);
	free(cola);
}
void DestruirColas()
{
	DestruirCola(cola_NEW_POKEMON);
	DestruirCola(cola_APPEARED_POKEMON);
	DestruirCola(cola_CATCH_POKEMON);
	DestruirCola(cola_CAUGHT_POKEMON);
	DestruirCola(cola_GET_POKEMON);
	DestruirCola(cola_LOCALIZED_POKEMON);
}

void AgregarSuscriptor(Cola* cola, ClienteBroker* clienteBroker)
{
	list_add(cola->lista, clienteBroker);
}

void RemoverSuscriptor(ClienteBroker* clienteBroker)
{
	bool(SonIguales)(void* clienteBroker2) { return (ClienteBroker*)clienteBroker2 == clienteBroker; }
	list_remove_by_condition(cola_NEW_POKEMON->lista, (bool(*)(void*))&SonIguales);
	list_remove_by_condition(cola_APPEARED_POKEMON->lista, &SonIguales);
	list_remove_by_condition(cola_CATCH_POKEMON->lista, &SonIguales);
	list_remove_by_condition(cola_CAUGHT_POKEMON->lista, &SonIguales);
	list_remove_by_condition(cola_GET_POKEMON->lista, &SonIguales);
	list_remove_by_condition(cola_LOCALIZED_POKEMON->lista, &SonIguales);
}
