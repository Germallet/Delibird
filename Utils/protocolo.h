#pragma once
#include "paquete.h"
#include "codigoDeCola.h"
#include "stream.h"
#include "operacion.h"
#include <stdbool.h>
#include <string.h>
#include <netdb.h>
#include <stdlib.h>

typedef struct {
	uint32_t posX;
	uint32_t posY;
} Posicion;

// NEW_POKEMON
typedef struct {
	char* pokemon;
	Posicion posicion;
	uint32_t cantidad;
} DATOS_NEW_POKEMON;
typedef struct {
	DATOS_NEW_POKEMON datos;
	uint32_t id;
} DATOS_NEW_POKEMON_ID;

// APPEARED_POKEMON
typedef struct {
	char* pokemon;
	Posicion posicion;
} DATOS_APPEARED_POKEMON;
typedef struct {
	DATOS_APPEARED_POKEMON datos;
	uint32_t id;
} DATOS_APPEARED_POKEMON_ID;

// CATCH_POKEMON
typedef struct {
	char* pokemon;
	Posicion posicion;
} DATOS_CATCH_POKEMON;
typedef struct {
	DATOS_CATCH_POKEMON datos;
	uint32_t id;
} DATOS_CATCH_POKEMON_ID;

// CAUGHT_POKEMON
typedef struct {
	uint32_t capturado;
} DATOS_CAUGHT_POKEMON;
typedef struct {
	DATOS_CAUGHT_POKEMON datos;
	uint32_t idCorrelativa;
} DATOS_CAUGHT_POKEMON_ID;

// GET_POKEMON
typedef struct {
	char* pokemon;
} DATOS_GET_POKEMON;

typedef struct {
	DATOS_GET_POKEMON datos;
	uint32_t id;
} DATOS_GET_POKEMON_ID;

// LOCALIZED_POKEMON
typedef struct {
	char* pokemon;
	uint32_t cantidad;
	Posicion* posiciones;
} DATOS_LOCALIZED_POKEMON;
typedef struct {
	DATOS_LOCALIZED_POKEMON datos;
	uint32_t id;
} DATOS_LOCALIZED_POKEMON_ID;

// BROKER
typedef struct
{
	uint32_t id;
} BROKER_DATOS_CONECTADO, BROKER_DATOS_RECONECTAR;
typedef struct
{
	uint32_t id;
} DATOS_ID_MENSAJE;
typedef struct
{
	CodigoDeCola cola;
} BROKER_DATOS_SUSCRIBIRSE;

// NEW_POKEMON
extern Stream* SerializarM_NEW_POKEMON(DATOS_NEW_POKEMON* datos);
extern Stream* SerializarM_NEW_POKEMON_ID(DATOS_NEW_POKEMON_ID* datos);
// APPEARED_POKEMON
extern Stream* SerializarM_APPEARED_POKEMON(DATOS_APPEARED_POKEMON* datos);
extern Stream* SerializarM_APPEARED_POKEMON_ID(DATOS_APPEARED_POKEMON_ID* datos);
// CATCH_POKEMON
extern Stream* SerializarM_CATCH_POKEMON(DATOS_CATCH_POKEMON* datos);
extern Stream* SerializarM_CATCH_POKEMON_ID(DATOS_CATCH_POKEMON_ID* datos);
// CAUGHT_POKEMON
extern Stream* SerializarM_CAUGHT_POKEMON(DATOS_CAUGHT_POKEMON* datos);
extern Stream* SerializarM_CAUGHT_POKEMON_ID(DATOS_CAUGHT_POKEMON_ID* datos);
// GET_POKEMON
extern Stream* SerializarM_GET_POKEMON(DATOS_GET_POKEMON* datos);
extern Stream* SerializarM_GET_POKEMON_ID(DATOS_GET_POKEMON_ID* datos);
// LOCALIZED_POKEMON
extern Stream* SerializarM_LOCALIZED_POKEMON(DATOS_LOCALIZED_POKEMON* datos);
extern Stream* SerializarM_LOCALIZED_POKEMON_ID(DATOS_LOCALIZED_POKEMON_ID* datos);
// ID_MENSAJE
extern Stream* SerializarM_ID_MENSAJE(DATOS_ID_MENSAJE* datos);
extern bool DeserializarM_ID_MENSAJE(Paquete* paquete, DATOS_ID_MENSAJE* datos);
// BROKER_RECONECTAR
extern Stream* SerializarM_BROKER_RECONECTAR(BROKER_DATOS_RECONECTAR* datos);
extern bool DeserializarM_BROKER_RECONECTAR(Paquete* paquete, BROKER_DATOS_RECONECTAR* datos);
// BROKER_CONECTADO
extern Stream* SerializarM_BROKER_CONECTADO(BROKER_DATOS_CONECTADO* datos);
extern bool DeserializarM_BROKER_CONECTADO(Paquete* paquete, BROKER_DATOS_CONECTADO* datos);
// BROKER_SUSCRIBIRSE
extern Stream* SerializarM_BROKER_SUSCRIBIRSE(BROKER_DATOS_SUSCRIBIRSE* datos);
extern bool DeserializarM_BROKER_SUSCRIBIRSE(Paquete* paquete, BROKER_DATOS_SUSCRIBIRSE* datos);

extern char* DatosAString_NEW_POKEMON(DATOS_NEW_POKEMON* datos);
extern char* DatosAString_APPEARED_POKEMON(DATOS_APPEARED_POKEMON* datos);
extern char* DatosAString_CATCH_POKEMON(DATOS_CATCH_POKEMON* datos);
extern char* DatosAString_CAUGHT_POKEMON(DATOS_CAUGHT_POKEMON* datos);
extern char* DatosAString_GET_POKEMON(DATOS_GET_POKEMON* datos);
extern char* DatosAString_LOCALIZED_POKEMON(DATOS_LOCALIZED_POKEMON* datos);

