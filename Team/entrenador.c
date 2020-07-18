#include "team.h"
#include "pokemon.h"
#include "posicion.h"
#include "entrenador.h"
#include "suscripcion.h"
#include "planificacion.h"
#include "../Utils/net.h"
#include "../Utils/dictionaryInt.h"
#include <stdlib.h>
#include <unistd.h>
#include <commons/string.h>
#define MULTIPLICADOR_TIEMPO 1 //TODO

Entrenador* entrenador_EXEC;
t_list* cola_NEW;
t_list* cola_READY;
t_list* cola_BLOCKED;
t_list* cola_EXIT;
t_dictionaryInt* diccionario_acciones;
t_dictionaryInt* diccionario_colas;
t_dictionaryInt* diccionario_razones;
t_dictionaryInt* diccionario_estado_string;

static void ciclo(Entrenador* entrenador);
static void inicializar_diccionario_acciones();
static void inicializar_diccionario_colas();
static void inicializar_diccionario_razones();
static void inicializar_diccionario_estado_string();

//-----------ENTRENADOR-----------//
Entrenador* crear_entrenador(char* posicion, char* pokemons_atrapados, char* pokemons_objetivo, int ID)
{
	Entrenador* entrenador = malloc(sizeof(Entrenador));

	entrenador->posicion = crear_posicion(posicion);
	entrenador->pokemons_atrapados = crear_lista_pokemon(pokemons_atrapados);
	entrenador->pokemons_objetivo = crear_lista_pokemon(pokemons_objetivo);
	entrenador->datos_acciones = list_create();
	entrenador->esta_disponible = true;
	entrenador->id_mensaje_espera = -1;
	entrenador->ID=ID;
	entrenador->estado=NEW;
	entrenador->indice_accion_actual = 0;
	entrenador->ciclosTotales = 0;

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
		if(((Datos_Accion*) datos_accion)->info != NULL)
			free(((Datos_Accion*) datos_accion)->info);
		free(datos_accion);
		datos_accion = NULL;
	}
	list_clean_and_destroy_elements(entrenador->datos_acciones, &destruir_datos_accion);
}

void terminar_hilo(Entrenador* entrenador)
{
	cargar_accion(entrenador, TERMINAR, NULL);
	pthread_mutex_unlock(&(entrenador->mutex));
	pthread_join(entrenador->hilo, NULL);
}

void destruir_entrenador(Entrenador* entrenador)
{
//	Entrenador* entrenador = entrenador_void;

	log_info(logger,"El entrenador %d termino en %d ciclos.",entrenador->ID,entrenador->ciclosTotales);

	list_destroy_and_destroy_elements(entrenador->pokemons_atrapados, &destruir_pokemon);
	list_destroy_and_destroy_elements(entrenador->pokemons_objetivo, &destruir_pokemon);
	resetear_acciones(entrenador);
	list_destroy(entrenador->datos_acciones);
	free(entrenador);
}

Posicion* obtener_posicion_entrenador(void* entrenador) { return &(((Entrenador*) entrenador)->posicion); }

int ciclos_restantes(Entrenador* entrenador)
{
	if(entrenador->datos_acciones == NULL)
		return 0;

	int distancia = distancia_entre(*((Posicion*) (((Datos_Accion*) list_get(entrenador->datos_acciones, 0))->info)), entrenador->posicion);
	return distancia + entrenador->datos_acciones->elements_count - (entrenador->indice_accion_actual+1);
}

void habilitar_entrenador(Entrenador* entrenador) { entrenador->esta_disponible=true; }
void deshabilitar_entrenador(Entrenador* entrenador) { entrenador->esta_disponible=false; }

bool esta_disponible(void* entrenador) { return ((Entrenador*) entrenador)->esta_disponible; }

bool puede_planificarse_para_atrapar(void* entrenador) { return esta_disponible(entrenador) && puede_seguir_atrapando_pokemons((Entrenador*) entrenador); }

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

static void insertar_FIFO(Entrenador* entrenador) { list_add(cola_READY, entrenador); }
static void insertar_SJF(Entrenador* entrenador)
{
	int index = 0;
	Entrenador* pivot = list_get(cola_READY, index);
	while(pivot != NULL && ciclos_restantes(pivot)<=ciclos_restantes(entrenador))
	{
		index++;
		pivot = list_get(cola_READY, index);
	}
	list_add_in_index(cola_READY, index, entrenador);
}

