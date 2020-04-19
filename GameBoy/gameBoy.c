#include "gameBoy.h"

// DECLARACIONES Y DEMASES EN EL .h

int main(int argc, char *argv[])
{
	int conexiones[CANTCONEXIONES];

	// LOG DE PRUEBA
	t_log* logger = log_create("gameBoy.log", "GameBoy", true, LOG_LEVEL_INFO);
	log_info(logger, "GameBoy!");

	log_info(logger, "Parametros: %d", argc);
	for (int i = 0; i < argc; i++)
		log_info(logger, "Parametro %d: %s", i, argv[i]);

	// CREAR CONFIG Y OBTENER VALORES (IP=PUERTO)
	t_config* config = config_create("gameBoy.config");
	char* ipBroker = config_get_string_value(config, "IP_BROKER");
	char* puertoBroker = config_get_string_value(config, "PUERTO_BROKER");
	char* ipTeam = config_get_string_value(config, "IP_TEAM");
	char* puertoTeam = config_get_string_value(config, "PUERTO_TEAM");
	char* ipGameCard = config_get_string_value(config, "IP_GAMECARD");
	char* puertoGameCard = config_get_string_value(config, "PUERTO_GAMECARD");

	// CREAR CONEXIONES
	conexiones[0] = Socket_Crear(ipBroker, puertoBroker);
	conexiones[1] = Socket_Crear(ipTeam, puertoTeam);
	conexiones[2] = Socket_Crear(ipGameCard, puertoGameCard);

	// VERIFICAR CONEXIONES
	for (int i = 0; i<CANTCONEXIONES; i++) {
		verificarConexion(conexiones[i], logger);
	}

	// TERMINAR PROGRAMA
	terminarPrograma(conexiones, logger, config);

	return 0;
}

void terminarPrograma(int conexiones[], t_log* logger, t_config* config)
{
	log_destroy(logger);
	config_destroy(config);
	for (int i = 0; i<CANTCONEXIONES; i++) {
		Socket_LiberarConexion(conexiones[i]);
	}
}

void verificarConexion(int conexion, t_log* logger) {
	if (conexion == -1) {
			log_error(logger, "No se pudo crear la conexion");
		} else {
			log_info(logger, "Se conecto al proceso");
		}
}
