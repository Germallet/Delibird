#include "team.h"
#include "pokemon.h"
#include "entrenador.h"
#include "planificacion.h"
#include "interrupciones.h"

bool hay_entrenador_en_ejecucion() { return entrenador_EXEC != NULL; }
bool hay_entrenadores_READY() { return !list_is_empty(cola_READY); }
bool hay_pokemons_para_atrapar() { return !list_is_empty(pokemons_mapa); }
bool necesitamos_pokemons() { return !list_is_empty(pokemons_necesarios); }
bool hay_entrenadores_disponibles_para_atrapar() { return list_any_satisfy(cola_BLOCKED, &puede_planificarse_para_atrapar) || !list_is_empty(cola_NEW); }

void planificar_atrapar_pokemon()
{
	Entrenador* entrenador = NULL;
	Pokemon_Mapa* pokemon = NULL;

	pokemon_y_entrenador_mas_cercanos_entre_si(&pokemon, (void**) &entrenador);

	deshabilitar_entrenador(entrenador);

	Posicion* posicion = malloc(sizeof(Posicion));
	*posicion = pokemon->posicion;
	cargar_accion(entrenador, MOVER, posicion);

	char* especie = malloc(strlen(pokemon->especie)+1);
	strcpy(especie, pokemon->especie);
	cargar_accion(entrenador, CAPTURAR_POKEMON, especie);

	cambiar_estado_a(entrenador, READY); // METER ORDENADO PARA SJF ACA

	se_asigno_para_capturar(pokemon);
}

void planificar_intercambiar_pokemon_si_es_posible()
{
	Entrenador* entrenador_1 = NULL;
	Entrenador* entrenador_2 = NULL;

	obtener_entrenadores_que_pueden_intercambiar(&entrenador_1, &entrenador_2);

	if(entrenador_1 != NULL && entrenador_2 != NULL)
	{
		Posicion* posicion = malloc(sizeof(Posicion));
		*posicion = entrenador_2->posicion;

		cargar_accion(entrenador_1, MOVER, posicion);
		cargar_accion(entrenador_1, INTERCAMBIAR_POKEMON_EN_PROGRESO, entrenador_2);
		cargar_accion(entrenador_1, INTERCAMBIAR_POKEMON_EN_PROGRESO, entrenador_2);
		cargar_accion(entrenador_1, INTERCAMBIAR_POKEMON_EN_PROGRESO, entrenador_2);
		cargar_accion(entrenador_1, INTERCAMBIAR_POKEMON_EN_PROGRESO, entrenador_2);
		cargar_accion(entrenador_1, INTERCAMBIAR_POKEMON_FINALIZADO, entrenador_2);

		cambiar_estado_a(entrenador_1, READY);  // METER ORDENADO PARA SJF ACA
	}
}

void terminar_team()
{
	log_info(logger, "GAME OVER.");

	if(logger != NULL) log_destroy(logger);
	if(config != NULL) config_destroy(config);

	if(entrenador_EXEC!=NULL) destruir_entrenador(entrenador_EXEC);
	list_destroy(cola_NEW);
	list_destroy(cola_READY);
	list_destroy(cola_BLOCKED);
	list_destroy_and_destroy_elements(cola_EXIT, &destruir_entrenador);

	list_destroy_and_destroy_elements(cola_INTERRUPCIONES, &destruir_interrupcion);
	list_destroy_and_destroy_elements(pokemons_necesarios, &destruir_pokemon);
	list_destroy_and_destroy_elements(pokemons_mapa, &destruir_pokemon_mapa);

	exit(0);
}

void planificar_entrenador_si_es_necesario()
{
	while(hay_pokemons_para_atrapar() && hay_entrenadores_disponibles_para_atrapar())
		planificar_atrapar_pokemon();
	if(hay_entrenadores_que_podrian_intercambiar())
		planificar_intercambiar_pokemon_si_es_posible();
	if(!hay_entrenador_en_ejecucion() && hay_entrenadores_READY())
		cambiar_estado_a(tomar_entrenador(cola_READY), EXEC);
	if(!hay_entrenador_en_ejecucion() && !necesitamos_pokemons())
		terminar_team();
}
