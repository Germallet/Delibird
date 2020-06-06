#include "../Utils/protocolo.h"
#include "../Utils/dictionaryInt.h"

typedef enum
{
	NINGUNA = 0,
	I_CAUGHT_POKEMON = 1,
	I_APPEARED_POKEMON = 2,
	I_LOCALIZED_POKEMON = 3,
}Tipo_Interrupcion;

typedef struct
{
	Tipo_Interrupcion tipo;
	void* info;
}Datos_Interrupcion;

typedef void (*Interrupcion)(void*);

typedef struct
{
	DATOS_CAUGHT_POKEMON* recibidos;
	Entrenador* entrenador;
}datos_interrupcion_CAUGHT_POKEMON;

extern t_dictionaryInt* diccionario_interrupciones;
extern t_list* cola_INTERRUPCIONES;

void agregar_interrupcion(Tipo_Interrupcion tipo, void* info);
void inicializar_interrupciones();
void ejecutar_interrupcion();
bool hay_interrupciones_para_ejecutar();
