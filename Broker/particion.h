#pragma once
#include "../Utils/codigoDeCola.h"
#include <stdbool.h>
#include <stdio.h>
#include <time.h>

typedef struct {
	int base;
	int tamanio;
	bool ocupado;
	clock_t tiempoAsignado;
	clock_t tiempoUpdated;
	CodigoDeCola cola;
	int id;
} Particion;

Particion* Particion_Crear(int inicio, int tamanio);
void Particion_Destruir(Particion* particion);
void Particion_Dump(Particion* particion, FILE* archivo, int numero);
Particion* Particon_Combinar(Particion* particionA, Particion* particionB);
