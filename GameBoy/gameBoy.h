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
void ConectadoConProceso(char*);
void DesconectadoProceso(char*);
void terminarPrograma(t_log* logger, t_config* config);

void verificarConexion(int conexion, t_log* logger);
bool sonIguales(char* a, char* b);
int cantidadParametros(char* parametros[]);

void send_NEW_POKEMON(int cantParametros, char* parametros[], int numSocket);
void send_APPEARED_POKEMON(int cantParametros, char* parametros[], int numSocket);
void send_CATCH_POKEMON(int cantParametros, char* parametros[], int numSocket);
void send_CAUGHT_POKEMON(int cantParametros, char* parametros[], int numSocket);
void send_GET_POKEMON(int cantParametros, char* parametros[], int numSocket);
void send_LOCALIZED_POKEMON(DATOS_LOCALIZED_POKEMON* datos, int numSocket);
void algoquenosenosocurre_SUSCRIPTOR(int cantParametros, char* parametros[], int numSocket);




