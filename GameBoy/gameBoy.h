// INCLUDES Y DEFINES
#include <commons/log.h>
#include <commons/config.h>
#include <commons/collections/list.h>
#include <stdlib.h>
#include <string.h>
#include "../Utils/protocolo.h"
#include "../Utils/net.h"
#include "../Utils/socket.h"
#include "../Utils/eventos.h"

// DECLARACIONES
void terminarPrograma(t_log* logger, t_config* config);
void verificarConexion(int conexion, t_log* logger);
int sonIguales(char* a, char* b);
void gestionarAppeared(char* parametros[], int numSocket);
void gestionarNew(char* parametros[], int numSocket);
void gestionarGet(char* parametros[], int numSocket);
void gestionarCatch(char* parametros[], int numSocket);
void gestionarCaught(char* parametros[], int numSocket);




