#include "team.h"
#include "pokemon.h"
#include "entrenador.h"
#include "../Utils/dictionaryInt.h"
#include <stdlib.h>
#include <unistd.h>
#include <commons/string.h>

Entrenador* entrenador_EXEC;
t_list* cola_NEW;
t_list* cola_READY;
t_list* cola_BLOCK;
t_list* cola_EXIT;
t_dictionaryInt* diccionario_acciones;

static void ciclo(Entrenador* entrenador);
static void inicializar_diccionario_acciones();

//-----------POSICION-----------//
Posicion* crear_posicion(char* string_posicion)
{
	Posicion* posicion = malloc(sizeof(Posicion));
	posicion->posX = string_posicion[0]-'0';
	posicion->posY = string_posicion[2]-'0';

	return posicion;
}

//-----------ENTRENADOR-----------//
Entrenador* crear_entrenador(char* posicion, char* pokemons_atrapados, char* pokemons_objetivo, int ID)
{
	Entrenador* entrenador = malloc(sizeof(Entrenador));

	entrenador->posicion = crear_posicion(posicion);
	entrenador->pokemons_atrapados = crear_lista_pokemon(pokemons_atrapados);
	entrenador->pokemons_objetivo = crear_lista_pokemon(pokemons_objetivo);
	entrenador->esta_disponible = true;
	entrenador->ID=ID;
	entrenador->estado=NEW;

	pthread_mutex_init(&(entrenador->mutex), NULL);
	pthread_mutex_lock(&(entrenador->mutex));

	pthread_create(&(entrenador->hilo), NULL, (void*) &ciclo, entrenador);

	return entrenador;
}

void destruir_entrenador(void* entrenador_void)
{
	Entrenador* entrenador = (Entrenador*) entrenador_void;
	free(entrenador->posicion);
	list_destroy_and_destroy_elements(entrenador->pokemons_atrapados, &destruir_pokemon);
	list_destroy_and_destroy_elements(entrenador->pokemons_objetivo, &destruir_pokemon);

	free(entrenador->acciones);
	entrenador->acciones = malloc(sizeof(Tipo_Accion));
	entrenador->acciones[0] = TERMINAR;
	entrenador->indice_accion_actual = 0;
	pthread_mutex_unlock(&(entrenador->mutex));

	pthread_join(entrenador->hilo, NULL);

	free(entrenador->acciones);
	free(entrenador);
}

void habilitar_entrenador(Entrenador* entrenador)
{
	entrenador->esta_disponible=true;
	entrenador->info = NULL;
}
void deshabilitar_entrenador(Entrenador* entrenador) { entrenador->esta_disponible=false; }
bool esta_disponible(void* entrenador) { return ((Entrenador*) entrenador)->esta_disponible;  }
static void siguiente_accion(Entrenador* entrenador) { entrenador->indice_accion_actual++; }

//-----------LISTA ENTRENADORES-----------//
Entrenador* tomar_entrenador(t_list* lista_entrenadores) { return list_remove(lista_entrenadores, 0); }

Entrenador* entrenador_mas_cercano(t_list* lista_entrenadores, Posicion* posicion_llegada)
{
	if(list_is_empty(lista_entrenadores))
		return NULL;
	if(lista_entrenadores->elements_count == 1)
		return lista_entrenadores->head->data;

	Entrenador* entrenador = lista_entrenadores->head->data;

	int distancia_entre(Posicion* posicion1, Posicion* posicion2) { return abs(posicion2->posX-posicion1->posX)+abs(posicion2->posY-posicion1->posY); }

	Entrenador* entrenador_mas_cercano_entre_dos(Entrenador* entrenador1, Entrenador* entrenador2)
	{ return (distancia_entre(entrenador1->posicion, posicion_llegada) > distancia_entre(entrenador2->posicion, posicion_llegada))? entrenador2 : entrenador1; }

	for(int i=1; i < lista_entrenadores->elements_count;i++) { entrenador = entrenador_mas_cercano_entre_dos(entrenador, list_get(lista_entrenadores,i)); }

	return entrenador;
}

