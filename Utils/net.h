#pragma once
#include "eventos.h"
#include <netdb.h>
#include <pthread.h>

typedef struct
{
	int socket;
	uint16_t puerto;
	Eventos* eventos;
	pthread_t* thread;
} Servidor;

//typedef struct Cliente; //Para evitar referencia circular fue declarado en eventos.h

extern Cliente* CrearCliente(char *ip, char* puerto, Eventos* eventos);
extern Servidor* CrearServidor(uint16_t puerto, Eventos* eventos);
extern void DestruirCliente(Cliente* cliente);
extern void DestruirServidor(Servidor* servidor);
