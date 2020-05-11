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
t_list* cola_BLOCKED;
t_list* cola_EXIT;
t_dictionaryInt* diccionario_acciones;
t_dictionaryInt* diccionario_colas;

static void ciclo(Entrenador* entrenador);
static void inicializar_diccionario_acciones();
static void inicializar_diccionario_colas();
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

void cargar_accion(Entrenador* entrenador, Tipo_Accion tipo ,void* info)
{
	Datos_Accion* datos_accion = malloc(sizeof(Datos_Accion));
	list_add(entrenador->datos_acciones, datos_accion);
}

void resetear_acciones(Entrenador* entrenador)
{
	entrenador->indice_accion_actual = 0;
	void destruir_datos_accion(void* datos_accion)
	{
		free(((Datos_Accion*) datos_accion)->info);
		free(datos_accion);
	}
	list_clean_and_destroy_elements(entrenador->datos_acciones, &destruir_datos_accion);
}

void terminar_hilo(Entrenador* entrenador)
{
	resetear_acciones(entrenador);
	cargar_accion(entrenador, TERMINAR, NULL);
	pthread_mutex_unlock(&(entrenador->mutex));
	pthread_join(entrenador->hilo, NULL);
	entrenador->hilo = 0;
}

void destruir_entrenador(void* entrenador_void)
{
	Entrenador* entrenador = (Entrenador*) entrenador_void;
	free(entrenador->posicion);
	list_destroy_and_destroy_elements(entrenador->pokemons_atrapados, &destruir_pokemon);
	list_destroy_and_destroy_elements(entrenador->pokemons_objetivo, &destruir_pokemon);

	if(entrenador->hilo != 0) terminar_hilo(entrenador);

	resetear_acciones(entrenador);
	list_destroy(entrenador->datos_acciones);
	free(entrenador);
}

void habilitar_entrenador(Entrenador* entrenador)
{
	entrenador->esta_disponible=true;
	resetear_acciones(entrenador);
}
void deshabilitar_entrenador(Entrenador* entrenador) { entrenador->esta_disponible=false; }

bool esta_disponible(void* entrenador) { return ((Entrenador*) entrenador)->esta_disponible;  }

t_list* cola_actual(Entrenador* entrenador) { return dictionaryInt_get(diccionario_colas, entrenador->estado); }

bool esta_en_ejecucion(Entrenador* entrenador) { return entrenador->estado==EXEC; }

void sacar_de_cola_actual(Entrenador* entrenador)
{
	if(esta_en_ejecucion(entrenador))
		entrenador_EXEC = NULL;
	else
	{
		t_list* cola = cola_actual(entrenador);

		for(int i=0;i<cola->elements_count;i++)
			if(entrenador->ID == ((Entrenador*) list_get(cola, i))->ID) list_remove_and_destroy_element(cola, i, &destruir_entrenador);
	}
}

void cambiar_estado_a(Entrenador* entrenador, Estado_Entrenador estado)
{
	sacar_de_cola_actual(entrenador);
	if(estado==EXEC)
		entrenador_EXEC = entrenador;
	else
		list_add(cola_BLOCKED, entrenador);
	entrenador->estado = estado;
}

bool puede_seguir_atrapando_pokemons(Entrenador* entrenador) { return entrenador->pokemons_atrapados->elements_count < entrenador->pokemons_objetivo->elements_count; }
bool esta_en_deadlock(Entrenador* entrenador) { return entrenador->estado!=EXIT && !puede_seguir_atrapando_pokemons(entrenador);}

Datos_Accion* datos_accion_actual(Entrenador* entrenador)
{
	return list_get(entrenador->datos_acciones, entrenador->indice_accion_actual);
}

