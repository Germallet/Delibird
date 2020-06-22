#include "dynamicPartitioning.h"
#include "memoria.h"
#include "broker.h"
#include "../Utils/dictionaryInt.h"
#include <string.h>

t_list* particiones;
int tamanioTotal;

static int ObtenerPosicion(Particion* particion)
{
	for (int indice = 0; indice < particiones->elements_count; indice++)
	{
		if((Particion*)(list_get(particiones, indice)) == particion)
			return indice;
	}
	return -1;
}

void DP_Inicializar(int tamanio)
{
	tamanioTotal = tamanio;
	particiones = list_create();
	list_add(particiones, Particion_Crear(0, tamanioTotal));
}

static Particion* Subparticionar(Particion* particion, int tamanio)
{
	Particion* particionNueva = Particion_Crear(particion->base + tamanio, particion->tamanio - tamanio);
	particion->tamanio = tamanio;

	int indice = 0;
	for(; list_get(particiones, indice) != particion; indice++);

	list_add_in_index(particiones, indice+1, particionNueva);
	return particionNueva;
}

Particion* DP_Seleccionar_FF(int tamanio)
{
	tamanio = tamanio < tamanioMinimo ? tamanioMinimo : tamanio;

	for (int indice = 0; indice < particiones->elements_count; indice++)
	{
		Particion* particion = (Particion*)(list_get(particiones, indice));
		if (!particion->ocupado && particion->tamanio >= tamanio/* + tamanioMinimo*/)
		{
			if (particion->tamanio > tamanio)
				Subparticionar(particion, tamanio);
			log_info(logger, "Particion ocupada (n: %d, pos: %d, tam: %d)", ObtenerPosicion(particion), particion->base, particion->tamanio);
			return particion;
		}
	}
	return NULL;
}
Particion* DP_Seleccionar_BF(int tamanio)
{
	Particion* mejorParticion = NULL;

	if(tamanio < tamanioMinimo)
		tamanio = tamanioMinimo;

	for (int indice = 0; indice < particiones->elements_count; indice++)
	{
		Particion* particion = (Particion*)(list_get(particiones, indice));
		if (!particion->ocupado && particion->tamanio >= tamanio)
		{
			if (mejorParticion == NULL || particion->tamanio < mejorParticion->tamanio/* + tamanioMinimo*/)
				mejorParticion = particion;
		}
	}
	if (mejorParticion->tamanio > tamanio)
		Subparticionar(mejorParticion, tamanio);

	if (mejorParticion != NULL)
		log_info(logger, "Particion ocupada (n: %d, pos: %d, tam: %d)", ObtenerPosicion(mejorParticion), mejorParticion->base, mejorParticion->tamanio);
	return mejorParticion;
}

void DP_Compactar()
{
	int base = 0;
	for(int indice = 0; indice < particiones->elements_count; indice++)
	{
		Particion* particion = list_get(particiones, indice);

		if(particion->ocupado)
		{
			memmove(memoria + base, memoria + particion->base, particion->tamanio);
			particion->base = base;
			base += particion->tamanio;
		}
		else
		{
			Particion_Destruir(particion);
			list_remove(particiones, indice);
			indice--;
		}
	}

	if(base != tamanioTotal-1)
		list_add(particiones, Particion_Crear(base, tamanioTotal - base));
}

static void DP_Consolidar(Particion* particion)
{
	int indice = 0;
	for(; list_get(particiones, indice) != particion; indice++);

	if(indice != 0)
	{
		Particion* particionA = (Particion*)list_get(particiones, indice-1);
		if (!particionA->ocupado)
		{
			particion = Particion_Combinar(particionA, particion, ObtenerPosicion(particionA), ObtenerPosicion(particion));
			list_remove(particiones, indice);
			indice--;
		}
	}
	if(indice != particiones->elements_count-1)
	{
		Particion* particionB = (Particion*)list_get(particiones, indice+1);
		if (!particionB->ocupado)
		{
			Particion_Combinar(particion, particionB, ObtenerPosicion(particion), ObtenerPosicion(particionB));
			list_remove(particiones, indice+1);
		}
	}
}
static void DP_Eliminar(Particion* particion)
{
	log_info(logger, "Particion eliminada (%d)", ObtenerPosicion(particion));
	particion->ocupado = false;
	DP_Consolidar(particion);
}

void DP_Eliminar_FIFO()
{
	if(particiones->elements_count == 0)
		return;
	Particion* victima = NULL;
	for (int indice = 0; indice < particiones->elements_count; indice++)
	{
		Particion* particionActual = (Particion*)(list_get(particiones, indice));
		if (particionActual->ocupado && (victima == NULL || particionActual->tiempoAsignado < victima->tiempoAsignado))
			victima = particionActual;
	}
	DP_Eliminar(victima);
}
void DP_Eliminar_LRU()
{
	if(particiones->elements_count == 0)
		return;
	Particion* victima = NULL;
	for (int indice = 0; indice < particiones->elements_count; indice++)
	{
		Particion* particionActual = (Particion*)(list_get(particiones, indice));
		if (particionActual->ocupado && (victima == NULL || particionActual->tiempoUpdated < victima->tiempoUpdated))
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
