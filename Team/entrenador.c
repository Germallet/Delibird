#include "team.h"
#include "pokemon.h"
#include "posicion.h"
#include "entrenador.h"
#include "suscripcion.h"
#include "../Utils/net.h"
#include "../Utils/dictionaryInt.h"
#include <stdlib.h>
#include <unistd.h>
#include <commons/string.h>
#define MULTIPLICADOR_TIEMPO 0.1

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

//-----------ENTRENADOR-----------//
Entrenador* crear_entrenador(char* posicion, char* pokemons_atrapados, char* pokemons_objetivo, int ID)
{
	Entrenador* entrenador = malloc(sizeof(Entrenador));

	entrenador->posicion = crear_posicion(posicion);
	entrenador->pokemons_atrapados = crear_lista_pokemon(pokemons_atrapados);
	entrenador->pokemons_objetivo = crear_lista_pokemon(pokemons_objetivo);
	entrenador->datos_acciones = list_create();
	entrenador->esta_disponible = true;
	entrenador->id_mensaje_espera = NULL;
	entrenador->ID=ID;
	entrenador->estado=NEW;

	pthread_mutex_init(&(entrenador->mutex), NULL);
	pthread_mutex_lock(&(entrenador->mutex));

	pthread_create(&(entrenador->hilo), NULL, (void*) &ciclo, entrenador);

	return entrenador;
}

void cargar_accion(Entrenador* entrenador, Tipo_Accion tipo , void* info)
{
	Datos_Accion* datos_accion = malloc(sizeof(Datos_Accion));
	datos_accion->tipo_accion = tipo;
	datos_accion->info = info;
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
	entrenador->hilo = -1;
}

void destruir_entrenador(void* entrenador_void)
{
	Entrenador* entrenador = (Entrenador*) entrenador_void;
	free(entrenador->id_mensaje_espera);
	list_destroy_and_destroy_elements(entrenador->pokemons_atrapados, &destruir_pokemon);
	list_destroy_and_destroy_elements(entrenador->pokemons_objetivo, &destruir_pokemon);

	if(entrenador->hilo != -1) terminar_hilo(entrenador);

	resetear_acciones(entrenador);
	list_destroy(entrenador->datos_acciones);
	free(entrenador);
}

Posicion obtener_posicion_entrenador(void* entrenador) { return ((Entrenador*) entrenador)->posicion; }

void asignar_id_mensaje_espera(Entrenador* entrenador, uint* nuevo_id)
{
	free(entrenador->id_mensaje_espera);
	entrenador->id_mensaje_espera = nuevo_id;
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

		bool lo_saco = false;

		for(int i=0;!lo_saco && i<cola->elements_count;i++)
		{
			if(entrenador->ID == ((Entrenador*) list_get(cola, i))->ID)
			{
				list_remove(cola, i);
				lo_saco = true;
			}
		}
	}
}

void cambiar_estado_a(Entrenador* entrenador, Estado_Entrenador estado)
{
	sacar_de_cola_actual(entrenador);
	if(estado==EXEC)
		entrenador_EXEC = entrenador;
	else
		list_add(dictionaryInt_get(diccionario_colas, estado), entrenador);
	entrenador->estado = estado;

}

bool puede_seguir_atrapando_pokemons(Entrenador* entrenador) { return entrenador->pokemons_atrapados->elements_count < entrenador->pokemons_objetivo->elements_count; }
bool esta_en_esperando_pokemons(void* entrenador) { return ((Entrenador*) entrenador)->estado!=EXIT && !puede_seguir_atrapando_pokemons((Entrenador*) entrenador);}

Datos_Accion* datos_accion_actual(Entrenador* entrenador) {	return list_get(entrenador->datos_acciones, entrenador->indice_accion_actual); }

bool necesito_pokemon(Entrenador* entrenador, char* especie_pokemon)
{
	bool es_misma_especie(void* especie) { return strcmp(((Pokemon*) especie)->especie, especie_pokemon)==0; }
	return list_any_satisfy(entrenador->pokemons_objetivo, &es_misma_especie);
}

bool tengo_pokemon(Entrenador* entrenador, char* especie_pokemon)
{
	bool es_misma_especie(void* especie) { return strcmp(((Pokemon*) especie)->especie, especie_pokemon)==0; }
	return list_any_satisfy(entrenador->pokemons_atrapados, &es_misma_especie);
}

