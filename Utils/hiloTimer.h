#pragma once
#include <pthread.h>

typedef struct
{
	int repeticiones;
	int tiempo;
	void* info;
	void (*evento)(void*);
	pthread_t* thread;
	pthread_mutex_t mxHiloTimer;
} HiloTimer;

extern HiloTimer* CrearHiloTimer(int repeticiones, unsigned int tiempo, void (*evento)(void*), void* info);
extern void DetenerHiloTimer(HiloTimer* hiloTimer);
