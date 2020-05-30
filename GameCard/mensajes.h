#include <stdlib.h>
#include <stdbool.h>
#include "../Utils/socket.h"
#include "../Utils/protocolo.h"

void SocketEscucha();
void SuscribirseColas(Cliente* cliente);
void ConexionColas(Cliente* cliente, Paquete* paquete);
static void Recibir_NEW_POKEMON(Cliente* cliente, Paquete* paqueteRecibido);
static void Operacion_NEW_POKEMON(DATOS_NEW_POKEMON datos);
static void Recibir_CATCH_POKEMON(Cliente* cliente, Paquete* paqueteRecibido);
static void Operacion_CATCH_POKEMON(DATOS_CATCH_POKEMON datos);
static void Recibir_GET_POKEMON(Cliente* cliente, Paquete* paqueteRecibido);
static void Operacion_GET_POKEMON(DATOS_GET_POKEMON datos);
void EnviarID(Cliente* cliente, uint32_t identificador);
