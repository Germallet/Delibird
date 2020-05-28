#include "broker.h"
#include "mensaje.h"
#include "clienteBroker.h"
#include "particion.h"
#include "dynamicPartitioning.h"
#include "buddySystem.h"
#include "selectorParticionLibre.h"
#include "../Utils/dictionaryInt.h"
#include <stdlib.h>
#include <pthread.h>

void* memoria;
t_list* particiones;
pthread_mutex_t mutexMemoria;

Particion* (*creadorParticiones)(int);
//Particion* (*reemplazoParticion)(int);
Particion* (*selectorParticion)(int);
int frecuenciaCompactacion;

void IniciarMemoria(int tamanioMemoria, char* algoritmoMemoria, char* algoritmoReemplazo, char* algoritmoSeleccion, int _frecuenciaCompactacion)
{
	// algoritmoMemoria
	if (strcmp(algoritmoMemoria, "PARTICIONES") == 0)
		creadorParticiones = &DP_CrearParticion;
	else if (strcmp(algoritmoMemoria, "BS") == 0)
		creadorParticiones = &BS_CrearParticion;
	else
	{
		log_error(logger, "Algorimo de memoria inválido");
		exit(-1);
	}

	// algoritmoMemoria
	/*if (strcmp(algoritmoReemplazo, "FIFO") == 0)
		reemplazoParticion = &DP_CrearParticion;
	else if (strcmp(algoritmoReemplazo, "LRU") == 0)
		reemplazoParticion = &BS_CrearParticion;
	else
	{
		log_error(logger, "Algorimo de memoria inválido");
		exit(-1);
	}*/

	// algoritmoSeleccion
	if (strcmp(algoritmoSeleccion, "FF") == 0)
		selectorParticion = &SelectorFF;
	else if (strcmp(algoritmoSeleccion, "BF") == 0)
		selectorParticion = &SelectorBF;
	else
	{
		log_error(logger, "Algorimo de selección inválido");
		exit(-1);
	}

	frecuenciaCompactacion = _frecuenciaCompactacion;

	memoria = malloc(tamanioMemoria);
	particiones = list_create();
	list_add(particiones, creadorParticiones(tamanioMemoria));
	pthread_mutex_init(&mutexMemoria, NULL);
}

void GuardarMensaje(Mensaje* mensaje, Stream* contenido)
{
	Particion* particion = NULL;
	int intentos = 0;

	pthread_mutex_lock(&mutexMemoria);
	while(true)
	{
		particion = selectorParticion(contenido->tamanio);
		if (particion != NULL)
			break;
		intentos++;

		if (intentos == frecuenciaCompactacion || (frecuenciaCompactacion == -1 && particiones->elements_count == 1 && !((Particion*)(list_get(particiones, 0)))->ocupado))
		{
			//TODO: compactar solamente cuando se hayan eliminado todas las particione???
			//TODO: Compactar();
			intentos = 0;
		}
		else
		{
			//TODO: reemplazoParticion();
		}
	}

	particion->id = mensaje->id;
	particion->ocupado = true;
	particion->cola = mensaje->tipoDeMensaje;
	memcpy(contenido->base, memoria + particion->inicio, contenido->tamanio);
	pthread_mutex_unlock(&mutexMemoria);
}

void* ObtenerContenidoMensaje(Mensaje* mensaje)
{
	return mensaje->contenido;
}

void Dump()
{
	FILE* archivo = fopen("dump.txt", "a");
	if(archivo == NULL) {
		log_error(logger, "No se pudo crear Dump (dump.txt)");
		return;
	}

	fprintf(archivo, "-----------------------------------------------------------------------------------------------------------------------------\n");

	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	fprintf(archivo, "Dump: %d/%d/%d %d:%d:%d\n", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900, tm.tm_hour, tm.tm_min, tm.tm_sec);

	pthread_mutex_lock(&mutexMemoria);
	for(int indice = 0; indice < particiones->elements_count; indice++)
		Particion_Dump((Particion*)(list_get(particiones, indice)), archivo, indice+1);
	pthread_mutex_unlock(&mutexMemoria);

	fclose(archivo);
	log_info(logger, "Dump! (dump.txt)");
}

void DestruirMemoria()
{
	free(memoria);
	list_destroy_and_destroy_elements(particiones, (void*)&Particion_Destruir);
}