static void meter_en_cola_READY(Entrenador* entrenador)
{
	if(es_planificacion_tipo("FIFO") || es_planificacion_tipo("RR"))
		insertar_FIFO(entrenador);
	if(es_planificacion_tipo("SJF-SD"))
		insertar_SJF(entrenador);
	if(es_planificacion_tipo("SJF-CD"))
	{
		insertar_SJF(entrenador);
		if(hay_entrenador_en_ejecucion())
		{
			insertar_SJF(entrenador_EXEC);
			entrenador_EXEC->estado = READY;/////////////////////////////////////////////////////////////////
			entrenador_EXEC = NULL;
		}
	}
}

void poner_entrenador_en_EXEC() {
	cambiar_estado_a(tomar_entrenador(cola_READY), EXEC, A_EXEC);
	cantidad_cambios_de_contexto++;
}
void cambiar_estado_a(Entrenador* entrenador, Estado_Entrenador estado, Razon razon)
{
	t_list* cola_anterior = dictionaryInt_get(diccionario_colas, entrenador->estado);
	t_list* cola_nueva = dictionaryInt_get(diccionario_colas, estado);
	sacar_de_cola_actual(entrenador);

	if(estado==EXEC)
		entrenador_EXEC = entrenador;
	else if(estado==READY)
		meter_en_cola_READY(entrenador);
	else
		list_add(cola_nueva, entrenador);
	entrenador->estado = estado;

	if(cola_anterior != cola_nueva) log_info(logger, "El entrenador %d paso a la cola %s por %s.", entrenador->ID, dictionaryInt_get(diccionario_estado_string, estado), dictionaryInt_get(diccionario_razones, razon));
}

bool puede_seguir_atrapando_pokemons(Entrenador* entrenador) { return cantidad_de_pokemons_en_lista(entrenador->pokemons_atrapados) < cantidad_de_pokemons_en_lista(entrenador->pokemons_objetivo); }
bool tiene_de_mas(Entrenador* entrenador, Pokemon* pokemon)
{
	char* especie = pokemon->especie;
	return cantidad_de_pokemon_en_lista(entrenador->pokemons_atrapados, especie) > cantidad_de_pokemon_en_lista(entrenador->pokemons_objetivo, especie);
}

bool tiene_pokemons_que_no_necesita(void* entrenador_void)
{
	Entrenador* entrenador = entrenador_void;
	bool entrenador_tiene_de_mas(void* pokemon) { return tiene_de_mas(entrenador, pokemon); }
	return list_any_satisfy(entrenador->pokemons_atrapados, &entrenador_tiene_de_mas);
}

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
	bool tiene_pokemon_en_la_cantidad_necesaria(void* pokemon)
	{
		char* especie = ((Pokemon*) pokemon)->especie;
		bool tiene_pokemon = tengo_pokemon(entrenador, especie);
		bool tiene_cantidad = cantidad_de_pokemon_en_lista(entrenador->pokemons_atrapados, especie) == cantidad_de_pokemon_en_lista(entrenador->pokemons_objetivo, especie);
		return tiene_pokemon && tiene_cantidad;
	}
	return list_all_satisfy(entrenador->pokemons_objetivo, &tiene_pokemon_en_la_cantidad_necesaria);
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

t_list* obtener_entrenadores_disponibles_para_atrapar()
{
	t_list* entrenadores_disponibles = list_duplicate(cola_NEW);
	t_list* entrenadores_bloqueados_disponibles = list_filter(cola_BLOCKED, &puede_planificarse_para_atrapar);
	list_add_all(entrenadores_disponibles, entrenadores_bloqueados_disponibles);
	list_destroy(entrenadores_bloqueados_disponibles);
	return entrenadores_disponibles;
}

