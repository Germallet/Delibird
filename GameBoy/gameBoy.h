// INCLUDES Y DEFINES
#include <commons/log.h>
#include <commons/config.h>
#include <commons/collections/list.h>
#include <stdlib.h>
#include <unistd.h>
#include "../Utils/net.h"
#include "../Utils/socket.h"

//LOGGEAR
void ConectadoConProceso(char*);
void ConectadoConCola(char* cola);

//EVENTOS
void ConexionBroker(Cliente* cliente, Paquete* paquete);
void RecibirMensaje_NEW_POKEMON_ID(Cliente* cliente, Paquete* paqueteRecibido);
void RecibirMensaje_APPEARED_POKEMON_IDx2(Cliente* cliente, Paquete* paqueteRecibido);
void RecibirMensaje_CATCH_POKEMON_ID(Cliente* cliente, Paquete* paqueteRecibido);
void RecibirMensaje_CAUGHT_POKEMON_IDx2(Cliente* cliente, Paquete* paqueteRecibido);
void RecibirMensaje_GET_POKEMON_ID(Cliente* cliente, Paquete* paqueteRecibido);
void RecibirMensaje_LOCALIZED_POKEMON_IDx2(Cliente* cliente, Paquete* paqueteRecibido);
void loggearMensajesRecibidos(Eventos* eventos, CodigoDeCola cola);

//TERMINAR PROGRAMA
void TerminarPrograma(t_log* logger, t_config* config);
void TerminarProgramaConError(char* error);

//VERIFICAR DATOS
void verificarConexion(int conexion, t_log* logger);
bool sonIguales(char* a, char* b);
int cantidadParametros(char* parametros[]);

//ENVIAR ID
void EnviarID(Cliente* cliente, uint32_t identificador);

//CONVERTIR MENSAJES INGRESADOS POR CONSOLA
CodigoDeCola* convertirCodigo(char* codigo);

DATOS_NEW_POKEMON* convertir_NEW_POKEMON(int cantParametros, char* parametros[]);
DATOS_NEW_POKEMON_ID* convertir_NEW_POKEMON_ID(int cantParametros, char* parametros[]);
DATOS_APPEARED_POKEMON* convertir_APPEARED_POKEMON(int cantParametros, char* parametros[]);
DATOS_APPEARED_POKEMON_ID* convertir_APPEARED_POKEMON_ID(int cantParametros, char* parametros[]);
DATOS_CATCH_POKEMON* convertir_CATCH_POKEMON(int cantParametros, char* parametros[]);
DATOS_CATCH_POKEMON_ID* convertir_CATCH_POKEMON_ID(int cantParametros, char* parametros[]);
DATOS_CAUGHT_POKEMON_ID* convertir_CAUGHT_POKEMON_ID(int cantParametros, char* parametros[]);
DATOS_GET_POKEMON* convertir_GET_POKEMON(int cantParametros, char* parametros[]);
DATOS_GET_POKEMON_ID* convertir_GET_POKEMON_ID(int cantParametros, char* parametros[]);







