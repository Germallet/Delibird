#include "particion.h"
#include "broker.h"
#include <stdlib.h>

Particion* Particion_Crear(int inicio, int tamanio)
{
	Particion* nuevaParticion = malloc(sizeof(Particion));
	nuevaParticion->inicio = inicio;
	nuevaParticion->tamanio = tamanio;
	nuevaParticion->ocupado = false;
	return nuevaParticion;
}

void Particion_Destruir(Particion* particion)
{
	free(particion);
}

void Particion_Dump(Particion* particion, FILE* archivo, int numero)
{
	if (particion->ocupado)
		fprintf(archivo,
			"Partición %d: 0x%X - 0x%X.    [%c]    Size: %db    LRU:%d    Cola:%s   ID:%d\n",
			numero,
			particion->inicio,
			particion->inicio + particion->tamanio,
			'X',
			particion->tamanio,
			particion->LRU,
			CodigoDeColaAString(particion->cola),
			particion->id
		);
	else
		fprintf(archivo,
			"Partición %d: 0x%X - 0x%X.    [%c]    Size: %db\n",
			numero,
			particion->inicio,
			particion->inicio + particion->tamanio,
			'L',
			particion->tamanio
		);
}