t_list* entrenadores_disponibles()
{
	t_list* entrenadores_disponibles = cola_NEW;
	list_add_all(entrenadores_disponibles, list_filter(cola_BLOCK, &esta_disponible));
	return entrenadores_disponibles;
}

//-----------OTRAS FUNCIONES-----------//
void inicializar_entrenadores()
{
	inicializar_diccionario_acciones();

	char** posiciones = config_get_array_value(config,"POSICIONES_ENTRENADORES");
	char** pokemons_atrapados = config_get_array_value(config,"POKEMON_ENTRENADORES");
	char** pokemons_objetivo = config_get_array_value(config,"OBJETIVOS_ENTRENADORES");

	int cantidad_entrenadores = 3; //TODO calcular length de vector cualquiera

	for(int i = 0; i<cantidad_entrenadores; i++)
		list_add(cola_NEW, crear_entrenador(posiciones[i], pokemons_atrapados[i], pokemons_objetivo[i],i+1));
}

void capturo_pokemon(Entrenador* entrenador)
{
	agregar_pokemon(entrenador->pokemons_atrapados, ((Pokemon_Mapa*) (entrenador->info))->especie);
	habilitar_entrenador(entrenador);
}
void fallo_captura_pokemon(Entrenador* entrenador)
{
	agregar_pokemon(pokemons_necesarios, ((Pokemon_Mapa*) (entrenador->info))->especie);
	habilitar_entrenador(entrenador);
}
void ejecutar_entrenador_actual() { pthread_mutex_unlock(&(entrenador_EXEC->mutex)); }
//-----------ACCIONES-----------//
static void ciclo(Entrenador* entrenador)
{
	while(1)
	{
		pthread_mutex_lock(&(entrenador->mutex));
		((Accion) dictionaryInt_get(diccionario_acciones, entrenador->acciones[entrenador->indice_accion_actual])) (entrenador);
		sleep((unsigned) config_get_array_value(config,"RETARDO_CICLO_CPU"));
		pthread_mutex_unlock(&(mutex_team));
	}
}

static void mover_una_casilla_hacia(Entrenador* entrenador)
{
	uint32_t x_actual = entrenador->posicion->posX;
	uint32_t y_actual = entrenador->posicion->posY;

	uint32_t x_objetivo = ((Posicion*) entrenador->info)->posX;
	uint32_t y_objetivo = ((Posicion*) entrenador->info)->posY;

	if(x_actual != x_objetivo) // Si no esta en x se acerca una pos a x
		entrenador->posicion->posX = x_actual + (x_objetivo-x_actual/abs(x_objetivo-x_actual));

	else if(y_actual != y_objetivo) // Si no esta en y se acerca una pos a y
		entrenador->posicion->posY = y_actual + (y_objetivo-y_actual/abs(y_objetivo-y_actual));

	if(entrenador->posicion->posX==x_objetivo && entrenador->posicion->posY==y_objetivo)
		siguiente_accion(entrenador);
}

static void capturar_pokemon(Entrenador* entrenador) {}
static void intercambiar_pokemon(Entrenador* entrenador) {}
static void terminar_hilo(Entrenador* entrenador) { pthread_exit(NULL); }

//-----------DICCIONARIO DE ACCIONES-----------//
static void inicializar_diccionario_acciones()
{
	diccionario_acciones = dictionaryInt_create();
	dictionaryInt_put(diccionario_acciones, MOVER, &mover_una_casilla_hacia);
	dictionaryInt_put(diccionario_acciones, CAPTURAR_POKEMON, &capturar_pokemon);
	dictionaryInt_put(diccionario_acciones, INTERCAMBIAR_POKEMON, &intercambiar_pokemon);
	dictionaryInt_put(diccionario_acciones, TERMINAR, &terminar_hilo);
}

