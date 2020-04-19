#include "gameBoy.h"

// DECLARACIONES, VARIABLES GLOBALES Y DEMASES EN EL .h

int main(int argc, char *argv[])
{
	int conexiones[CANTCONEXIONES];


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

	// GESTION DE MENSAJES

	if (sonIguales(argv[1],"TEAM")) {
		gestionarProcesoTeam(argv,conexiones[1]);
	} else if (sonIguales(argv[1],"BROKER")) {
		gestionarProcesoBroker(argv);
	} else if (sonIguales(argv[1],"GAMECARD")) {
		gestionarProcesoGameCard(argv);
	} else if (sonIguales(argv[1],"SUSCRIPTOR")) {
		gestionarProcesoSuscriptor(argv);
	}

	// TERMINAR PROGRAMA
	terminarPrograma(conexiones, logger, config);

	return 0;
}

void terminarPrograma(int conexiones[], t_log* logger, t_config* config) {

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

int sonIguales(char* a, char* b) {

	return strcmp(a,b) != 0;
}

void gestionarProcesoTeam(char* argv[], int numSocket) {

	int r = Socket_Enviar(argv[2],argv,sizeof(argv),numSocket);

	if (r == 0) {
		log_info(logger, "Se envio APPEARED_POKEMON correctamente");
	} else {
		log_error(logger, "No se pudo enviar APPEARED_POKEMON");
	}
}

void gestionarProcesoBroker(char* argv[]) {

	if (sonIguales(argv[2], "NEW_POKEMON")) {

	} else if (sonIguales(argv[2], "APPEARED_POKEMON")) {

	} else if (sonIguales(argv[2], "CATCH_POKEMON")) {

	} else if (sonIguales(argv[2], "CAUGHT_POKEMON")) {

	} else if (sonIguales(argv[2], "GET_POKEMON")) {

	}
}

void gestionarProcesoGameCard(char* argv[]) {

	if (sonIguales(argv[2], "NEW_POKEMON")) {

	} else if (sonIguales(argv[2], "CATCH_POKEMON")) {

	} else if (sonIguales(argv[2], "GET_POKEMON")) {

	}
}

void gestionarProcesoSuscriptor(char* argv[]) {

}