char* str_replace(char* orig, char* rep, char* with) {
    char *result; // the return string
    char *ins;    // the next insert point
    char *tmp;    // varies
    int len_rep;  // length of rep (the string to remove)
    int len_with; // length of with (the string to replace rep with)
    int len_front; // distance between rep and end of last rep
    int count;    // number of replacements

    // sanity checks and initialization
    if (!orig || !rep)
        return NULL;
    len_rep = strlen(rep);
    if (len_rep == 0)
        return NULL; // empty rep causes infinite loop during count
    if (!with)
        with = "";
    len_with = strlen(with);

    // count the number of replacements needed
    ins = orig;
    for (count = 0; (tmp = strstr(ins, rep)); ++count) {
        ins = tmp + len_rep;
    }

    tmp = result = malloc(strlen(orig) + (len_with - len_rep) * count + 1);

    if (!result)
        return NULL;

    // first time through the loop, all the variable are set correctly
    // from here on,
    //    tmp points to the end of the result string
    //    ins points to the next occurrence of rep in orig
    //    orig points to the remainder of orig after "end of rep"
    while (count--) {
        ins = strstr(orig, rep);
        len_front = ins - orig;
        tmp = strncpy(tmp, orig, len_front) + len_front;
        tmp = strcpy(tmp, with) + len_with;
        orig += len_front + len_rep; // move to next "end of rep"
    }
    strcpy(tmp, orig);
    return result;
}


//-----------OTRAS FUNCIONES-----------//
void obtener_entrenadores()
{
	inicializar_diccionario_acciones();
	inicializar_diccionario_colas();
	inicializar_diccionario_razones();
	inicializar_diccionario_estado_string();

	char* entrenadores = config_get_string_value(config,"POSICIONES_ENTRENADORES");

	char** posiciones = config_get_array_value(config,"POSICIONES_ENTRENADORES");
	char* pokemons_atrapados_string = config_get_string_value(config,"POKEMON_ENTRENADORES");
	char** pokemons_objetivo = config_get_array_value(config,"OBJETIVOS_ENTRENADORES");

	char* rep_pokemons_atrapados = str_replace(pokemons_atrapados_string, ",,", ", ,");
	char* rep_pokemons_atrapados_pivot = str_replace(rep_pokemons_atrapados, "[,", "[ ,");
	free(rep_pokemons_atrapados);
	rep_pokemons_atrapados = str_replace(rep_pokemons_atrapados_pivot, ",]", ", ]");
	free(rep_pokemons_atrapados_pivot);

	char** pokemons_atrapados = string_get_string_as_array(rep_pokemons_atrapados);

	int cantidadPokemonsAtrapados;
	for(cantidadPokemonsAtrapados = 0; pokemons_atrapados[cantidadPokemonsAtrapados] != NULL; cantidadPokemonsAtrapados++);

	int cantidad_entrenadores = 1;
	for(int i=0;entrenadores[i]!='\0';i++)
		cantidad_entrenadores += entrenadores[i] == ',';

	for(int i = 0; i<cantidad_entrenadores; i++)
	{
		if (i < cantidadPokemonsAtrapados)
		{
			list_add(cola_NEW, crear_entrenador(posiciones[i], pokemons_atrapados[i], pokemons_objetivo[i], i+1));
			free(posiciones[i]);
			free(pokemons_atrapados[i]);
			free(pokemons_objetivo[i]);
		}
		else
		{
			list_add(cola_NEW, crear_entrenador(posiciones[i], NULL, pokemons_objetivo[i], i+1));
			free(posiciones[i]);
			free(pokemons_objetivo[i]);
		}
	}

	free(posiciones);
	free(pokemons_atrapados);
	free(rep_pokemons_atrapados);
	free(pokemons_objetivo);
}

void actualizar_estado(Entrenador* entrenador)
{
	if(tiene_todos_sus_pokemons(entrenador))
	{
		terminar_hilo(entrenador);
		cambiar_estado_a(entrenador, EXIT, A_EXIT);
	}
	else
	{
		habilitar_entrenador(entrenador);
		cambiar_estado_a(entrenador, BLOCKED, FIN_EXEC);
	}
}

void capturo_pokemon(Entrenador* entrenador)
{
	char* especie_pokemon_atrapada = datos_accion_actual(entrenador)->info;
	agregar_pokemon(entrenador->pokemons_atrapados, especie_pokemon_atrapada);
	log_info(logger, "El entrenador %d atrapo un %s en la posicion (%d,%d)", entrenador->ID, especie_pokemon_atrapada, entrenador->posicion.posX, entrenador->posicion.posY);
	resetear_acciones(entrenador);
	actualizar_estado(entrenador);
}

