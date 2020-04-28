// INCLUDES Y DEFINES
#include <commons/log.h>
#include <commons/config.h>
#include <commons/collections/list.h>
#include <stdlib.h>
#include "../Utils/protocolo.h"
#include "../Utils/net.h"
#include "../Utils/eventos.h"
#include "../Utils/socket.h"

// DECLARACIONES
void terminarPrograma(t_log* logger, t_config* config);
void verificarConexion(int conexion, t_log* logger);
int sonIguales(char* a, char* b);
void send_NEW_POKEMON(char* parametros[], int numSocket);
void send_APPEARED_POKEMON(char* parametros[], int numSocket);
void send_CATCH_POKEMON(char* parametros[], int numSocket);
void send_CAUGHT_POKEMON(char* parametros[], int numSocket);
void send_GET_POKEMON(char* parametros[], int numSocket);
void send_LOCALIZED_POKEMON(char* parametros[], int numSocket);
void algoquenosenosocurre_SUSCRIPTOR(char* parametros[], int numSocket);




