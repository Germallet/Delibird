#pragma once
#include "paquete.h"
#include <stdbool.h>
#include <string.h>
#include <netdb.h>
#include <stdlib.h>

typedef enum
{
	// CLIENTE -> BROKER
	BROKER_CONECTAR,
	BROKER_RECONECTAR,
	BROKER_SUSCRIBIRSE,
	// BROKER -> CLIENTE
	BROKER_CONECTADO,
	BROKER_SUSCRITO,
	BROKER_ID_MENSAJE,
	// CUALQUIERA -> CUALQUIERA
	NEW_POKEMON,
	APPEARED_POKEMON,
	CATCH_POKEMON,
	CAUGHT_POKEMON,
	GET_POKEMON,
	LOCALIZED_POKEMON
} CodigoDeOperacion;

typedef enum
{
	COLA_NEW_POKEMON,
	COLA_APPEARED_POKEMON,
	COLA_CATCH_POKEMON,
	COLA_CAUGHT_POKEMON,
	COLA_GET_POKEMON,
	COLA_LOCALIZED_POKEMON
} CodigoDeCola;

typedef struct
{
	uint32_t id;
} BROKER_DATOS_CONECTADO, BROKER_DATOS_RECONECTAR;

typedef struct
{
	CodigoDeCola cola;
} BROKER_DATOS_SUSCRIBIRSE;

typedef struct {
	uint32_t posX;
	uint32_t posY;
} Posicion;

typedef struct {
	uint32_t id;
} DATOS_ID_MENSAJE;

typedef struct {
	uint32_t largoPokemon;
	char* pokemon;
	Posicion posicion;
	uint32_t cantidad;
} DATOS_NEW_POKEMON;

typedef struct {
	DATOS_NEW_POKEMON datos;
	uint32_t id;
} DATOS_NEW_POKEMON_ID;

typedef struct {
	uint32_t largoPokemon;
	char* pokemon;
	Posicion posicion;
	uint32_t idNew;
} DATOS_APPEARED_POKEMON;

typedef struct {
	DATOS_APPEARED_POKEMON datos;
	uint32_t id;
} DATOS_APPEARED_POKEMON_ID;

typedef struct {
	uint32_t largoPokemon;
	char* pokemon;
	Posicion posicion;
} DATOS_CATCH_POKEMON;

typedef struct {
	DATOS_CATCH_POKEMON datos;
	uint32_t id;
} DATOS_CATCH_POKEMON_ID;

typedef struct {
	uint32_t idCatch;
	uint32_t capturado;
} DATOS_CAUGHT_POKEMON;

typedef struct {
	DATOS_CAUGHT_POKEMON datos;
	uint32_t id;
} DATOS_CAUGHT_POKEMON_ID;

typedef struct {
	uint32_t largoPokemon;
	char* pokemon;
} DATOS_GET_POKEMON;

typedef struct {
	DATOS_GET_POKEMON datos;
	uint32_t id;
} DATOS_GET_POKEMON_ID;

typedef struct {
	uint32_t largoPokemon;
	char* pokemon;
	uint32_t cantidad;
	Posicion* posiciones;
	uint32_t idGet;
} DATOS_LOCALIZED_POKEMON; //TENIA ID

extern void* Serializar_NEW_POKEMON(DATOS_NEW_POKEMON* datos, int* tamanioBuffer);
extern void* Serializar_NEW_POKEMON_ID(DATOS_NEW_POKEMON_ID* datos, int* tamanioBuffer);
extern void* Serializar_APPEARED_POKEMON(DATOS_APPEARED_POKEMON* datos, int* tamanioBuffer);
extern void* Serializar_CATCH_POKEMON(DATOS_CATCH_POKEMON* datos, int* tamanioBuffer);
extern void* Serializar_CATCH_POKEMON_ID(DATOS_CATCH_POKEMON_ID* datos, int* tamanioBuffer);
extern void* Serializar_CAUGHT_POKEMON(DATOS_CAUGHT_POKEMON* datos, int* tamanioBuffer);
extern void* Serializar_GET_POKEMON(DATOS_GET_POKEMON* datos, int* tamanioBuffer);
extern void* Serializar_GET_POKEMON_ID(DATOS_GET_POKEMON_ID* datos, int* tamanioBuffer);
extern void* Serializar_LOCALIZED_POKEMON(DATOS_LOCALIZED_POKEMON* datos, int* tamanioBuffer);
extern void* Serializar_ID_MENSAJE(DATOS_ID_MENSAJE* datos, int* tamanioBuffer);

extern bool Deserializar_APPEARED_POKEMON(Paquete* paquete, DATOS_APPEARED_POKEMON* datos);
extern bool Deserializar_NEW_POKEMON(Paquete* paquete, DATOS_NEW_POKEMON* datos);
extern bool Deserializar_GET_POKEMON(Paquete* paquete, DATOS_GET_POKEMON* datos);
extern bool Deserializar_CATCH_POKEMON(Paquete* paquete, DATOS_CATCH_POKEMON* datos);
extern bool Deserializar_CAUGHT_POKEMON(Paquete* paquete, DATOS_CAUGHT_POKEMON* datos);
extern bool Deserializar_LOCALIZED_POKEMON(Paquete* paquete, DATOS_LOCALIZED_POKEMON* datos);
extern bool Deserializar_ID_MENSAJE(Paquete* paquete, DATOS_ID_MENSAJE* datos);

extern void* Serializar_BROKER_RECONECTAR(BROKER_DATOS_RECONECTAR* datos, int* tamanioBuffer);
extern bool Deserializar_BROKER_RECONECTAR(Paquete* paquete, BROKER_DATOS_RECONECTAR* datos);
extern void* Serializar_BROKER_CONECTADO(BROKER_DATOS_CONECTADO* datos, int* tamanioBuffer);
extern bool Deserializar_BROKER_CONECTADO(Paquete* paquete, BROKER_DATOS_CONECTADO* datos);
void* Serializar_BROKER_SUSCRIBIRSE(BROKER_DATOS_SUSCRIBIRSE* datos, int* tamanioBuffer);
bool Deserializar_BROKER_SUSCRIBIRSE(Paquete* paquete, BROKER_DATOS_SUSCRIBIRSE* datos);
