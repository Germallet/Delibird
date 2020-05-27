#include "broker.h"
#include "mensaje.h"
#include "clienteBroker.h"
#include "../Utils/dictionaryInt.h"
#include <stdlib.h>
#include <pthread.h>

void* memoria;
pthread_mutex_t mutexMemoria;

void IniciarMemoria(int tamanioMemoria)
{
	memoria = malloc(tamanioMemoria);
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

	// TODO: Mostrar particiones

	fprintf(archivo, "-----------------------------------------------------------------------------------------------------------------------------\n");

	fclose(archivo);
	log_info(logger, "Dump creado (dump.txt)");
}

void DestruirMemoria()
{
	free(memoria);
}
