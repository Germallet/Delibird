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
	INTERCAMBIAR_POKEMON = 3
}Tipo_Accion;

typedef struct
{
	Tipo_Accion tipo_accion;
	void* info;
}Datos_Accion;

typedef struct
{
	int ID;
	Posicion* posicion;
	t_list* pokemons_atrapados;
	t_list* pokemons_objetivo;
	Estado_Entrenador estado;
	bool esta_disponible;

	pthread_t hilo;
	pthread_mutex_t mutex;
	uint32_t id_mensaje_espera;

	int indice_accion_actual;
	t_list* datos_acciones;
}Entrenador;

extern Entrenador* entrenador_EXEC;

typedef void (*Accion)(Entrenador*);

Entrenador* crear_entrenador(char* posicion, char* pokemons_atrapados, char* pokemons_objetivo, int ID);
void destruir_entrenador(void* entrenador_void);
Entrenador* entrenador_mas_cercano(t_list* lista_entrenadores, Posicion* posicion_llegada);
void cargar_accion(Entrenador* entrenador, Tipo_Accion tipo ,void* info);
void capturo_pokemon(Entrenador* entrenador);
void obtener_entrenadores();
void ejecutar_entrenador_actual();
void capturo_pokemon(Entrenador* entrenador);
void fallo_captura_pokemon(Entrenador* entrenador);
void habilitar_entrenador(Entrenador* entrenador);
void deshabilitar_entrenador(Entrenador* entrenador);
void cambiar_estado_a(Entrenador* entrenador, Estado_Entrenador estado);
t_list* entrenadores_disponibles();


