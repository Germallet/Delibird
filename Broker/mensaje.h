#pragma once
#include "../Utils/protocolo.h"
#include "../Utils/net.h"
#include <commons/collections/list.h>

typedef struct
{
	int id;
	int posicionEnMemoria;
	t_list* clientesEnviados;
	t_list* clientesACK;
	// TODO: Variables temporales hasta verdadera implementación
	CodigoDeCola tipoDeMensaje;
	void* contenido;
} Mensaje;

void IniciarMensajes();
Mensaje* CrearMensaje(CodigoDeCola tipoDeMensaje, void* contenido);
bool Mensaje_SeLeEnvioA(Mensaje* mensaje, void* cliente);
void Mensaje_EnviarA(Mensaje* mensaje, void* contenido, Cliente* cliente);
