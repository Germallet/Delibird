#include "arbol.h"

NodoArbol* crearNodo(char* nombre) {
	NodoArbol* nodo = malloc(sizeof(NodoArbol));
	char* a = string_new();
	string_append(&a,"/");
	string_append(&a,nombre);
	nodo->nombre = a;
	nodo->hijos = list_create();
	nodo->padre = NULL;
	return nodo;
}

void agregarNodo(NodoArbol* padre, NodoArbol* hijo) {
	list_add(padre->hijos, &hijo);
	hijo->padre = padre;
}
