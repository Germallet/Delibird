#include "../Utils/dictionaryInt.h"

typedef enum
{
	NINGUNA = 0,
	I_PLANIFICAR = 1,
	I_CAUGHT_POKEMON = 2,
	I_APPEARED_POKEMON = 3,
	I_LOCALIZED_POKEMON = 4,
	I_ASIGNACION_ID = 5,
	I_TERMINAR = 6
}Tipo_Interrupcion;

typedef struct
{
	Tipo_Interrupcion tipo;
	void* info;
}Datos_Interrupcion;
typedef void (*Interrupcion)(void*);

extern t_dictionaryInt* diccionario_interrupciones;
extern t_list* cola_INTERRUPCIONES;

void inicializar_interrupciones();
void ejecutar_interrupcion();
bool hay_interrupciones();

void interrupcion_TERMINAR(void* dato); //TODO Sacar
