#pragma once
#include "mensaje.h"
#include "../Utils/protocolo.h"

void IniciarMemoria(int tamanioMemoria, char* algoritmoMemoria);
void GuardarMensaje(Mensaje* mensaje, Stream* contenido);
void* ObtenerContenidoMensaje(Mensaje* mensaje);
void Dump();
void DestruirMemoria();

extern t_list* particiones;
