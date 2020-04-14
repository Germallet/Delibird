#include "gameBoy.h"

int main(void)
{
	int conexionBroker;
	int conexionTeam;
	int conexionGameCard;

	t_log* logger = log_create("gameBoy.log", "GameBoy", true, LOG_LEVEL_INFO);
	log_info(logger, "GameBoy!");

	t_config* config = config_create("gameBoy.config");
	char* ipBroker = config_get_string_value(config,"IP_BROKER");
	char* puertoBroker = config_get_string_value(config,"PUERTO_BROKER");
	char* ipTeam = config_get_string_value(config,"IP_TEAM");
	char* puertoTeam = config_get_string_value(config,"PUERTO_TEAM");
	char* ipGameCard = config_get_string_value(config,"IP_GAMECARD");
	char* puertoGameCard = config_get_string_value(config,"PUERTO_GAMECARD");

	conexionBroker = Socket_Crear(ipBroker,puertoBroker);
	if (conexionBroker == -1) {
		log_error(logger, "No se pudo crear la conexion");
		return 1;
	}
	log_info(logger,"Se conecto al proceso Broker");

	conexionTeam = Socket_Crear(ipTeam,puertoTeam);
	if (conexionTeam == -1) {
		log_error(logger, "No se pudo crear la conexion");
		return 1;
	}
	log_info(logger,"Se conecto al proceso Team");

	conexionGameCard = Socket_Crear(ipGameCard,puertoGameCard);
	if (conexionGameCard == -1) {
		log_error(logger, "No se pudo crear la conexion");
		return 1;
	}
	log_info(logger,"Se conecto al proceso GameCard");

	Socket_LiberarConexion(conexionBroker);
	Socket_LiberarConexion(conexionTeam);
	Socket_LiberarConexion(conexionGameCard);
	config_destroy(config);
	log_destroy(logger);

	return 0;
}
