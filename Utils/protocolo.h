#pragma once
#include "paquete.h"
#include <stdbool.h>
#include <string.h>
#include <netdb.h>
#include <stdlib.h>

typedef enum
{
	// CLIENTE -> BROKER
	BROKER_CONECTAR, 	//0
	BROKER_RECONECTAR, 	//1
	BROKER_SUSCRIBIRSE, //2
	BROKER_ACK,			//3
	// BROKER -> CLIENTE
	BROKER_CONECTADO,	//4
	BROKER_SUSCRITO,	//5
	BROKER_ID_MENSAJE,	//6
	// CUALQUIERA -> CUALQUIERA
	NEW_POKEMON,		//7
	APPEARED_POKEMON,	//8
	CATCH_POKEMON,		//9
	CAUGHT_POKEMON,		//10
	GET_POKEMON,		//11
	LOCALIZED_POKEMON	//12
} CodigoDeOperacion;

typedef enum
{
	COLA_NEW_POKEMON,		//0
	COLA_APPEARED_POKEMON,	//1
	COLA_CATCH_POKEMON,		//2
	COLA_CAUGHT_POKEMON,	//3
	COLA_GET_POKEMON,		//4
	COLA_LOCALIZED_POKEMON	//5
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
} DATOS_APPEARED_POKEMON;

typedef struct {
	DATOS_APPEARED_POKEMON datos;
	uint32_t idCorrelativo_NEW;
} DATOS_APPEARED_POKEMON_ID;

typedef struct {
	DATOS_APPEARED_POKEMON_ID datos;
	uint32_t id;
} DATOS_APPEARED_POKEMON_IDx2;

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
	uint32_t idCorrelativo_CATCH;
	uint32_t capturado;
} DATOS_CAUGHT_POKEMON_ID;

typedef struct {
	DATOS_CAUGHT_POKEMON_ID datos;
	uint32_t id;
} DATOS_CAUGHT_POKEMON_IDx2;

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
	uint32_t idCorrelativo_GET;
} DATOS_LOCALIZED_POKEMON_ID;

typedef struct {
	DATOS_LOCALIZED_POKEMON_ID datos;
	uint32_t id;
} DATOS_LOCALIZED_POKEMON_IDx2;

extern void* Serializar_ID_MENSAJE(DATOS_ID_MENSAJE* datos, int* tamanioBuffer);
extern void* Serializar_NEW_POKEMON(DATOS_NEW_POKEMON* datos, int* tamanioBuffer);
extern void* Serializar_NEW_POKEMON_ID(DATOS_NEW_POKEMON_ID* datos, int* tamanioBuffer);
extern void* Serializar_APPEARED_POKEMON(DATOS_APPEARED_POKEMON* datos, int* tamanioBuffer);
extern void* Serializar_APPEARED_POKEMON_ID(DATOS_APPEARED_POKEMON_ID* datos, int* tamanioBuffer);
extern void* Serializar_CATCH_POKEMON(DATOS_CATCH_POKEMON* datos, int* tamanioBuffer);
extern void* Serializar_CATCH_POKEMON_ID(DATOS_CATCH_POKEMON_ID* datos, int* tamanioBuffer);
extern void* Serializar_CAUGHT_POKEMON_ID(DATOS_CAUGHT_POKEMON_ID* datos, int* tamanioBuffer);
extern void* Serializar_GET_POKEMON(DATOS_GET_POKEMON* datos, int* tamanioBuffer);
extern void* Serializar_GET_POKEMON_ID(DATOS_GET_POKEMON_ID* datos, int* tamanioBuffer);
extern void* Serializar_LOCALIZED_POKEMON_ID(DATOS_LOCALIZED_POKEMON_ID* datos, int* tamanioBuffer);

extern bool Deserializar_ID_MENSAJE(Paquete* paquete, DATOS_ID_MENSAJE* datos);
extern bool Deserializar_NEW_POKEMON(Paquete* paquete, DATOS_NEW_POKEMON* datos);
extern bool Deserializar_NEW_POKEMON_ID(Paquete* paquete, DATOS_NEW_POKEMON_ID* datos);
extern bool Deserializar_APPEARED_POKEMON(Paquete* paquete, DATOS_APPEARED_POKEMON* datos);
extern bool Deserializar_APPEARED_POKEMON_ID(Paquete* paquete, DATOS_APPEARED_POKEMON_ID* datos);
extern bool Deserializar_APPEARED_POKEMON_IDx2(Paquete* paquete, DATOS_APPEARED_POKEMON_IDx2* datos);
extern bool Deserializar_CATCH_POKEMON(Paquete* paquete, DATOS_CATCH_POKEMON* datos);
extern bool Deserializar_CATCH_POKEMON_ID(Paquete* paquete, DATOS_CATCH_POKEMON_ID* datos);
extern bool Deserializar_CAUGHT_POKEMON_ID(Paquete* paquete, DATOS_CAUGHT_POKEMON_ID* datos);
extern bool Deserializar_CAUGHT_POKEMON_IDx2(Paquete* paquete, DATOS_CAUGHT_POKEMON_IDx2* datos);
extern bool Deserializar_GET_POKEMON(Paquete* paquete, DATOS_GET_POKEMON* datos);
extern bool Deserializar_GET_POKEMON_ID(Paquete* paquete, DATOS_GET_POKEMON_ID* datos);
extern bool Deserializar_LOCALIZED_POKEMON_ID(Paquete* paquete, DATOS_LOCALIZED_POKEMON_ID* datos);
extern bool Deserializar_LOCALIZED_POKEMON_IDx2(Paquete* paquete, DATOS_LOCALIZED_POKEMON_IDx2* datos);

extern void* Serializar_BROKER_RECONECTAR(BROKER_DATOS_RECONECTAR* datos, int* tamanioBuffer);
extern bool Deserializar_BROKER_RECONECTAR(Paquete* paquete, BROKER_DATOS_RECONECTAR* datos);
extern void* Serializar_BROKER_CONECTADO(BROKER_DATOS_CONECTADO* datos, int* tamanioBuffer);
extern bool Deserializar_BROKER_CONECTADO(Paquete* paquete, BROKER_DATOS_CONECTADO* datos);
void* Serializar_BROKER_SUSCRIBIRSE(BROKER_DATOS_SUSCRIBIRSE* datos, int* tamanioBuffer);
bool Deserializar_BROKER_SUSCRIBIRSE(Paquete* paquete, BROKER_DATOS_SUSCRIBIRSE* datos);
