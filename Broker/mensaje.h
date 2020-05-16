#pragma once
#include "../Utils/protocolo.h"
#include "../Utils/net.h"
#include <commons/collections/list.h>

typedef struct
{
	int id;
	int posicionEnMemoria;
	int tamanio;
	t_list* clientesEnviados;
	t_list* clientesACK;
	// TODO: Variables temporales hasta verdadera implementación
	CodigoDeCola tipoDeMensaje;
	void* contenido;
} Mensaje;

void IniciarMensajes();
uint32_t GenerarIDMensaje();
Mensaje* CrearMensaje(CodigoDeCola tipoDeMensaje, uint32_t id, size_t tamanio);
bool Mensaje_SeLeEnvioA(Mensaje* mensaje, void* cliente);
void Mensaje_EnviarA(Mensaje* mensaje, void* contenido, Cliente* cliente);
