#include "clienteBroker.h"
#include <commons/collections/list.h>

typedef struct
{
	t_list* lista;
} Cola;

extern Cola *cola_NEW_POKEMON, *cola_APPEARED_POKEMON, *cola_CATCH_POKEMON, *cola_CAUGHT_POKEMON, *cola_GET_POKEMON, *cola_LOCALIZED_POKEMON;

void CrearColas();
void DestruirColas();
void AgregarSuscriptor(Cola* cola, ClienteBroker* clienteBroker);
void RemoverSuscriptor(ClienteBroker* clienteBroker);
