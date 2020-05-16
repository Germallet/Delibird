#pragma once
#include "mensaje.h"
#include "../Utils/protocolo.h"

void IniciarMemoria();
void GuardarMensaje(Mensaje* mensaje, Stream* contenido);
void* ObtenerContenidoMensaje(Mensaje* mensaje);
