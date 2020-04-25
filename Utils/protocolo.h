#include <stdbool.h>

typedef struct {
	int posX;
	int posY;
} Posicion;

typedef struct {
	char* nombre;
	Posicion posicion;
	int ID_MENSAJE;
} DATOS_APPEARED_POKEMON;

typedef struct
{
	char* nombre;
	Posicion posicion;
	int cantidad;
} DATOS_NEW_POKEMON;

typedef struct
{
	char* nombre;
	Posicion posicion;
} DATOS_CATCH_POKEMON;

typedef struct
{
	int ID_MENSAJE;
	bool capturado;
} DATOS_CAUGHT_POKEMON;

typedef struct
{
	char* nombre;
} DATOS_GET_POKEMON;


typedef struct
{
	int ID_MENSAJE;
	char* nombre;
	Posicion* posiciones;
	int* cantidades;
} DATOS_LOCALIZED_POKEMON;

extern DATOS_APPEARED_POKEMON* Deserializar_APPEARED_POKEMON();
extern DATOS_NEW_POKEMON* Deserializar_NEW_POKEMON();
extern DATOS_GET_POKEMON* Deserializar_GET_POKEMON();
extern DATOS_CATCH_POKEMON* Deserializar_CATCH_POKEMON();
extern DATOS_CAUGHT_POKEMON* Deserializar_CAUGHT_POKEMON();
extern DATOS_LOCALIZED_POKEMON* Deserializar_LOCALIZED_POKEMON();

extern void* Serializar_APPEARED_POKEMON(DATOS_APPEARED_POKEMON* datos, int* tamanioBuffer);
extern void* Serializar_NEW_POKEMON(DATOS_NEW_POKEMON* datos, int* tamanioBuffer);
extern void* Serializar_CATCH_POKEMON(DATOS_CATCH_POKEMON* datos, int* tamanioBuffer);
extern void* Serializar_GET_POKEMON(DATOS_GET_POKEMON* datos, int* tamanioBuffer);
extern void* Serializar_CAUGHT_POKEMON(DATOS_CAUGHT_POKEMON* datos, int* tamanioBuffer);
extern void* Serializar_LOCALIZED_POKEMON(DATOS_LOCALIZED_POKEMON* datos, int* tamanioBuffer);
