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
void crearArchivo(char* path);
bool existeDirectorio(char* path);
void crearDirectorioFiles(NodoArbol* arbol);
void crearDirectorioBlocks(NodoArbol* padre);
void crearDirectorioMetadata(NodoArbol* padre, t_bitarray* bitmap);
void crearBitmap(char* path, t_bitarray* bitmap, uint32_t cantBlocks);
char* last(char** lista);
char* crearPokemon(char* nombre);
char* encontrarPokemon(char* nombre);
void agregarCantidadEnPosicion(DatosBloques* pokemon,Posicion pos, uint32_t cantidad);
char* leerBlocksPorConfig(char* path);

