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
void gestionarProcesoTeam(char* argv[]);
void gestionarProcesoBroker(char* argv[]);
void gestionarProcesoGameCard(char* argv[]);
void gestionarProcesoSuscriptor(char* argv[]);


// LOG INICIALIZADO
t_log* logger = log_create("gameBoy.log", "GameBoy", true, LOG_LEVEL_INFO);
log_info(logger, "Se inicio el proceso GameBoy!");





