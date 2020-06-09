#include <stdio.h>
#include <commons/collections/list.h>

typedef struct {
	char* nombre;
	t_list* hijos;
} NodoArbol;

void arbol_init(char* puntoMontaje);
void agregarAlArbol(char* nombre);
