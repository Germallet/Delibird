// INCLUDES Y DEFINES
#include <commons/log.h>
#include <commons/config.h>
#include <commons/collections/list.h>
#include "../Utils/socket.h"
#define CANTCONEXIONES 3


// DECLARACIONES
void terminarPrograma(int conexiones[], t_log* logger, t_config* config);
void verificarConexion(int conexion, t_log* logger);
int sonIguales(char* a, char* b);
void gestionarProcesoTeam(char* argv[], int conexion);
void gestionarProcesoBroker(char* argv[], int conexion);
void gestionarProcesoGameCard(char* argv[], int conexion);
void gestionarProcesoSuscriptor(char* argv[], int conexion);


// LOG INICIALIZADO
t_log* logger = log_create("gameBoy.log", "GameBoy", true, LOG_LEVEL_INFO);
log_info(logger, "Se inicio el proceso GameBoy!");