void fallo_captura_pokemon(Entrenador* entrenador)
{
	agregar_pokemon(pokemons_necesarios, (char*) (datos_accion_actual(entrenador)->info));
	habilitar_entrenador(entrenador);
}

void ejecutar_entrenador_actual()
{
	if(hay_entrenador_en_ejecucion())
		pthread_mutex_unlock(&(entrenador_EXEC->mutex));
	else
	{
		//log_info(logger, "IDLE");
		sleep((unsigned) config_get_int_value(config,"RETARDO_CICLO_CPU"));
		pthread_mutex_unlock(&(mutex_team));
	}
}

t_list* especies_que_tiene_y_no_necesita(Entrenador* entrenador)
{
	t_list* pokemons_de_mas = list_create();
	for(int i=0;i<entrenador->pokemons_atrapados->elements_count;i++)
	{
		Pokemon* pokemon = (Pokemon*) list_get(entrenador->pokemons_atrapados, i);

		if(tiene_de_mas(entrenador, pokemon))
			list_add(pokemons_de_mas, pokemon->especie);
	}
	return pokemons_de_mas;
}

bool esta_en_EXIT(Entrenador* entrenador) { return entrenador->estado == EXIT; }
bool necesita_pokemon(Entrenador* entrenador, Pokemon* pokemon) { return tiene_pokemon(entrenador->pokemons_objetivo, pokemon->especie); }
bool necesita_especie(Entrenador* entrenador, char* especie)
{
	t_list* especies = list_map(entrenador->pokemons_objetivo, (void*) &obtener_especie_pokemon);
	bool necesita = tiene_especie(especies, especie);
	list_destroy(especies);
	return necesita;
}

bool necesita_algun_pokemon_de(Entrenador* entrenador, t_list* pokemons)
{
	bool entrenador_necesita_pokemon(void* pokemon) { return necesita_pokemon(entrenador, (Pokemon*) pokemon); }
	return list_any_satisfy(pokemons, &entrenador_necesita_pokemon);
}

bool necesita_alguna_especie_de(Entrenador* entrenador, t_list* especies)
{
	bool entrenador_necesita_especie(void* especie) { return necesita_especie(entrenador, (char*) especie); }
	return list_any_satisfy(especies, &entrenador_necesita_especie);
}

char* especie_que_necesita_de(Entrenador* entrenador, t_list* especies)
{
	bool entrenador_necesita_especie(void* especie) { return necesita_especie(entrenador, especie); }
	for(int i=0; i<especies->elements_count;i++)
		if(entrenador_necesita_especie(list_get(especies, i))) return list_get(especies, i);
	return NULL;
}

bool tienen_pokemons_para_intercambiar(Entrenador* entrenador_1, Entrenador* entrenador_2)
{
	t_list* especies_que_le_sobran_a_entrenador_1 = especies_que_tiene_y_no_necesita(entrenador_1);
	t_list* especies_que_le_sobran_a_entrenador_2 = especies_que_tiene_y_no_necesita(entrenador_2);

	bool tienen = necesita_alguna_especie_de(entrenador_2, especies_que_le_sobran_a_entrenador_1) || necesita_alguna_especie_de(entrenador_1, especies_que_le_sobran_a_entrenador_2);

	list_destroy(especies_que_le_sobran_a_entrenador_1);
	list_destroy(especies_que_le_sobran_a_entrenador_2);

	return tienen;
}

bool hay_entrenador_en_ejecucion() { return entrenador_EXEC!=NULL; }
bool listas_vacias() {
	return list_is_empty(cola_BLOCKED) && list_is_empty(cola_READY);
}
bool estamos_en_deadlock() { return !necesitamos_pokemons() && !hay_entrenador_en_ejecucion() && cola_BLOCKED->elements_count > 1;}

t_list* obtener_entrenadores_que_podrian_intercambiar()
{
	bool tiene_pokemons_que_no_necesita_y_esta_disponible(void* entrenador)	{ return tiene_pokemons_que_no_necesita(entrenador) && esta_disponible(entrenador); }
	return list_filter(cola_BLOCKED, &tiene_pokemons_que_no_necesita_y_esta_disponible);
}

