#pragma once
#include "../Utils/codigoDeCola.h"
#include <stdbool.h>
#include <stdio.h>

typedef struct {
	int inicio;
	int tamanio;
	bool ocupado;
	int LRU;
	CodigoDeCola cola;
	int id;
} Particion;

Particion* Particion_Crear(int inicio, int tamanio);
void Particion_Destruir(Particion* particion);
void Particion_Dump(Particion* particion, FILE* archivo, int numero);
