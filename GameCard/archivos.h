#pragma once

#include <stdbool.h>
#include <commons/config.h>
#include <commons/string.h>
#include <string.h>
#include <stdio.h>
#include "../Utils/protocolo.h"
#include <commons/bitarray.h>
#include "arbol.h"
#include "gameCard.h"

typedef struct {
	Posicion pos;
	uint32_t cantidad;
} DatosBloques;

void cerrar(char* path);
void abrir(char* path);

char* pathMetadataBinDe(char* path, char* nombreArchivo);
char* posicionAString(DatosBloques* d);
t_list* convertirBloques(t_list* bloques, int cantBloques);
t_list* interpretarCadena(char* cadenaDatos, int cantBloques, int size);

char* leerOpen(char* path);
int leerSize(char* path);
char* leerDirectorio(char* path);
char* leerBlocksPorConfig(char* path);
void leerArchivos(t_list* bloques, int cantBloques, int size, char* datos);
t_list* leerBlocks(char* path, int* cantBloques);

int tamanioBloque(int* nroBloque);

bool sonIguales(char* a, char* b);
bool estaAbierto(char* path);
bool esDirectorio(char* path);
bool existeDirectorio(char* path);
short existeArchivo(char *path);

void crearBitmap(char* path,int cantBlocks);
void crearArchivo(char* path);
void crearDirectorioFiles();
void crearDirectorioBlocks();
void crearDirectorioMetadata();
void crearBloques(int blocks);
void crearBitmap(char* path, int cantBlocks);

NodoArbol* crearPokemon(char* nombre);
void crearMetadataPokemon(char* path);

DatosBloques* encontrarPosicion(t_list* pokemon, Posicion pos);

int agregarCantidadEnPosicion(t_list* pokemon, DatosBloques posYCant, t_list* numerosBloques, int size);
void escribirListaEnArchivo(t_list* pokemon, int size, t_list* numerosBloques);
void cambiarMetadataPokemon(char* pathPokemon, t_list* numerosBloques, int bytes);








//void last(void** lista, void* valorRetorno);
