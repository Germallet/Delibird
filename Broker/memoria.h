#pragma once
#include "mensaje.h"
#include "../Utils/protocolo.h"

void IniciarMemoria();
Mensaje* CrearMensaje(CodigoDeCola tipoDeMensaje, void* contenido);
void GuardarMensaje(Mensaje* mensaje);
void* ObtenerContenidoMensaje(Mensaje* mensaje);