bool tiene_todos_sus_pokemons(Entrenador* entrenador)
{
	t_list* pokemons_pivot = list_duplicate(entrenador->pokemons_atrapados);

	for(int i=0;i<entrenador->pokemons_objetivo->elements_count;i++)
		destruir_pokemon(tomar_pokemon(pokemons_pivot, ((Pokemon*) list_get(entrenador->pokemons_objetivo, i))->especie));

	bool tiene_todos_los_pokemons = list_is_empty(pokemons_pivot);

	list_destroy_and_destroy_elements(pokemons_pivot, &destruir_pokemon);

	return tiene_todos_los_pokemons;
}

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
	list_add_all(entrenadores_disponibles, list_filter(cola_BLOCKED, &esta_disponible));
	return entrenadores_disponibles;
}

//-----------OTRAS FUNCIONES-----------//
void obtener_entrenadores()
{
	inicializar_diccionario_acciones();
	inicializar_diccionario_colas();

	char** posiciones = config_get_array_value(config,"POSICIONES_ENTRENADORES");
	char** pokemons_atrapados = config_get_array_value(config,"POKEMON_ENTRENADORES");
	char** pokemons_objetivo = config_get_array_value(config,"OBJETIVOS_ENTRENADORES");

	int cantidad_entrenadores = 3; //TODO calcular length de vector cualquiera

	for(int i = 0; i<cantidad_entrenadores; i++)
		list_add(cola_NEW, crear_entrenador(posiciones[i], pokemons_atrapados[i], pokemons_objetivo[i],i+1));
}

void capturo_pokemon(Entrenador* entrenador)
{
	agregar_pokemon(entrenador->pokemons_atrapados, (char*) (datos_accion_actual(entrenador)->info));
	if(puede_seguir_atrapando_pokemons(entrenador))
		habilitar_entrenador(entrenador);
	else
	{
		if(tiene_todos_sus_pokemons(entrenador))
		{
			terminar_hilo(entrenador);
			cambiar_estado_a(entrenador, EXIT);
		}
		else
		{
			deshabilitar_entrenador(entrenador);
			resetear_acciones(entrenador);
			cambiar_estado_a(entrenador, BLOCKED);
		}

	}

}
void fallo_captura_pokemon(Entrenador* entrenador)
{
	agregar_pokemon(pokemons_necesarios, (char*) (datos_accion_actual(entrenador)->info));
	habilitar_entrenador(entrenador);
}
void ejecutar_entrenador_actual() { pthread_mutex_unlock(&(entrenador_EXEC->mutex)); }

t_list* pokemons_que_tiene_y_no_necesita(Entrenador* entrenador)
{
	t_list* pokemons_de_mas = list_duplicate(entrenador->pokemons_atrapados);

	for(int i=0;i<entrenador->pokemons_objetivo->elements_count;i++)
		destruir_pokemon(tomar_pokemon(pokemons_de_mas, ((Pokemon*) list_get(entrenador->pokemons_objetivo, i))->especie));

	return pokemons_de_mas;
}

bool necesita_algun_pokemon_de(Entrenador* entrenador, t_list* pokemons)
{
	bool necesita_pokemon(void* pokemon) { return tiene_pokemon(entrenador->pokemons_objetivo, ((Pokemon*) pokemon)->especie); }
	return list_any_satisfy(pokemons, &necesita_pokemon);
}
bool necesita_pokemon(Entrenador* entrenador, Pokemon* pokemon) { return tiene_pokemon(entrenador->pokemons_objetivo, pokemon->especie); }

