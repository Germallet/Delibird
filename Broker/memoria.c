#include "broker.h"
#include "mensaje.h"
#include "clienteBroker.h"
#include "particion.h"
#include "dynamicPartitioning.h"
#include "buddySystem.h"
#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>

void* memoria;
pthread_mutex_t mutexMemoria;
int tamanioMinimo;

void (*compactar)();
Particion* (*seleccionar)(int);
void (*eliminarParticion)();
void (*dump)();
void (*destuir)();
int frecuenciaCompactacion;

void IniciarMemoria(int tamanioMemoria, int _tamanioMinimo, char* algoritmoMemoria, char* algoritmoReemplazo, char* algoritmoSeleccion, int _frecuenciaCompactacion)
{
	tamanioMinimo = _tamanioMinimo;

	// algoritmoMemoria
	if (strcmp(algoritmoMemoria, "PARTICIONES") == 0) // Particiones dinámicas
	{
		compactar = &DP_Compactar;
		dump = &DP_Dump;
		destuir = &DP_Destruir;

		if (strcmp(algoritmoSeleccion, "FF") == 0)
			seleccionar = &DP_Seleccionar_FF;
		else if (strcmp(algoritmoSeleccion, "BF") == 0)
			seleccionar = &DP_Seleccionar_BF;
		else
		{
			log_error(logger, "Algorimo de selección inválido");
			exit(-1);
		}

		if (strcmp(algoritmoReemplazo, "FIFO") == 0)
			eliminarParticion = &DP_Eliminar_FIFO;
		else if (strcmp(algoritmoReemplazo, "LRU") == 0)
			eliminarParticion = &DP_Eliminar_LRU;
		else
		{
			log_error(logger, "Algorimo de reemplazo inválido");
			exit(-1);
		}

		DP_Inicializar(tamanioMemoria);
	}
	else if (strcmp(algoritmoMemoria, "BS") == 0) // BuddySystem
	{
		compactar = &BS_Compactar;
		seleccionar = &BS_Seleccionar;
		dump = &BS_Dump;
		destuir = &BS_Destruir;

		if (strcmp(algoritmoReemplazo, "FIFO") == 0)
			eliminarParticion = &BS_Eliminar_FIFO;
		else if (strcmp(algoritmoReemplazo, "LRU") == 0)
			eliminarParticion = &BS_Eliminar_LRU;
		else
		{
			log_error(logger, "Algorimo de reemplazo inválido");
			exit(-1);
		}
	}
	else
	{
		log_error(logger, "Algorimo de memoria inválido");
		exit(-1);
	}

	frecuenciaCompactacion = _frecuenciaCompactacion;

	memoria = malloc(tamanioMemoria);
	pthread_mutex_init(&mutexMemoria, NULL);
}

void GuardarMensaje(Mensaje* mensaje, CodigoDeCola tipoDeMensaje, Stream* contenido)
{
	Particion* particion = NULL;
	int intentos = 0;

	pthread_mutex_lock(&mutexMemoria);
	while(true)
	{
		particion = seleccionar(contenido->tamanio);
		if (particion != NULL)
			break;
		intentos++;

		if (intentos == frecuenciaCompactacion/* || (frecuenciaCompactacion == -1 && particiones->elements_count == 1 && !((Particion*)(list_get(particiones, 0)))->ocupado)*/)
		{
			compactar();
			intentos = 0;
		}
		else
			eliminarParticion();
	}

	particion->id = mensaje->id;
	particion->ocupado = true;
	particion->cola = tipoDeMensaje;
	particion->tiempoAsignado = clock();
	particion->tiempoUpdated = clock();
	memcpy(memoria + particion->base, contenido->base, contenido->tamanio);
	pthread_mutex_unlock(&mutexMemoria);

	mensaje->particion = particion;
}

void* ObtenerContenidoMensaje(Mensaje* mensaje)
{
	void* contenido = malloc(mensaje->tamanio);
	pthread_mutex_lock(&mutexMemoria);
	memcpy(contenido, memoria + mensaje->particion->base, mensaje->tamanio);
	pthread_mutex_unlock(&mutexMemoria);
	return contenido;
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
	dump(archivo);
	pthread_mutex_unlock(&mutexMemoria);

	fclose(archivo);
	log_info(logger, "Dump! (dump.txt)");
}

void DestruirMemoria()
{
	free(memoria);
	destuir();
}
