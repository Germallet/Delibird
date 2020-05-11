#include "team.h"
#include "pokemon.h"
#include "entrenador.h"
#include "planificacion.h"
#include "interrupciones.h"
#include <stdlib.h>
#include <pthread.h>

t_dictionaryInt* diccionario_interrupciones;
t_list* cola_INTERRUPCIONES;
pthread_mutex_t mutex_interrupciones;

void inicializar_diccionario_interrupciones();

//-----------COLA DE INTERRUPCIONES-----------//
void agregar_interrupcion(Datos_Interrupcion* interrupcion)
{
	pthread_mutex_lock(&mutex_interrupciones);
	list_add(cola_INTERRUPCIONES, interrupcion);
	pthread_mutex_unlock(&mutex_interrupciones);
}
void destruir_interrupcion(void* interrupcion)
{
	free(((Datos_Interrupcion*) interrupcion)->info);
	free(interrupcion);
}

void inicializar_interrupciones()
{
	inicializar_diccionario_interrupciones();

	Datos_Interrupcion* planificacion_inicial = malloc(sizeof(Datos_Interrupcion));
	planificacion_inicial->tipo = I_PLANIFICAR;
	planificacion_inicial->info = NULL;

	cola_INTERRUPCIONES = list_create();
	agregar_interrupcion(planificacion_inicial);

	pthread_mutex_init(&(mutex_interrupciones), NULL);
}

//-----------OTRAS FUNCIONES-----------//
void ejecutar_interrupcion()
{
	Datos_Interrupcion* interrupcion_actual = ((Datos_Interrupcion*) cola_INTERRUPCIONES->head->data);
	((Interrupcion) dictionaryInt_get(diccionario_interrupciones, interrupcion_actual->tipo)) (interrupcion_actual->info);
	list_remove_and_destroy_element(cola_INTERRUPCIONES, 0, &destruir_interrupcion);
}

bool hay_interrupciones_para_ejecutar() { return !list_is_empty(cola_INTERRUPCIONES); }

//-----------INTERRUPCIONES-----------//
void interrupcion_PLANIFICAR(void* dato)
{
	if(!hay_entrenador_en_ejecucion())
	{
		//if(!list_is_empty(cola_READY)) entrenador_EXEC = tomar_entrenador(cola_READY); //
	}
	if(hay_pokemons_para_atrapar())
	{
		if(hay_entrenadores_disponibles()) //Hay entrenadores disponibles
		{
			//planificar_entrenador(); asignar_pokemon_a_atrapar(char* especie = tomar_pokemons_necesario_que_este_en_mapa()) )
		}
	}
}
void interrupcion_CAUGHT_POKEMON(void* dato) {}
void interrupcion_APPEARED_POKEMON(void* dato) {}
void interrupcion_LOCALIZED_POKEMON(void* dato) {}
void interrupcion_ASIGNACION_ID(void* dato) {}
void interrupcion_TERMINAR(void* dato)
{
	if(logger != NULL) log_destroy(logger);
	if(config != NULL) config_destroy(config);

	if(entrenador_EXEC!=NULL) destruir_entrenador(entrenador_EXEC);
	list_destroy_and_destroy_elements(cola_NEW, &destruir_entrenador);
	list_destroy_and_destroy_elements(cola_READY, &destruir_entrenador);
	list_destroy_and_destroy_elements(cola_BLOCKED, &destruir_entrenador);
	list_destroy_and_destroy_elements(cola_EXIT, &destruir_entrenador);

	list_destroy_and_destroy_elements(cola_INTERRUPCIONES, &destruir_interrupcion);
	list_destroy_and_destroy_elements(pokemons_necesarios, &destruir_pokemon);
	list_destroy_and_destroy_elements(pokemons_mapa, &destruir_pokemon);

	exit(0);
}

//-----------DICCIONARIO DE INTERRUPCIONES-----------//
void inicializar_diccionario_interrupciones()
{
	diccionario_interrupciones = dictionaryInt_create();
	dictionaryInt_put(diccionario_interrupciones, I_PLANIFICAR, &interrupcion_PLANIFICAR);
	dictionaryInt_put(diccionario_interrupciones, I_CAUGHT_POKEMON, &interrupcion_CAUGHT_POKEMON);
	dictionaryInt_put(diccionario_interrupciones, I_APPEARED_POKEMON, &interrupcion_APPEARED_POKEMON);
	dictionaryInt_put(diccionario_interrupciones, I_LOCALIZED_POKEMON, &interrupcion_LOCALIZED_POKEMON);
	dictionaryInt_put(diccionario_interrupciones, I_ASIGNACION_ID, &interrupcion_ASIGNACION_ID);
	dictionaryInt_put(diccionario_interrupciones, I_TERMINAR, &interrupcion_TERMINAR);
}