void pokemons_que_van_a_intercambiar(Entrenador* entrenador_1, Entrenador* entrenador_2, char** pokemon1, char** pokemon2)
{
	t_list* pokemons_que_le_sobran_a_entrenador_1 = especies_que_tiene_y_no_necesita(entrenador_1);
	t_list* pokemons_que_le_sobran_a_entrenador_2 = especies_que_tiene_y_no_necesita(entrenador_2);

	if(necesita_alguna_especie_de(entrenador_1, pokemons_que_le_sobran_a_entrenador_2))
	{
		char* especieQueNecesita = especie_que_necesita_de(entrenador_1, pokemons_que_le_sobran_a_entrenador_2);
		*pokemon1 = malloc(strlen(especieQueNecesita)+1);
		strcpy(*pokemon1, especieQueNecesita);
	}
	else
	{
		char* especieQueNecesita = list_get(pokemons_que_le_sobran_a_entrenador_2, 0);
		*pokemon1 = malloc(strlen(especieQueNecesita)+1);
		strcpy(*pokemon1, especieQueNecesita);
	}
	if(necesita_alguna_especie_de(entrenador_2, pokemons_que_le_sobran_a_entrenador_1))
	{
		char* especieQueNecesita = especie_que_necesita_de(entrenador_2, pokemons_que_le_sobran_a_entrenador_1);
		*pokemon2 = malloc(strlen(especieQueNecesita)+1);
		strcpy(*pokemon2, especieQueNecesita);
	}
	else
	{
		char* especieQueNecesita = list_get(pokemons_que_le_sobran_a_entrenador_1, 0);
		*pokemon2 = malloc(strlen(especieQueNecesita)+1);
		strcpy(*pokemon2, especieQueNecesita);
	}

	list_destroy(pokemons_que_le_sobran_a_entrenador_1);
	list_destroy(pokemons_que_le_sobran_a_entrenador_2);
}

t_list* obtener_entrenadores_que_pueden_intercambiar_con(Entrenador* entrenador, t_list* entrenadores)
{
	bool tienen_pokemons_para_intercambiar_con_entrenador(Entrenador* entrenador2) { return tienen_pokemons_para_intercambiar(entrenador, entrenador2); }
	return list_filter(entrenadores, (void*) &tienen_pokemons_para_intercambiar_con_entrenador);
}

//-----------A-----------//
static void operacion_ASIGNACION_ID(Cliente* cliente, Paquete* paquete)
{
	DATOS_ID_MENSAJE datos;
	if(!DeserializarM_ID_MENSAJE(paquete, &datos)) log_error(logger, "Error al intentar deserializar el mensaje.");

	Entrenador* entrenador = cliente->info;

	pthread_mutex_lock(&entrenadoresEsperandoCaught);
	list_add(id_mensajes_esperados, cliente->info);
	entrenador->id_mensaje_espera = datos.id;
	pthread_mutex_unlock(&entrenadoresEsperandoCaught);

	log_info(logger, "Enviado CATCH_POKEMON, esperando CAUGHT id: %d", datos.id);
}

