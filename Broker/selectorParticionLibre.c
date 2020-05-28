#include "selectorParticionLibre.h"
#include "memoria.h"

Particion* SelectorFF(int tamanio)
{
	for(int indice = 0; indice < particiones->elements_count; indice++)
	{
		Particion* particion = (Particion*)(list_get(particiones, indice));
		if (!particion->ocupado && particion->tamanio >= tamanio)
			return particion;
	}
	return NULL;
}

Particion* SelectorBF(int tamanio)
{
	Particion* mejorParticion = NULL;

	for(int indice = 0; indice < particiones->elements_count; indice++)
	{
		Particion* particion = (Particion*)(list_get(particiones, indice));
		if (!particion->ocupado && particion->tamanio >= tamanio)
		{
			if (mejorParticion == NULL || particion->tamanio < mejorParticion->tamanio)
				mejorParticion = particion;
		}
	}
	return mejorParticion;
}
