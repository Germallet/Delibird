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
void agregar_interrupcion(Tipo_Interrupcion tipo, void* info)
{
	Datos_Interrupcion* interrupcion = malloc(sizeof(Datos_Interrupcion));
	interrupcion->tipo = tipo;
	interrupcion->info = info;
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
	cola_INTERRUPCIONES = list_create();
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
void interrupcion_CAUGHT_POKEMON(void* dato)
{
	datos_interrupcion_CAUGHT_POKEMON* datos = dato;
	Entrenador* entrenador = datos->entrenador;

	if(datos->recibidos->capturado)
		capturo_pokemon(entrenador);
	else
		fallo_captura_pokemon(entrenador);
	free(datos->recibidos);
	free(datos);
}
void interrupcion_APPEARED_POKEMON(void* dato)
{
	DATOS_APPEARED_POKEMON* datos = dato;
	if(necesito_especie_pokemon(datos->pokemon))
		agregar_pokemon_a_mapa(datos->pokemon, datos->posicion);
	free(datos->pokemon);
}
void interrupcion_LOCALIZED_POKEMON(void* dato)
{
	DATOS_LOCALIZED_POKEMON* datos = dato;
	for(int i=0;i<datos->cantidad;i++)
		agregar_pokemon_a_mapa(datos->pokemon, datos->posiciones[i]);
	se_localizo(datos->pokemon);
	free(datos);
}

//-----------DICCIONARIO DE INTERRUPCIONES-----------//
void inicializar_diccionario_interrupciones()
{
	diccionario_interrupciones = dictionaryInt_create();
	dictionaryInt_put(diccionario_interrupciones, I_CAUGHT_POKEMON, &interrupcion_CAUGHT_POKEMON);
	dictionaryInt_put(diccionario_interrupciones, I_APPEARED_POKEMON, &interrupcion_APPEARED_POKEMON);
	dictionaryInt_put(diccionario_interrupciones, I_LOCALIZED_POKEMON, &interrupcion_LOCALIZED_POKEMON);
}
