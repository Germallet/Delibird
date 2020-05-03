#pragma once
#include "paquete.h"
#include <stdbool.h>
#include <string.h>
#include <netdb.h>

typedef enum
{
	// CLIENTE -> BROKER
	BROKER_CONECTAR,
	BROKER_RECONECTAR,
	BROKER_SUSCRIBIRSE,
	// BROKER -> CLIENTE
	BROKER_CONECTADO,
	BROKER_SUSCRITO,
	// CUALQUIERA -> CUALQUIERA
	NEW_POKEMON,
	APPEARED_POKEMON,
	CATCH_POKEMON,
	CAUGHT_POKEMON,
	GET_POKEMON,
	LOCALIZED_POKEMON
} CodigoDeOperacion;

typedef struct
{
	uint32_t id;
} BROKER_DATOS_CONECTADO, Broker_DATOS_RECONECTAR;

typedef struct {
	uint32_t posX;
	uint32_t posY;
} Posicion;

typedef struct {
	void* datos;
	uint32_t ID;
} DATOS_MENSAJE;

typedef struct {
	void* datos;
	uint32_t ID;
	uint32_t ID_CORRELATIVO;
} DATOS_MENSAJE_RTA;

typedef struct {
	uint32_t largoPokemon;
	char* pokemon;
	Posicion posicion;
	uint32_t cantidad;
} DATOS_NEW_POKEMON;

typedef struct {
	uint32_t largoPokemon;
	char* pokemon;
	Posicion posicion; //TENIA ID
} DATOS_APPEARED_POKEMON;

typedef struct {
	uint32_t largoPokemon;
	char* pokemon;
	Posicion posicion;
} DATOS_CATCH_POKEMON;

typedef struct {
	uint32_t capturado; //TENIA ID
} DATOS_CAUGHT_POKEMON;

typedef struct {
	uint32_t largoPokemon;
	char* pokemon;
} DATOS_GET_POKEMON;

typedef struct {
	uint32_t largoPokemon;
	char* pokemon;
	uint32_t cantidad;
	Posicion posiciones[];
} DATOS_LOCALIZED_POKEMON; //TENIA ID

extern void* Serializar_APPEARED_POKEMON(DATOS_APPEARED_POKEMON* datos, int* tamanioBuffer);
extern void* Serializar_NEW_POKEMON(DATOS_NEW_POKEMON* datos, int* tamanioBuffer);
extern void* Serializar_CATCH_POKEMON(DATOS_CATCH_POKEMON* datos, int* tamanioBuffer);
extern void* Serializar_GET_POKEMON(DATOS_GET_POKEMON* datos, int* tamanioBuffer);
extern void* Serializar_CAUGHT_POKEMON(DATOS_CAUGHT_POKEMON* datos, int* tamanioBuffer);
extern void* Serializar_LOCALIZED_POKEMON(DATOS_LOCALIZED_POKEMON* datos, int* tamanioBuffer);
extern void* Serializar_ID_MENSAJE(uint32_t* ID, void* buffer, int* tamanioBuffer);

extern bool Deserializar_APPEARED_POKEMON(Paquete* paquete, DATOS_APPEARED_POKEMON* datos);
extern bool Deserializar_NEW_POKEMON(Paquete* paquete, DATOS_NEW_POKEMON* datos);
extern bool Deserializar_GET_POKEMON(Paquete* paquete, DATOS_GET_POKEMON* datos);
extern bool Deserializar_CATCH_POKEMON(Paquete* paquete, DATOS_CATCH_POKEMON* datos);
extern bool Deserializar_CAUGHT_POKEMON(Paquete* paquete, DATOS_CAUGHT_POKEMON* datos);
extern bool Deserializar_LOCALIZED_POKEMON(Paquete* paquete, DATOS_LOCALIZED_POKEMON* datos);
extern bool Deserializar_NEW_POKEMON(Paquete* paquete, DATOS_NEW_POKEMON* datos);

extern void* Serializar_BROKER_RECONECTAR(Broker_DATOS_RECONECTAR* datos, int* tamanioBuffer);
extern bool Deserializar_BROKER_RECONECTAR(int socket, Broker_DATOS_RECONECTAR* datos);
extern void* Serializar_BROKER_CONECTADO(Broker_DATOS_RECONECTAR* datos, int* tamanioBuffer);
extern bool Deserializar_BROKER_CONECTADO(int socket, Broker_DATOS_RECONECTAR* datos);

