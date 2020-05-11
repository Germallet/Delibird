#include "team.h"
#include "pokemon.h"
#include "entrenador.h"
#include "planificacion.h"

bool hay_entrenador_en_ejecucion() { return entrenador_EXEC != NULL; }
bool hay_pokemons_para_atrapar() { return !list_is_empty(pokemons_mapa); }
bool hay_entrenadores_disponibles() { return !list_is_empty(entrenadores_disponibles()); }

void planificar_atrapar_pokemon()
{
	Entrenador* entrenador = NULL;
	Pokemon_Mapa* pokemon = NULL;

	pokemon_y_entrenador_mas_cercanos_entre_si(una_especie_que_necesito_y_esta_en_mapa(), pokemon, entrenador);

	cargar_accion(entrenador, MOVER, pokemon->posicion);
	cargar_accion(entrenador, CAPTURAR_POKEMON, pokemon->especie);

	cambiar_estado_a(entrenador, EXEC);
}

void planificar_intercambiar_pokemon()
{
	Entrenador* entrenador_1 = NULL;
	Entrenador* entrenador_2 = NULL;

	//obtener_entrenadores_que_pueden_intercambiar(entrenador_1, entrenador_2); TODO

	cargar_accion(entrenador_1, MOVER, entrenador_2->posicion);
	cargar_accion(entrenador_1, INTERCAMBIAR_POKEMON, entrenador_2);

	cambiar_estado_a(entrenador_1, EXEC);
}
