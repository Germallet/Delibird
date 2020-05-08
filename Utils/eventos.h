#ifndef UTILS_EVENTOS
#define UTILS_EVENTOS

#include "dictionaryInt.h"
#include "paquete.h"

typedef enum
{
	ERROR_RECIBIR = 1,
	ERROR_OPERACION_INVALIDA = 2,
	ERROR_PROCESAR_PAQUETE = 3
} ErrorDeEscucha;

typedef void (*EventoGeneral)(void*); // Donde void* es Cliente*
typedef void (*EventoError)(ErrorDeEscucha, Paquete*);
typedef void (*EventoOperacion)(void*, Paquete*); // Donde void* es Cliente*

typedef struct
{
	EventoGeneral conectado;
	EventoGeneral desconectado;
	EventoError error;
	t_dictionaryInt* operaciones;
} Eventos;

extern Eventos* Eventos_Crear0();
extern Eventos* Eventos_Crear2(EventoGeneral conectado, EventoGeneral desconectado);
extern Eventos* Eventos_Crear3(EventoGeneral conectado, EventoGeneral desconectado, EventoError error);
extern void Eventos_AgregarOperacion(Eventos* eventos, uint32_t codigoDeOperacion, EventoOperacion evento);
extern bool Eventos_TieneOperacion(Eventos* eventos, uint32_t codigoDeOperacion);
extern EventoOperacion Eventos_ObtenerOperacion(Eventos* eventos, uint32_t codigoDeOperacion);
extern void Eventos_Destruir(Eventos* eventos);

#endif
