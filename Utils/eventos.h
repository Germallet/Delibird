#pragma once
#include <commons/collections/dictionary.h>
#include "paquete.h"

typedef enum
{
	MENSAJE = 1,
	NEW_POKEMON = 2,
	APPEARED_POKEMON = 3,
	CATCH_POKEMON = 4,
	CAUGHT_POKEMON = 5,
	GET_POKEMON = 6,
	LOCALIZED_POKEMON = 7,
	SUSCRIPTOR = 8,
	OK = 0
} CodigoDeOperacion;

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
	t_dictionary* operaciones;
} Eventos;

typedef struct
{
	int socket;
	struct sockaddr_in* direccion;
	Eventos eventos;
	pthread_t* thread;
} Cliente;

typedef void (*EventoOperacion)(Cliente*, Paquete*);

extern Eventos* Eventos_Crear(Evento conectado, Evento desconectado, EventoError error);
extern void Eventos_AgregarOperacion(Eventos* eventos, CodigoDeOperacion codigoDeOperacion, EventoOperacion evento);
extern bool Eventos_TieneOperacion(Eventos* eventos, CodigoDeOperacion codigoDeOperacion);
extern EventoOperacion Eventos_ObtenerOperacion(Eventos* eventos, CodigoDeOperacion codigoDeOperacion);
extern void Eventos_Destruir(Eventos* eventos);