bool tiene_todas_las_especies_que_necesita(Entrenador* entrenador)
{
	bool entrenador_tiene_pokemon(void* especie) { return tengo_pokemon(entrenador, ((Pokemon*) especie)->especie); }
	return list_all_satisfy(entrenador->pokemons_objetivo, &entrenador_tiene_pokemon);
}

bool tiene_la_cantidad_necesaria_de_cada_especie(Entrenador* entrenador)
{
	bool atrapo_la_cantidad_necesaria(void* especie_pokemon_void)
	{
		bool atrapo_los_necesarios = false;
		char* especie_pokemon = especie_pokemon_void;
		if(cantidad_de_pokemon_en_lista(entrenador->pokemons_atrapados, especie_pokemon) == cantidad_de_pokemon_en_lista(entrenador->pokemons_objetivo, especie_pokemon))
			atrapo_los_necesarios = true;

		return atrapo_los_necesarios;
	}

	return list_all_satisfy(entrenador->pokemons_atrapados, &atrapo_la_cantidad_necesaria);
}

bool tiene_todos_sus_pokemons(Entrenador* entrenador) { return tiene_todas_las_especies_que_necesita(entrenador) && tiene_la_cantidad_necesaria_de_cada_especie(entrenador); }

static void siguiente_accion(Entrenador* entrenador) { entrenador->indice_accion_actual++; }

//-----------LISTA ENTRENADORES-----------//
Entrenador* tomar_entrenador(t_list* lista_entrenadores) { return list_remove(lista_entrenadores, 0); }

Entrenador* entrenador_mas_cercano(t_list* lista_entrenadores, Posicion posicion_llegada)
{
	if(list_is_empty(lista_entrenadores))
		return NULL;
	if(lista_entrenadores->elements_count == 1)
		return lista_entrenadores->head->data;

	Entrenador* entrenador = lista_entrenadores->head->data;

	Entrenador* entrenador_mas_cercano_entre_dos(Entrenador* entrenador1, Entrenador* entrenador2)
	{ return (distancia_entre(entrenador1->posicion, posicion_llegada) > distancia_entre(entrenador2->posicion, posicion_llegada))? entrenador2 : entrenador1; }

	for(int i=1; i < lista_entrenadores->elements_count;i++) { entrenador = entrenador_mas_cercano_entre_dos(entrenador, list_get(lista_entrenadores,i)); }

	return entrenador;
}

t_list* obtener_entrenadores_disponibles()
{
	t_list* entrenadores_disponibles = list_duplicate(cola_NEW);
	t_list* entrenadores_bloqueados_disponibles = list_filter(cola_BLOCKED, &esta_disponible);
	list_add_all(entrenadores_disponibles, entrenadores_bloqueados_disponibles);
	list_destroy(entrenadores_bloqueados_disponibles);
	return entrenadores_disponibles;
}

//-----------OTRAS FUNCIONES-----------//
void obtener_entrenadores()
{
	inicializar_diccionario_acciones();
	inicializar_diccionario_colas();

	char* entrenadores = config_get_string_value(config,"POSICIONES_ENTRENADORES");
	char** posiciones = config_get_array_value(config,"POSICIONES_ENTRENADORES");
	char** pokemons_atrapados = config_get_array_value(config,"POKEMON_ENTRENADORES");
	char** pokemons_objetivo = config_get_array_value(config,"OBJETIVOS_ENTRENADORES");

	int cantidad_entrenadores = 1;
		for(int i=0;entrenadores[i]!='\0';i++)
			cantidad_entrenadores += entrenadores[i] == ',';

	for(int i = 0; i<cantidad_entrenadores; i++)
	{
		list_add(cola_NEW, crear_entrenador(posiciones[i], pokemons_atrapados[i], pokemons_objetivo[i],i+1));
		free(posiciones[i]);
		free(pokemons_atrapados[i]);
		free(pokemons_objetivo[i]);
	}

	free(posiciones);
	free(pokemons_atrapados);
	free(pokemons_objetivo);
}

