#include "hiloTimer.h"
#include <stdlib.h>
#include <unistd.h>

void EjecucionTimer(HiloTimer* hiloTimer)
{
	while (hiloTimer->evento != NULL && hiloTimer->repeticiones != 0) {
		pthread_mutex_lock(&(hiloTimer->mxHiloTimer));
		(hiloTimer->evento)(hiloTimer->info);

		if (hiloTimer->repeticiones != -1) hiloTimer->repeticiones--;

		pthread_mutex_unlock(&(hiloTimer->mxHiloTimer));
		sleep(hiloTimer->tiempo);
	}
	if (hiloTimer->info != NULL) free(hiloTimer->info);

	free(hiloTimer);
	pthread_exit(0);
}

HiloTimer* CrearHiloTimer(int repeticiones, unsigned int tiempo, void (*evento)(void*), void* info)
{
	HiloTimer* nuevoHilo = malloc(sizeof(HiloTimer));
	nuevoHilo->repeticiones = repeticiones;
	nuevoHilo->tiempo = tiempo;
	nuevoHilo->info = info;
	nuevoHilo->evento = evento;
	pthread_mutex_init(&(nuevoHilo->mxHiloTimer), NULL);
	pthread_create(&(nuevoHilo->thread), NULL, (void*)&EjecucionTimer, nuevoHilo);
	pthread_detach(nuevoHilo->thread);
	return nuevoHilo;
}

void DetenerHiloTimer(HiloTimer* hiloTimer)
{
	pthread_mutex_lock(&(hiloTimer->mxHiloTimer));
	hiloTimer->evento = NULL;
	pthread_mutex_unlock(&(hiloTimer->mxHiloTimer));
}
