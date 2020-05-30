#include "dynamicPartitioning.h"
#include "particion.h"
#include "../Utils/dictionaryInt.h"

t_list* particiones;

void DP_Inicializar(int tamanio)
{
	particiones = list_create();
	list_add(particiones, Particion_Crear(0, tamanio));
}

static Particion* Subparticionar(Particion* particion, int tamanio)
{
	Particion* particionNueva = Particion_Crear(particion->base + tamanio, particion->tamanio - tamanio);
	particion->tamanio = tamanio;
	return particionNueva;
}

Particion* DP_Seleccionar_FF(int tamanio)
{
	for (int indice = 0; indice < particiones->elements_count; indice++)
	{
		Particion* particion = (Particion*)(list_get(particiones, indice));
		if (!particion->ocupado && particion->tamanio >= tamanio)
		{
			if (particion->tamanio > tamanio)
				Subparticionar(particion, tamanio);
			return particion;
		}
	}
	return NULL;
}
Particion* DP_Seleccionar_BF(int tamanio)
{
	Particion* mejorParticion = NULL;

	for (int indice = 0; indice < particiones->elements_count; indice++)
	{
		Particion* particion = (Particion*)(list_get(particiones, indice));
		if (!particion->ocupado && particion->tamanio >= tamanio)
		{
			if (mejorParticion == NULL || particion->tamanio < mejorParticion->tamanio)
			{
				if (particion->tamanio > tamanio)
					Subparticionar(particion, tamanio);
				return particion;
			}
		}
	}
	return mejorParticion;
}

void DP_Compactar()
{
}

static void DP_Eliminar(Particion* particion)
{
	particion->ocupado = false;
}

void DP_Eliminar_FIFO()
{
	if(particiones->elements_count == 0)
		return;
	Particion* victima = list_get(particiones, 0);
	for (int indice = 1; indice < particiones->elements_count; indice++)
	{
		Particion* particionActual = (Particion*)(list_get(particiones, indice));
		if (particionActual->tiempoAsignado < victima->tiempoAsignado)
			victima = particionActual;
	}
	DP_Eliminar(victima);
}
void DP_Eliminar_LRU()
{
	if(particiones->elements_count == 0)
		return;
	Particion* victima = list_get(particiones, 0);
	for (int indice = 1; indice < particiones->elements_count; indice++)
	{
		Particion* particionActual = (Particion*)(list_get(particiones, indice));
		if (particionActual->tiempoUpdated > victima->tiempoUpdated)
			victima = particionActual;
	}
	DP_Eliminar(victima);
}

void DP_Dump(FILE* archivo)
{
	for(int indice = 0; indice < particiones->elements_count; indice++)
		Particion_Dump((Particion*)(list_get(particiones, indice)), archivo, indice+1);
}

void DP_Destruir()
{
	list_destroy_and_destroy_elements(particiones, (void*)&Particion_Destruir);
}
