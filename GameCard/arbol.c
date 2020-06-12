#include "arbol.h"

NodoArbol* crearNodo(char* nombre) {
	NodoArbol* nodo = malloc(sizeof(NodoArbol));
	nodo->nombre = nombre;
	nodo->hijos = list_create();
	nodo->padre = NULL;
	return nodo;
}

void agregarNodo(NodoArbol* padre, NodoArbol* hijo) {
	list_add(padre->hijos, &hijo);
	hijo->padre = padre;
}
