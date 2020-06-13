#pragma once
#include "mensaje.h"
#include "../Utils/protocolo.h"

void IniciarMemoria(int tamanioMemoria, int tamanioMinimo, char* algoritmoMemoria, char* algoritmoReemplazo, char* algoritmoSeleccion, int frecuenciaCompactacion);
void GuardarMensaje(Mensaje* mensaje, CodigoDeCola tipoDeMensaje, Stream* contenido);
void* ObtenerContenidoMensaje(Mensaje* mensaje);
void Dump();
void DestruirMemoria();

extern void* memoria;
extern int tamanioMinimo;
