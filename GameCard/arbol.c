#include "arbol.h"

NodoArbol* crearNodo(char* nombre) {
	NodoArbol* nodo;
	nodo->nombre = nombre;
	nodo->hijos = list_create();
	return nodo;
}

void agregarHijo(NodoArbol* padre, NodoArbol* hijo) {
	list_add(padre->hijos, &hijo);
}
