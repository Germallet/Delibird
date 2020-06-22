#include "particion.h"
#include "broker.h"
#include <stdlib.h>

Particion* Particion_Crear(int inicio, int tamanio)
{
	Particion* nuevaParticion = malloc(sizeof(Particion));
	nuevaParticion->base = inicio;
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
			"Partición %d: 0x%X - 0x%X.    [%c]    Size: %db    LRU: %li    Cola: %s   ID: %d\n",
			numero,
			particion->base,
			particion->base + particion->tamanio,
			'X',
			particion->tamanio,
			particion->tiempoUpdated,
			CodigoDeColaAString(particion->cola),
			particion->id
		);
	else
		fprintf(archivo,
			"Partición %d: 0x%X - 0x%X.    [%c]    Size: %db\n",
			numero,
			particion->base,
			particion->base + particion->tamanio,
			'L',
			particion->tamanio
		);
}

Particion* Particion_Combinar(Particion* particionA, Particion* particionB, int posA, int posB)
{
	particionA->tamanio += particionB->tamanio;
	Particion_Destruir(particionB);
	log_info(logger, "Particiones consolidadas (%d, %d)", posA, posB);
	return particionA;
}
