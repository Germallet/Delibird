#pragma once
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include "../Utils/socket.h"
#include "../Utils/serializacion.h"
#include "archivos.h"

void SocketEscucha();
void SuscribirseColas(Cliente* cliente);
void ConexionColas(Cliente* cliente, Paquete* paquete);

void Recibir_NEW_POKEMON(Cliente* cliente, Paquete* paqueteRecibido);
void Operacion_NEW_POKEMON(DATOS_NEW_POKEMON_ID* datos);

void Recibir_CATCH_POKEMON(Cliente* cliente, Paquete* paqueteRecibido);
void Operacion_CATCH_POKEMON(DATOS_CATCH_POKEMON* datos);

void Recibir_GET_POKEMON(Cliente* cliente, Paquete* paqueteRecibido);
void Operacion_GET_POKEMON(DATOS_GET_POKEMON* datos);

void EnviarID(Cliente* cliente, uint32_t identificador);

void Enviar_APPEARED_POKEMON(DATOS_NEW_POKEMON_ID* datos);
void Enviar_CAUGHT_POKEMON(/*PARAMETROS*/);
void Enviar_LOCALIZED_POKEMON(/*PARAMETROS*/);

void Recibir_ID(Cliente* cliente, Paquete* paqueteRecibido);
