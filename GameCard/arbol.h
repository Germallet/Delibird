#include <stdio.h>
#include <commons/collections/list.h>

typedef struct {
	char* nombre;
	t_list* hijos;
} NodoArbol;

NodoArbol* crearNodo(char* puntoMontaje);
void agregarHijo(NodoArbol* padre, NodoArbol* hijo);
