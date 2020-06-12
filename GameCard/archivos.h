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



bool sonIguales(char* a, char* b);
bool estaAbierto(char* path);
bool esDirectorio(char* path);
int* leerBlocks(char* path, int* cantBloques);
char* leerOpen(char* path);
int leerSize(char* path);
char* leerDirectorio(char* path);
bool existeDirectorio(char* path);
short existeArchivo(char *path);
void crearBitmap(char* path, t_bitarray* bitmap, int cantBlocks);
void crearArchivo(char* path);
void crearDirectorioFiles(NodoArbol* arbol);
void crearDirectorioBlocks(NodoArbol* padre, int blocks);
void crearDirectorioMetadata(NodoArbol* padre, t_bitarray* bitmap, int size, int blocks, char* magicNumber);
void crearBloques(int blocks);
void crearBitmap(char* path, t_bitarray* bitmap, int cantBlocks);
void last(void** lista, void* valorRetorno);
NodoArbol* crearPokemon(char* nombre);
void crearMetadataPokemon(char* path);
void agregarCantidadEnPosicion(t_list* pokemon, DatosBloques posYCant, int* bloques);
char* leerBlocksPorConfig(char* path);
t_list* convertirBloques(int* bloques, int cantBloques);
char* leerArchivos(int* bloques, int cantBloques, int size);
t_list* interpretarCadena(char* cadenaDatos, int cantBloques, int size);
DatosBloques* encontrarPosicion(t_list* pokemon, Posicion pos);
void escribirListaEnArchivo(t_list* pokemon);
