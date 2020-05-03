#pragma once
#include "dictionaryInt.h"
#include "paquete.h"

typedef enum
{
	ERROR_RECIBIR = 1,
	ERROR_OPERACION_INVALIDA = 2,
	ERROR_PROCESAR_PAQUETE = 3
} ErrorDeEscucha;

typedef void (*Evento)(void);
typedef void (*EventoError)(ErrorDeEscucha, Paquete*);

typedef struct
{
	Evento conectado;
	Evento desconectado;
	EventoError error;
	t_dictionaryInt* operaciones;
} Eventos;

typedef struct
{
	int socket;
	struct sockaddr_in* direccion;
	Eventos eventos;
	pthread_t* thread;
	pthread_mutex_t mx_destruir;
} Cliente;

typedef void (*EventoOperacion)(Cliente*, Paquete*);

extern Eventos* Eventos_Crear0();
extern Eventos* Eventos_Crear2(Evento conectado, Evento desconectado);
extern Eventos* Eventos_Crear3(Evento conectado, Evento desconectado, EventoError error);
extern void Eventos_AgregarOperacion(Eventos* eventos, uint32_t codigoDeOperacion, EventoOperacion evento);
extern bool Eventos_TieneOperacion(Eventos* eventos, uint32_t codigoDeOperacion);
extern EventoOperacion Eventos_ObtenerOperacion(Eventos* eventos, uint32_t codigoDeOperacion);
extern void Eventos_Destruir(Eventos* eventos);
