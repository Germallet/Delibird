#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <commons/collections/list.h>
#include <commons/string.h>

typedef struct {
	char* nombre;
	t_list* hijos;
//	void* padre;
} NodoArbol;

NodoArbol* crearNodo(char* puntoMontaje);
void agregarNodo(NodoArbol* padre, NodoArbol* hijo);
