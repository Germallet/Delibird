#include "broker.h"
#include "mensaje.h"
#include "clienteBroker.h"
#include "particion.h"
#include "dynamicPartitioning.h"
#include "buddySystem.h"
#include "../Utils/dictionaryInt.h"
#include <stdlib.h>
#include <pthread.h>

void* memoria;
t_list* particiones;
pthread_mutex_t mutexMemoria;
Particion* (*creadorParticiones)(int);

void IniciarMemoria(int tamanioMemoria, char* algoritmoMemoria)
{
	if (strcmp(algoritmoMemoria, "PARTICIONES") == 0)
		creadorParticiones = &DP_CrearParticion;
	else if (strcmp(algoritmoMemoria, "BS") == 0)
		creadorParticiones = &BS_CrearParticion;
	else
	{
		log_error(logger, "Algorimo de memoria inválido");
		exit(-1);
	}

	memoria = malloc(tamanioMemoria);
	particiones = list_create();
	list_add(particiones, creadorParticiones(tamanioMemoria));
	pthread_mutex_init(&mutexMemoria, NULL);
}

void GuardarMensaje(Mensaje* mensaje, Stream* contenido)
{
	mensaje->contenido = malloc(contenido->tamanio);
	memcpy(mensaje->contenido, contenido->base, contenido->tamanio);
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

	fprintf(archivo, "-----------------------------------------------------------------------------------------------------------------------------\n");

	fclose(archivo);
	log_info(logger, "Dump! (dump.txt)");
}

void DestruirMemoria()
{
	free(memoria);
	list_destroy_and_destroy_elements(particiones, (void*)&Particion_Destruir);
}
