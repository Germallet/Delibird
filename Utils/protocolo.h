#include <stdbool.h>
#include "../Utils/net.h"
#include <string.h>

typedef struct {
	uint32_t posX;
	uint32_t posY;
} Posicion;

typedef struct {
	uint32_t largoPokemon;
	char* pokemon;
	Posicion posicion;
	uint32_t cantidad;
} DATOS_NEW_POKEMON;

typedef struct {
	uint32_t largoPokemon;
	char* pokemon;
	Posicion posicion;
	uint32_t ID_MENSAJE;
} DATOS_APPEARED_POKEMON;

typedef struct {
	uint32_t largoPokemon;
	char* pokemon;
	Posicion posicion;
} DATOS_CATCH_POKEMON;

typedef struct {
	uint32_t ID_MENSAJE;
	uint32_t capturado;
} DATOS_CAUGHT_POKEMON;

typedef struct {
	uint32_t largoPokemon;
	char* pokemon;
} DATOS_GET_POKEMON;


typedef struct {
	uint32_t ID_MENSAJE;
	char* nombre;
	uint32_t cantidad;
	Posicion* posiciones;
} DATOS_LOCALIZED_POKEMON;

extern void* Serializar_APPEARED_POKEMON(DATOS_APPEARED_POKEMON* datos, int* tamanioBuffer);
extern void* Serializar_NEW_POKEMON(DATOS_NEW_POKEMON* datos, int* tamanioBuffer);
extern void* Serializar_CATCH_POKEMON(DATOS_CATCH_POKEMON* datos, int* tamanioBuffer);
extern void* Serializar_GET_POKEMON(DATOS_GET_POKEMON* datos, int* tamanioBuffer);
extern void* Serializar_CAUGHT_POKEMON(DATOS_CAUGHT_POKEMON* datos, int* tamanioBuffer);
extern void* Serializar_LOCALIZED_POKEMON(DATOS_LOCALIZED_POKEMON* datos, int* tamanioBuffer);

extern void* Deserializar(int socket);

extern DATOS_APPEARED_POKEMON* Deserializar_APPEARED_POKEMON();
extern DATOS_NEW_POKEMON* Deserializar_NEW_POKEMON();
extern DATOS_GET_POKEMON* Deserializar_GET_POKEMON();
extern DATOS_CATCH_POKEMON* Deserializar_CATCH_POKEMON();
extern DATOS_CAUGHT_POKEMON* Deserializar_CAUGHT_POKEMON();
extern DATOS_LOCALIZED_POKEMON* Deserializar_LOCALIZED_POKEMON();