bool pueden_intercambiar(Entrenador* entrenador_1, Entrenador* entrenador_2, char* especie_1, char* especie_2)
{
	especie_1 = NULL;
	especie_2 = NULL;

	t_list* pokemons_que_le_sobran_a_entrenador_1 = pokemons_que_tiene_y_no_necesita(entrenador_1);
	t_list* pokemons_que_le_sobran_a_entrenador_2 = pokemons_que_tiene_y_no_necesita(entrenador_2);

	char* una_especie_que_necesita_de(Entrenador* entrenador, t_list* pokemons)
	{
		char* especie = NULL;

		for(int i=0;i<pokemons->elements_count;i++)
			if(necesita_pokemon(entrenador, (Pokemon*) list_get(pokemons, i))) especie = ((Pokemon*) list_get(pokemons, i))->especie;

		return especie;
	}

	if(necesita_algun_pokemon_de(entrenador_1, pokemons_que_le_sobran_a_entrenador_2)) especie_1 = una_especie_que_necesita_de(entrenador_1, pokemons_que_le_sobran_a_entrenador_2);
	if(necesita_algun_pokemon_de(entrenador_2, pokemons_que_le_sobran_a_entrenador_1)) especie_2 = una_especie_que_necesita_de(entrenador_2, pokemons_que_le_sobran_a_entrenador_1);

	bool se_encontro_algun_pokemon_intercambiable = (especie_1 != NULL) || (especie_2 != NULL);

	return esta_en_deadlock(entrenador_1) && esta_en_deadlock(entrenador_2) && se_encontro_algun_pokemon_intercambiable;
}

void obtener_entrenadores_que_pueden_intercambiar(Entrenador* entrenador_1, Entrenador* entrenador_2)
{

}

//-----------ACCIONES-----------//
static void ciclo(Entrenador* entrenador)
{
	while(1)
	{
		pthread_mutex_lock(&(entrenador->mutex));
		((Accion) dictionaryInt_get(diccionario_acciones, datos_accion_actual(entrenador)->tipo_accion)) (entrenador);
		sleep((unsigned) config_get_array_value(config,"RETARDO_CICLO_CPU"));
		pthread_mutex_unlock(&(mutex_team));
	}
}

static void mover_una_casilla_hacia(Entrenador* entrenador)
{
	uint32_t x_actual = entrenador->posicion->posX;
	uint32_t y_actual = entrenador->posicion->posY;

	uint32_t x_objetivo = ((Posicion*) datos_accion_actual(entrenador)->info)->posX;
	uint32_t y_objetivo = ((Posicion*) datos_accion_actual(entrenador)->info)->posY;

	if(x_actual != x_objetivo) // Si no esta en x se acerca una pos a x
		entrenador->posicion->posX = x_actual + (x_objetivo-x_actual/abs(x_objetivo-x_actual));

	else if(y_actual != y_objetivo) // Si no esta en y se acerca una pos a y
		entrenador->posicion->posY = y_actual + (y_objetivo-y_actual/abs(y_objetivo-y_actual));

	if(entrenador->posicion->posX==x_objetivo && entrenador->posicion->posY==y_objetivo)
		siguiente_accion(entrenador);
}

static void capturar_pokemon(Entrenador* entrenador) {}
static void intercambiar_pokemon(Entrenador* entrenador) {}
static void terminar(Entrenador* entrenador) { pthread_exit(NULL); }

//-----------DICCIONARIO DE ACCIONES-----------//
static void inicializar_diccionario_acciones()
{
	diccionario_acciones = dictionaryInt_create();
	dictionaryInt_put(diccionario_acciones, MOVER, &mover_una_casilla_hacia);
	dictionaryInt_put(diccionario_acciones, CAPTURAR_POKEMON, &capturar_pokemon);
	dictionaryInt_put(diccionario_acciones, INTERCAMBIAR_POKEMON, &intercambiar_pokemon);
	dictionaryInt_put(diccionario_acciones, TERMINAR, &terminar);
}

//-----------DICCIONARIO DE COLAS-----------//
static void inicializar_diccionario_colas()
{
	diccionario_acciones = dictionaryInt_create();
	dictionaryInt_put(diccionario_colas, NEW, cola_NEW);
	dictionaryInt_put(diccionario_colas, READY, cola_READY);
	dictionaryInt_put(diccionario_colas, BLOCKED, cola_BLOCKED);
	dictionaryInt_put(diccionario_colas, EXIT, cola_EXIT);
}