//-----------ACCIONES-----------//
static void ciclo(Entrenador* entrenador)
{
	while(true)
	{
		pthread_mutex_lock(&(entrenador->mutex));
		((Accion) dictionaryInt_get(diccionario_acciones, datos_accion_actual(entrenador)->tipo_accion)) (entrenador);
		entrenador->ciclosTotales++;
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
	log_info(logger, "El entrenador %d intenta atrapar un %s en la posicion (%d,%d)", entrenador->ID, datos_accion_actual(entrenador)->info, entrenador->posicion.posX, entrenador->posicion.posY);

	//Cliente* cliente = crear_cliente_de_broker();
	Eventos* eventos = Eventos_Crear0();
	Eventos_AgregarOperacion(eventos, BROKER_ID_MENSAJE, (EventoOperacion) &operacion_ASIGNACION_ID);
	Cliente* cliente = CrearCliente(config_get_string_value(config, "IP_BROKER"), config_get_int_value(config, "PUERTO_BROKER"), eventos);
	if(cliente != NULL)
	{
		DATOS_CATCH_POKEMON* datos = malloc(sizeof(DATOS_CATCH_POKEMON));
		datos->pokemon = datos_accion_actual(entrenador)->info;
		datos->posicion = entrenador->posicion;
		cliente->info = entrenador;
		EnviarMensaje(cliente, CATCH_POKEMON, datos, (Serializador) &SerializarM_CATCH_POKEMON);
		free(datos);

		deshabilitar_entrenador(entrenador);
		cambiar_estado_a(entrenador, BLOCKED, ESPERA_POKEMON);
	}
	else
	{
		log_info(logger, "[Capturando sin Broker]");
		capturo_pokemon(entrenador);
	}
}
static void intercambiar_pokemon_en_progreso(Entrenador* entrenador)
{
	log_info(logger, "Los entrenadores %d y %d estan intercambiando pokemons en la posicion (%d,%d)", entrenador->ID, ((Entrenador*) datos_accion_actual(entrenador)->info)->ID, entrenador->posicion.posX, entrenador->posicion.posY);
	datos_accion_actual(entrenador)->info = NULL;
	siguiente_accion(entrenador);
}
static void intercambiar_pokemon_finalizado(Entrenador* entrenador)
{
	Entrenador* entrenador2 = datos_accion_actual(entrenador)->info;
	datos_accion_actual(entrenador)->info = NULL;
	char* pokemon1 = entrenador->info;
	char* pokemon2 = entrenador2->info;

	tomar_pokemon(entrenador2->pokemons_atrapados, pokemon1);
	agregar_pokemon(entrenador->pokemons_atrapados, pokemon1);

	tomar_pokemon(entrenador->pokemons_atrapados, pokemon2);
	agregar_pokemon(entrenador2->pokemons_atrapados, pokemon2);

	log_info(logger, "Los entrenadores %d y %d intercambiaron %s-%s pokemons en la posicion (%d,%d)", entrenador->ID, entrenador2->ID, pokemon1, pokemon2, entrenador->posicion.posX, entrenador->posicion.posY);

	informeDLs.resueltos ++;
	actualizar_estado(entrenador);
	actualizar_estado(entrenador2);

	free(entrenador->info);
	free(entrenador2->info);

	siguiente_accion(entrenador);
}
static void terminar(Entrenador* entrenador) { pthread_exit(NULL); }

//-----------DICCIONARIO DE ACCIONES-----------//
static void inicializar_diccionario_acciones()
{
	diccionario_acciones = dictionaryInt_create();
	dictionaryInt_put(diccionario_acciones, MOVER, &mover_una_casilla_hacia);
	dictionaryInt_put(diccionario_acciones, CAPTURAR_POKEMON, &capturar_pokemon);
	dictionaryInt_put(diccionario_acciones, INTERCAMBIAR_POKEMON_EN_PROGRESO, &intercambiar_pokemon_en_progreso);
	dictionaryInt_put(diccionario_acciones, INTERCAMBIAR_POKEMON_FINALIZADO, &intercambiar_pokemon_finalizado);
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

//-----------DICCIONARIO DE ESTADO_STRING-----------//
static void inicializar_diccionario_estado_string()
{
	diccionario_estado_string = dictionaryInt_create();
	dictionaryInt_put(diccionario_estado_string, NEW, "NEW");
	dictionaryInt_put(diccionario_estado_string, EXEC, "EXEC");
	dictionaryInt_put(diccionario_estado_string, READY, "READY");
	dictionaryInt_put(diccionario_estado_string, BLOCKED, "BLOCKED");
	dictionaryInt_put(diccionario_estado_string, EXIT, "EXIT");
}

//-----------DICCIONARIO DE RAZONES-----------//

static void inicializar_diccionario_razones()
{
	diccionario_razones = dictionaryInt_create();
	dictionaryInt_put(diccionario_razones, FIN_QUANTUM, "fin de quantum");
	dictionaryInt_put(diccionario_razones, IO, "entreda/salida");
	dictionaryInt_put(diccionario_razones, FIN_EXEC, "fin de ejecucion");
	dictionaryInt_put(diccionario_razones, CAPTURA, "asignarsele una captura pokemon");
	dictionaryInt_put(diccionario_razones, INTERCAMBIO, "asignarsele un intercambio");
	dictionaryInt_put(diccionario_razones, A_EXEC, "ser elegido por el planificador");
	dictionaryInt_put(diccionario_razones, A_EXIT, "haber obtenido todos sus pokemons");
	dictionaryInt_put(diccionario_razones, ESPERA_POKEMON, "estar intentando atrapar un pokemon");
}
