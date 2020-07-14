#include "buddySystem.h"
#include "memoria.h"
#include "broker.h"
#include "mensaje.h"
#include "../Utils/dictionaryInt.h"
#include <string.h>
#include <math.h>

t_list* particiones;

static int ObtenerPosicion(Particion* particion)
{
	for (int indice = 0; indice < particiones->elements_count; indice++)
	{
		if((Particion*)(list_get(particiones, indice)) == particion)
			return indice;
	}
	return -1;
}

void BS_Inicializar(int tamanio)
{
	particiones = list_create();
	list_add(particiones, Particion_Crear(0, tamanio));
}

static int potenciaMasCercana(int numero)
{
	return pow(2, ceil(log(numero)/log(2)));
}

static Particion* Subparticionar(Particion* particion, int tamanio)
{
	if (potenciaMasCercana(particion->tamanio) == tamanio)
		return particion;

	Particion* particionNueva = Particion_Crear(particion->base + (particion->tamanio/2), particion->tamanio/2);
	particion->tamanio = particion->tamanio/2;

	int indice = 0;
	for(; list_get(particiones, indice) != particion; indice++);

	list_add_in_index(particiones, indice+1, particionNueva);

	return Subparticionar(particion, tamanio);
}

Particion* BS_Seleccionar(int tamanio)
{
	Particion* seleccionado = NULL;
	tamanio = tamanio < tamanioMinimo ? tamanioMinimo : tamanio;
	tamanio = potenciaMasCercana(tamanio);

	for (int indice = 0; indice < particiones->elements_count; indice++)
	{
		Particion* particion = (Particion*)(list_get(particiones, indice));
		if (!particion->ocupado && particion->tamanio >= tamanio)
		{
			if (seleccionado == NULL || particion->tamanio < seleccionado->tamanio + tamanioMinimo)
				seleccionado = particion;
		}
	}

	if (seleccionado != NULL)
	{
		if (seleccionado->tamanio > tamanio)
			seleccionado = Subparticionar(seleccionado, tamanio);
		log_info(logger, "Particion ocupada (n: %d, pos: %d, tam: %d)", ObtenerPosicion(seleccionado), seleccionado->base, seleccionado->tamanio);
	}

	return seleccionado;
}

static bool SonBuddy(Particion* particionA, Particion* particionB)
{
	/*log_info(logger, "SonBuddy?");
	log_info(logger, "A: base %d, limite %d", particionA->base, particionA->tamanio);
	log_info(logger, "B: base %d, limite %d", particionB->base, particionB->tamanio);
	bool rta1 = (particionA->tamanio == particionB->tamanio);
	bool rta2 = (particionA->base + particionA->tamanio == particionB->base || particionB->base + particionB->tamanio == particionA->base);
	bool rta3 = (particionA->base == (particionB->base ^ particionA->tamanio) || particionB->base == (particionA->base ^ particionB->tamanio));
	bool rta = rta1 && rta2 && rta3;
	if (particionA->tamanio == particionB->tamanio) log_info(logger, "rta1 S"); else log_info(logger, "rta1 N");
	log_info(logger, "rta2 %d", rta2);
	log_info(logger, "rta3 %d", rta3);
	log_info(logger, "rta %d", rta);*/
	return 	(particionA->tamanio == particionB->tamanio) &&
			(particionA->base + particionA->tamanio == particionB->base || particionB->base + particionB->tamanio == particionA->base) &&
			(particionA->base == (particionB->base ^ particionA->tamanio) || particionB->base == (particionA->base ^ particionB->tamanio));
}
static Particion* ObtenerBuddy(Particion* particion)
{
	bool EsBuddy(Particion* particionB) {return SonBuddy(particion, particionB);}
	return (Particion*)list_find(particiones, (void*)&EsBuddy);
}

static void BS_Consolidar(Particion* particion)
{
	Particion* particionBuddy = ObtenerBuddy(particion);
	if(particionBuddy != NULL && !particionBuddy->ocupado)
	{
		if (particion->base > particionBuddy->base)
		{
			Particion* particionC = particionBuddy;
			particionBuddy = particion;
			particion = particionC;
		}

		int posA = ObtenerPosicion(particion);
		int posB = ObtenerPosicion(particionBuddy);
		list_remove(particiones, posB);
		particion = Particion_Combinar(particion, particionBuddy, posA, posB);
		BS_Consolidar(particion);
	}
}
static void BS_Eliminar(Particion* particion)
{
	Mensajes_EliminarParticion(particion);
	log_info(logger, "Particion eliminada (%d)", ObtenerPosicion(particion));
	particion->ocupado = false;
	BS_Consolidar(particion);
}

void BS_Eliminar_FIFO()
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
	BS_Eliminar(victima);
}
void BS_Eliminar_LRU()
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
	BS_Eliminar(victima);
}

void BS_Dump(FILE* archivo)
{
	for(int indice = 0; indice < particiones->elements_count; indice++)
		Particion_Dump((Particion*)(list_get(particiones, indice)), archivo, indice+1);
}

void BS_Destruir()
{
	list_destroy_and_destroy_elements(particiones, (void*)&Particion_Destruir);
}
