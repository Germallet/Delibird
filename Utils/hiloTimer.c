#include "hiloTimer.h"
#include <stdlib.h>

static void EjecucionTimer(HiloTimer* hiloTimer)
{
	pthread_mutex_lock(&(hiloTimer->mxHiloTimer));
	if (hiloTimer->evento != NULL && hiloTimer->repeticiones != 0)
	{
		(hiloTimer->evento)(hiloTimer->info);
		if (hiloTimer->repeticiones != -1)
			hiloTimer->repeticiones--;
	}
	else
	{
		if (hiloTimer->info != NULL)
			free(hiloTimer->info);
		free(hiloTimer);
		pthread_exit(0);
	}
	pthread_mutex_unlock(&(hiloTimer->mxHiloTimer));
}

HiloTimer* CrearHiloTimer(int repeticiones, void* info, void (*evento)(void*))
{
	HiloTimer* nuevoHilo = malloc(sizeof(HiloTimer));
	nuevoHilo->repeticiones = repeticiones;
	nuevoHilo->info = info;
	nuevoHilo->evento = evento;
	pthread_create(nuevoHilo->thread, NULL, (void*)EjecucionTimer, nuevoHilo);
	pthread_detach(*(nuevoHilo->thread));
	pthread_mutex_init(&(nuevoHilo->mxHiloTimer), NULL);
	return nuevoHilo;
}

void DetenerHiloTimer(HiloTimer* hiloTimer)
{
	pthread_mutex_lock(&(hiloTimer->mxHiloTimer));
	hiloTimer->evento = NULL;
	pthread_mutex_unlock(&(hiloTimer->mxHiloTimer));
}
