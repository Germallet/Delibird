#include <stdbool.h>
#include <commons/config.h>
#include <commons/string.h>
#include <string.h>
#include <stdio.h>
#include "../Utils/protocolo.h"
#include "arbol.h"

typedef struct {
	Posicion pos;
	uint32_t cantidad;
} DatosBloques;

bool sonIguales(char* a, char* b);
bool estaAbierto(char* path);
bool esDirectorio(char* path);
void crearArchivo(char* path);