void capturo_pokemon(Entrenador* entrenador)
{
	char* especie_pokemon_atrapada = datos_accion_actual(entrenador)->info;
	agregar_pokemon(entrenador->pokemons_atrapados, especie_pokemon_atrapada);
	log_info(logger, "El entrenador %d atrapo un %s en la posicion (%d,%d)", entrenador->ID, especie_pokemon_atrapada, entrenador->posicion.posX, entrenador->posicion.posY);
	resetear_acciones(entrenador);

	if(puede_seguir_atrapando_pokemons(entrenador))
	{
		cambiar_estado_a(entrenador, BLOCKED);
		habilitar_entrenador(entrenador);
	}
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
			cambiar_estado_a(entrenador, BLOCKED);
		}
	}
}

void fallo_captura_pokemon(Entrenador* entrenador)
{
	agregar_pokemon(pokemons_necesarios, (char*) (datos_accion_actual(entrenador)->info));
	habilitar_entrenador(entrenador);
}

void ejecutar_entrenador_actual()
{
	if(entrenador_EXEC!=NULL)
		pthread_mutex_unlock(&(entrenador_EXEC->mutex));
	else
	{
		log_info(logger, "IDLE");
		sleep((unsigned) config_get_int_value(config,"RETARDO_CICLO_CPU"));
		pthread_mutex_unlock(&(mutex_team));
	}
}

t_list* pokemons_que_tiene_y_no_necesita(Entrenador* entrenador)
{
	t_list* pokemons_de_mas = list_duplicate(entrenador->pokemons_atrapados);

	for(int i=0;i<entrenador->pokemons_objetivo->elements_count;i++)
		tomar_pokemon(pokemons_de_mas, ((Pokemon*) list_get(entrenador->pokemons_objetivo, i))->especie);

	return pokemons_de_mas;
}

bool necesita_algun_pokemon_de(Entrenador* entrenador, t_list* pokemons)
{
	bool necesita_pokemon(void* pokemon) { return tiene_pokemon(entrenador->pokemons_objetivo, ((Pokemon*) pokemon)->especie); }
	return list_any_satisfy(pokemons, &necesita_pokemon);
}
bool necesita_pokemon(Entrenador* entrenador, Pokemon* pokemon) { return tiene_pokemon(entrenador->pokemons_objetivo, pokemon->especie); }

bool tienen_pokemons_para_intercambiar(Entrenador* entrenador_1, Entrenador* entrenador_2)
{
	t_list* pokemons_que_le_sobran_a_entrenador_1 = pokemons_que_tiene_y_no_necesita(entrenador_1);
	t_list* pokemons_que_le_sobran_a_entrenador_2 = pokemons_que_tiene_y_no_necesita(entrenador_2);

	char* una_especie_que_necesita_de(Entrenador* entrenador, t_list* pokemons)
	{
		char* especie = NULL;

		for(int i=0;i<pokemons->elements_count;i++)
			if(necesita_pokemon(entrenador, (Pokemon*) list_get(pokemons, i))) especie = ((Pokemon*) list_get(pokemons, i))->especie;

		return especie;
	}

	if(necesita_algun_pokemon_de(entrenador_1, pokemons_que_le_sobran_a_entrenador_2)) entrenador_1->info = una_especie_que_necesita_de(entrenador_1, pokemons_que_le_sobran_a_entrenador_2);
	if(necesita_algun_pokemon_de(entrenador_2, pokemons_que_le_sobran_a_entrenador_1)) entrenador_2->info = una_especie_que_necesita_de(entrenador_2, pokemons_que_le_sobran_a_entrenador_1);

	bool se_encontro_algun_pokemon_intercambiable = (entrenador_1->info != NULL) || (entrenador_2->info != NULL);

	list_destroy(pokemons_que_le_sobran_a_entrenador_1);
	list_destroy(pokemons_que_le_sobran_a_entrenador_2);

	return se_encontro_algun_pokemon_intercambiable;
}

t_list* obtener_entrenadores_en_espera_de_pokemons() { return list_filter(cola_BLOCKED, &esta_en_esperando_pokemons); }

bool hay_entrenadores_que_pueden_intercambiar()
{
	t_list* entrenadores_en_deadlock = obtener_entrenadores_en_espera_de_pokemons();
	Entrenador* entrenador_pivot = NULL;

	bool hay = false;

	if(!list_is_empty(entrenadores_en_deadlock)) entrenador_pivot = list_get(entrenadores_en_deadlock, 0);

	for(int i=1;!hay && i<entrenadores_en_deadlock->elements_count;i++)
		if(tienen_pokemons_para_intercambiar(entrenador_pivot, list_get(entrenadores_en_deadlock, i))) hay = true;

	list_destroy(entrenadores_en_deadlock);

	return hay;
}

