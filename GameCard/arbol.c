#include "arbol.h"

NodoArbol* crearNodo(char* nombre) {

	NodoArbol* nodo = malloc(sizeof(NodoArbol));
	nodo->nombre = string_duplicate(nombre);
	nodo->hijos = list_create();
	return nodo;
}

void agregarNodo(NodoArbol* padre, NodoArbol* hijo) {
	list_add(padre->hijos, hijo);
}
