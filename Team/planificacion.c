#include "team.h"
#include "pokemon.h"
#include "posicion.h"
#include "entrenador.h"
#include "planificacion.h"
#include "interrupciones.h"

int quantum;
bool deadlocks_estan_identificados = false;
t_list* deadlocks;

bool hay_entrenadores_READY() { return !list_is_empty(cola_READY); }
bool hay_pokemons_para_atrapar() { return !list_is_empty(pokemons_mapa); }
bool necesitamos_pokemons() { return !list_is_empty(pokemons_necesarios); }
bool hay_entrenadores_disponibles_para_atrapar() { return list_any_satisfy(cola_BLOCKED, &puede_planificarse_para_atrapar) || !list_is_empty(cola_NEW); }
bool es_planificacion_tipo(char* tipo) { return strcmp(config_get_string_value(config, "ALGORITMO_PLANIFICACION"), tipo)==0; }

Deadlock* crear_deadlock(Entrenador* entrenador1, Entrenador* entrenador2, char* pokemon1, char* pokemon2)
{
	Deadlock* deadlock = malloc(sizeof(Deadlock));
	deadlock->entrenador1 = entrenador1;
	deadlock->entrenador2 = entrenador2;
	deadlock->pokemon1 = malloc(strlen(pokemon1)+1);
	strcpy(deadlock->pokemon1, pokemon1);
	deadlock->pokemon2 = malloc(strlen(pokemon2)+1);
	strcpy(deadlock->pokemon2, pokemon2);
	return deadlock;
}

void destruir_deadlock(Deadlock* deadlock)
{
	free(((Deadlock*) deadlock)->pokemon1);
	free(((Deadlock*) deadlock)->pokemon2);
	free(deadlock);
}

void tomar_deadlock(Entrenador** entrenador1, Entrenador** entrenador2)
{
	Deadlock* deadlock = list_remove(deadlocks, 0);

	*entrenador1 = deadlock->entrenador1;
	*entrenador2 = deadlock->entrenador2;

	if(esta_en_EXIT(*entrenador1) || esta_en_EXIT(*entrenador2))
	{
		list_destroy_and_destroy_elements(deadlocks, (void*) &destruir_deadlock);
		destruir_deadlock(deadlock);
		deadlocks = list_create();
		identificar_deadlocks();
		deadlock = list_remove(deadlocks, 0);
		*entrenador1 = deadlock->entrenador1;
		*entrenador2 = deadlock->entrenador2;
	}

	(*entrenador1)->info = malloc(strlen(deadlock->pokemon1)+1);
	strcpy((*entrenador1)->info, deadlock->pokemon1);

	*entrenador2 = deadlock->entrenador2;
	(*entrenador2)->info = malloc(strlen(deadlock->pokemon2)+1);
	strcpy((*entrenador2)->info, deadlock->pokemon2);

	destruir_deadlock(deadlock);
}

static int ciclos_estimados(Deadlock* deadlock)
{
	int distancia = distancia_entre(deadlock->entrenador1->posicion, deadlock->entrenador2->posicion);
	return distancia + 5;
}

void reiniciar_quantum() { quantum = 0;}

void aumentar_quantum()
{
	quantum++;
	if(quantum == config_get_int_value(config, "QUANTUM"))
	{
		cambiar_estado_a(entrenador_EXEC, READY, FIN_QUANTUM);
		reiniciar_quantum();
	}
}

static void insertar_FIFO(Deadlock* deadlock) { list_add(deadlocks, deadlock); }
void insertar_SJF(Deadlock* deadlock)
{
	int index = 0;
	Deadlock* pivot = list_get(deadlocks, index);
	while(pivot != NULL && ciclos_estimados(pivot)<=ciclos_estimados(deadlock))
	{
		index++;
		pivot = list_get(deadlocks, index);
	}
	list_add_in_index(deadlocks, index, deadlock);
}

void agregar_deadlock(Entrenador* entrenador1, Entrenador* entrenador2, char* pokemon1, char* pokemon2)
{
	if(es_planificacion_tipo("FIFO") || es_planificacion_tipo("RR"))
		insertar_FIFO(crear_deadlock(entrenador1, entrenador2, pokemon1, pokemon2));
	if(es_planificacion_tipo("SJF-SD") || es_planificacion_tipo("SJF-CD"))
		insertar_SJF(crear_deadlock(entrenador1, entrenador2, pokemon1, pokemon2));
	log_info(logger, "Los entrenadores %d y %d estan en deadlock", entrenador1->ID, entrenador2->ID);
}

void identificar_deadlocks()
{
	t_list* entrenadores_pivot = obtener_entrenadores_que_podrian_intercambiar();

	Entrenador* entrenador1;
	Entrenador* entrenador2;
	t_list* entrenadores_intercambiar;
	char* pokemon1;
	char* pokemon2;
	while(entrenadores_pivot->elements_count>1)
	{
		entrenador1 = list_remove(entrenadores_pivot, 0);
		entrenadores_intercambiar = obtener_entrenadores_que_pueden_intercambiar_con(entrenador1, entrenadores_pivot);
		for(int i=0;i<entrenadores_intercambiar->elements_count;i++)
		{
			entrenador2 = list_get(entrenadores_intercambiar, i);
			pokemons_que_van_a_intercambiar(entrenador1, entrenador2, &pokemon1, &pokemon2);
			agregar_deadlock(entrenador1, entrenador2, pokemon1, pokemon2);
			free(pokemon1);
			free(pokemon2);
		}
		list_destroy(entrenadores_intercambiar);
	}

	list_destroy(entrenadores_pivot);
	deadlocks_estan_identificados = true;
}

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

	cambiar_estado_a(entrenador, READY, CAPTURA);

	se_asigno_para_capturar(pokemon);
}

void planificar_intercambiar_pokemon()
{
	Entrenador* entrenador_1 = NULL;
	Entrenador* entrenador_2 = NULL;

	tomar_deadlock(&entrenador_1, &entrenador_2);

	Posicion* posicion = malloc(sizeof(Posicion));
	*posicion = entrenador_2->posicion;

	cargar_accion(entrenador_1, MOVER, posicion);
	cargar_accion(entrenador_1, INTERCAMBIAR_POKEMON_EN_PROGRESO, entrenador_2);
	cargar_accion(entrenador_1, INTERCAMBIAR_POKEMON_EN_PROGRESO, entrenador_2);
	cargar_accion(entrenador_1, INTERCAMBIAR_POKEMON_EN_PROGRESO, entrenador_2);
	cargar_accion(entrenador_1, INTERCAMBIAR_POKEMON_EN_PROGRESO, entrenador_2);
	cargar_accion(entrenador_1, INTERCAMBIAR_POKEMON_FINALIZADO, entrenador_2);

	cambiar_estado_a(entrenador_1, READY, INTERCAMBIO);
}

void terminar_team()
{
	log_info(logger, "El Team atrapo todos los pokemons que necesitaban sus entrenadores en %d ciclos.", cantidad_ciclos);

	if(logger != NULL) log_destroy(logger);
	if(config != NULL) config_destroy(config);
	list_destroy(deadlocks);

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
	if(!hay_entrenador_en_ejecucion() && hay_entrenadores_READY())
		poner_entrenador_en_EXEC();
	if(estamos_en_deadlock())
	{
		if(!deadlocks_estan_identificados)
			identificar_deadlocks();
		if(!list_is_empty(deadlocks))
		{
			planificar_intercambiar_pokemon();
			poner_entrenador_en_EXEC();
		}
	}
	if(!hay_entrenador_en_ejecucion() && !necesitamos_pokemons())
		terminar_team();
}
