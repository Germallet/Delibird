#pragma once
#include "../Utils/protocolo.h"
#include "../Utils/net.h"
#include <commons/collections/list.h>

typedef struct
{
	uint32_t id;
	int posicionEnMemoria;
	int tamanio;
	pthread_mutex_t mutexMensaje;
	t_list* clientesEnviados;
	t_list* clientesACK;
	// TODO: Variables temporales hasta verdadera implementación
	CodigoDeCola tipoDeMensaje;
	void* contenido;
} Mensaje;

void IniciarMensajes();
uint32_t GenerarIDMensaje();
Mensaje* CrearMensaje(CodigoDeCola tipoDeMensaje, uint32_t id, size_t tamanio);
bool RegistrarACK(uint32_t idMensaje, void* clienteBroker);
bool Mensaje_SeLeEnvioA(Mensaje* mensaje, void* clienteBroker);
void Mensaje_EnviarA(Mensaje* mensaje, void* contenido, Cliente* cliente);
