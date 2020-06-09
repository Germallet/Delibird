#include "arbol.h"

NodoArbol* arbol_init(char* puntoMontaje) {
	NodoArbol* raiz;
	raiz->nombre = puntoMontaje;
	return raiz;
}

void agregarAlArbol(NodoArbol* nodo) {

}

NodoArbol* crearNodo(char* nombre) {
	NodoArbol* nodo;
	nodo->nombre = nombre;
	nodo->hijos = list_create();
	return nodo;
}

void agregarHijo(NodoArbol* padre, NodoArbol* hijo) {
	list_add(padre->hijos, &hijo);
	agregarAlArbol(hijo);
}
