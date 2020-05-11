#pragma once
#include "eventos.h"
#include "protocolo.h"
#include <netdb.h>
#include <pthread.h>

typedef struct
{
	int socket;
	struct sockaddr_in* direccion;
	Eventos* eventos;
	pthread_t* thread;
	pthread_mutex_t mx_destruir;
	void* info;
} Cliente;

typedef struct
{
	int socket;
	uint16_t puerto;
	Eventos* eventos;
	pthread_t* thread;
	pthread_mutex_t mx_destruir;
} Servidor;

typedef void* (*Serializador)(void*,int*);

extern Cliente* CrearCliente(char *ip, uint16_t puerto, Eventos* eventos);
extern Servidor* CrearServidor(char* ip, uint16_t puerto, Eventos* eventos);
extern void DestruirCliente(Cliente* cliente);
extern void DestruirServidor(Servidor* servidor);
extern int EnviarMensaje(Cliente* cliente, CodigoDeOperacion codigoDeOperacion, void* datos, Serializador serializador);
