#include "../Utils/protocolo.h"
#include <pthread.h>
#include <commons/collections/list.h>

extern t_list* cola_NEW;
extern t_list* cola_READY;
extern t_list* cola_BLOCKED;
extern t_list* cola_EXIT;

typedef enum
{
	NEW = 0,
	READY = 1,
	EXEC = 2,
	BLOCKED = 3,
	EXIT = 4
}Estado_Entrenador;

typedef enum
{
	TERMINAR = 0,
	MOVER = 1,
	CAPTURAR_POKEMON = 2,
	INTERCAMBIAR_POKEMON_EN_PROGRESO = 3,
	INTERCAMBIAR_POKEMON_FINALIZADO = 4
}Tipo_Accion;

typedef enum
{
	FIN_QUANTUM = 0,
	IO = 1,
	FIN_EXEC = 2,
	CAPTURA = 3,
	INTERCAMBIO = 4,
	A_EXEC = 5,
	A_EXIT = 6,
	ESPERA_POKEMON = 7
}Razon;

typedef struct
{
	Tipo_Accion tipo_accion;
	void* info;
}Datos_Accion;

typedef struct
{
	int ID;
	Posicion posicion;
	t_list* pokemons_atrapados;
	t_list* pokemons_objetivo;
	Estado_Entrenador estado;
	bool esta_disponible;

	pthread_t hilo;
	pthread_mutex_t mutex;
	uint32_t id_mensaje_espera;

	void* info;
	int indice_accion_actual;
	t_list* datos_acciones;

	uint32_t ciclosTotales;
}Entrenador;

extern Entrenador* entrenador_EXEC;

int cantidad_cambios_de_contexto;

typedef void (*Accion)(Entrenador*);

Entrenador* crear_entrenador(char* posicion, char* pokemons_atrapados, char* pokemons_objetivo, int ID);
Entrenador* tomar_entrenador(t_list* lista_entrenadores);
Posicion* obtener_posicion_entrenador(void* entrenador);
t_list* obtener_entrenadores_que_pueden_intercambiar_con(Entrenador* entrenador, t_list* entrenadores);
t_list* obtener_entrenadores_disponibles_para_atrapar();
t_list* obtener_entrenadores_que_podrian_intercambiar();
void cargar_accion(Entrenador* entrenador, Tipo_Accion tipo ,void* info);
void asignar_id_mensaje_espera(Entrenador* entrenador, uint* nuevo_id);
void destruir_entrenador(void* entrenador_void);
void obtener_entrenadores();
void poner_entrenador_en_EXEC();
void ejecutar_entrenador_actual();
void capturo_pokemon(Entrenador* entrenador);
void fallo_captura_pokemon(Entrenador* entrenador);
void habilitar_entrenador(Entrenador* entrenador);
void deshabilitar_entrenador(Entrenador* entrenador);
void cambiar_estado_a(Entrenador* entrenador, Estado_Entrenador estado, Razon razon);
void pokemons_que_van_a_intercambiar(Entrenador* entrenador_1, Entrenador* entrenador_2, char** pokemon1, char** pokemon2);
bool puede_seguir_atrapando_pokemons(Entrenador* entrenador);
bool puede_planificarse_para_atrapar(void* entrenador);
bool esta_en_EXIT(Entrenador* entrenador);
bool hay_entrenador_en_ejecucion();
bool estamos_en_deadlock();
