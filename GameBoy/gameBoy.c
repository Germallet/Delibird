#include "gameBoy.h"


int main()
{
	u_int32_t conexiones[3];

	t_log* logger = log_create("gameBoy.log", "GameBoy", true, LOG_LEVEL_INFO);
	log_info(logger, "GameBoy!");

	t_config* config = config_create("gameBoy.config");
	char* ipBroker = config_get_string_value(config,"IP_BROKER");
	char* puertoBroker = config_get_string_value(config,"PUERTO_BROKER");
	char* ipTeam = config_get_string_value(config,"IP_TEAM");
	char* puertoTeam = config_get_string_value(config,"PUERTO_TEAM");
	char* ipGameCard = config_get_string_value(config,"IP_GAMECARD");
	char* puertoGameCard = config_get_string_value(config,"PUERTO_GAMECARD");

	u_int32_t conexiones[0] = Socket_Crear(ipBroker,puertoBroker);
	u_int32_t conexiones[1] = Socket_Crear(ipTeam,puertoTeam);
	u_int32_t conexiones[2] = Socket_Crear(ipGameCard,puertoGameCard);

	terminarPrograma(conexiones,logger,config);

	return 0;
}


void terminarPrograma(u_int32_t conexiones[3], t_log* logger, t_config* config) {

	for (int i=0;i<3;i++) {
		Socket_LiberarConexion(conexiones[i]);
	}
	config_destroy(config);
	log_destroy(logger);
}
