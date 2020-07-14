#pragma once
#include "particion.h"
#include "../Utils/protocolo.h"
#include "../Utils/net.h"
#include <commons/collections/list.h>

typedef struct
{
	uint32_t id;
	uint32_t idCorrelativo;
	pthread_mutex_t mutexMensaje; // TODO
	t_list* clientesEnviados;
	t_list* clientesACK;
	Particion* particion;
	int tamanio;
} Mensaje;

void IniciarMensajes();
uint32_t GenerarIDMensaje();
Mensaje* CrearMensaje(CodigoDeCola tipoDeMensaje, uint32_t idCorrelativo, uint32_t id, size_t tamanio);
bool RegistrarACK(uint32_t idMensaje, void* clienteBroker);
bool Mensaje_SeLeEnvioA(Mensaje* mensaje, void* clienteBroker);
void Mensaje_EnviarA(Mensaje* mensaje, CodigoDeCola tipoDeMensaje, void* contenido, Cliente* cliente);
void Mensajes_EliminarParticion(Particion* particion);