void obtener_entrenadores_que_pueden_intercambiar(Entrenador* entrenador_1, Entrenador* entrenador_2)
{
	t_list* entrenadores_en_deadlock = obtener_entrenadores_en_espera_de_pokemons();

	Entrenador* entrenador_pivot = NULL;
	bool encontro = false;
	if(!list_is_empty(entrenadores_en_deadlock)) entrenador_pivot = list_get(entrenadores_en_deadlock, 0);

	for(int i=1;!encontro && i<entrenadores_en_deadlock->elements_count;i++)
	{
		if(tienen_pokemons_para_intercambiar(entrenador_pivot, list_get(entrenadores_en_deadlock, i)))
		{
			entrenador_1 = entrenador_pivot;
			entrenador_2 = list_get(entrenadores_en_deadlock, i);
		}
	}
	if(!encontro)
	{
		entrenador_1 = NULL;
		entrenador_2 = NULL;
	}

	list_destroy_and_destroy_elements(entrenadores_en_deadlock, &destruir_entrenador);
}

//-----------ACCIONES-----------//
static void ciclo(Entrenador* entrenador)
{
	while(1)
	{
		pthread_mutex_lock(&(entrenador->mutex));
		((Accion) dictionaryInt_get(diccionario_acciones, datos_accion_actual(entrenador)->tipo_accion)) (entrenador);
		sleep((unsigned) config_get_int_value(config,"RETARDO_CICLO_CPU")*MULTIPLICADOR_TIEMPO);
		pthread_mutex_unlock(&(mutex_team));
	}
}

static void mover_una_casilla_hacia(Entrenador* entrenador)
{
	uint32_t x_actual = entrenador->posicion.posX;
	uint32_t y_actual = entrenador->posicion.posY;

	uint32_t x_objetivo = ((Posicion*) datos_accion_actual(entrenador)->info)->posX;
	uint32_t y_objetivo = ((Posicion*) datos_accion_actual(entrenador)->info)->posY;

	if(x_actual != x_objetivo) // Si no esta en x se acerca una pos a x
		entrenador->posicion.posX = x_actual + (x_objetivo>x_actual ? 1 : -1 );

	else if(y_actual != y_objetivo) // Si no esta en y se acerca una pos a y
		entrenador->posicion.posY = y_actual + (y_objetivo>y_actual ? 1 : -1 );

	log_info(logger, "El entrenador %d se movio a la posicion (%d,%d), hacia (%d,%d)", entrenador->ID, entrenador->posicion.posX, entrenador->posicion.posY, x_objetivo, y_objetivo);

	if(entrenador->posicion.posX==x_objetivo && entrenador->posicion.posY==y_objetivo)
		siguiente_accion(entrenador);
}

static void capturar_pokemon(void* entrenador_void)
{
	Entrenador* entrenador = entrenador_void;
	char* especie_pokemon_a_atrapar = datos_accion_actual(entrenador)->info;

	log_info(logger, "El entrenador %d intenta atrapar un %s en la posicion (%d,%d)", entrenador->ID, especie_pokemon_a_atrapar, entrenador->posicion.posX, entrenador->posicion.posY);

	Cliente* cliente = crear_cliente_de_broker(Eventos_Crear0());
	if(cliente != NULL)
	{
		DATOS_CATCH_POKEMON* datos = malloc(sizeof(DATOS_GET_POKEMON));
		datos->pokemon = especie_pokemon_a_atrapar;
		datos->posicion = entrenador->posicion;

		EnviarMensaje(cliente, GET_POKEMON, datos, (Serializador) &SerializarM_CATCH_POKEMON);
	}
	else
		capturo_pokemon(entrenador);
}
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
	diccionario_colas = dictionaryInt_create();
	dictionaryInt_put(diccionario_colas, NEW, cola_NEW);
	dictionaryInt_put(diccionario_colas, READY, cola_READY);
	dictionaryInt_put(diccionario_colas, BLOCKED, cola_BLOCKED);
	dictionaryInt_put(diccionario_colas, EXIT, cola_EXIT);
}

