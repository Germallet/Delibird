#include "gameBoy.h"
#include "../Utils/protocolo.h"

t_log* logger;

void ClienteConectado()
{
	log_info(logger, "ClienteConectado");
}
void ClienteDesconectado()
{
	log_info(logger, "ClienteDesconectado");
}
void ClienteError(ErrorDeEscucha error, Paquete* paqueteRecibido)
{
	if (error == ERROR_RECIBIR)
		log_info(logger, "Error al recibir paquete. (Cod. op.: %d)", paqueteRecibido->codigoOperacion);
	else if (error == ERROR_OPERACION_INVALIDA)
		log_info(logger, "Recibido código de operación inválido. (Cod. op.: %d)", paqueteRecibido->codigoOperacion);
	else if (error == ERROR_PROCESAR_PAQUETE)
		log_info(logger, "Error al procesar paquete. (Cod. op.: %d)", paqueteRecibido->codigoOperacion);
}
void ClienteOperacion_MENSAJE(DatosConexion* conexion, Paquete* paqueteRecibido)
{
	log_info(logger, "ClienteOperacion_MENSAJE: %s", paqueteRecibido->stream);
}
void ClienteOperacion_NEW_POKEMON(DatosConexion* conexion, Paquete* paqueteRecibido)
{
	log_info(logger, "ClienteOperacion_NEW_POKEMON");
}

int main(int argc, char *argv[])
{
	logger = log_create("gameBoy.log", "GameBoy", true, LOG_LEVEL_INFO);

	int conexiones[CANTCONEXIONES];

	// CREAR CONFIG Y OBTENER VALORES (IP=PUERTO)
	t_config* config = config_create("gameBoy.config");
	char* ipBroker = config_get_string_value(config, "IP_BROKER");
	char* puertoBroker = config_get_string_value(config, "PUERTO_BROKER");
	char* ipTeam = config_get_string_value(config, "IP_TEAM");
	char* puertoTeam = config_get_string_value(config, "PUERTO_TEAM");
	char* ipGameCard = config_get_string_value(config, "IP_GAMECARD");
	char* puertoGameCard = config_get_string_value(config, "PUERTO_GAMECARD");

	Eventos* eventos = Eventos_Crear(&ClienteConectado, &ClienteDesconectado, &ClienteError);
	Eventos_AgregarOperacion(eventos, MENSAJE, &ClienteOperacion_MENSAJE);

	// CREAR CONEXIONES
	conexiones[0] = Socket_CrearCliente(ipBroker, puertoBroker, eventos);
	conexiones[1] = Socket_CrearCliente(ipTeam, puertoTeam, eventos);
	conexiones[2] = Socket_CrearCliente(ipGameCard, puertoGameCard, eventos);

	// VERIFICAR CONEXIONES
	for (int i = 0; i<CANTCONEXIONES; i++) {
		verificarConexion(conexiones[i], logger);
	}

	// GESTION DE MENSAJES

	/*if (sonIguales(argv[1],"TEAM")) {
		gestionarProcesoTeam(argv,conexiones[1]);
	} else if (sonIguales(argv[1],"BROKER")) {
		gestionarProcesoBroker(argv);
	} else if (sonIguales(argv[1],"GAMECARD")) {
		gestionarProcesoGameCard(argv);
	} else if (sonIguales(argv[1],"SUSCRIPTOR")) {
		gestionarProcesoSuscriptor(argv);
	}*/

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

/*int sonIguales(char* a, char* b) {

	return strcmp(a,b) != 0;
}

void gestionarProcesoTeam(char* parametros[], int numSocket) {

	char* p[3];

	uint32_t codigo = parametros[2];

	p[0] = parametros[3];
	p[1] = parametros[4];
	p[2] = parametros[5];

	DATOS_APPEARED_POKEMON datos;
	int* tamanioBuffer;

	void* buffer = Serializar_APPEARED_POKEMON(&datos, tamanioBuffer);
	int r = Socket_Enviar(APPEARED_POKEMON, buffer, tamanioBuffer, numSocket); //Debe haber mejor forma para hacer esto

	if (r == 0) {
		log_info(logger, "Se envio APPEARED_POKEMON correctamente");
	}
}

void gestionarProcesoBroker(char* parametros[], int conexion) {

	if (sonIguales(parametros[2], "NEW_POKEMON")) {

	} else if (sonIguales(parametros[2], "APPEARED_POKEMON")) {

	} else if (sonIguales(parametros[2], "CATCH_POKEMON")) {

	} else if (sonIguales(parametros[2], "CAUGHT_POKEMON")) {

	} else if (sonIguales(parametros[2], "GET_POKEMON")) {

	}
}

void gestionarProcesoGameCard(char* parametros[], int conexion) {

	if (sonIguales(parametros[2], "NEW_POKEMON")) {

	} else if (sonIguales(parametros[2], "CATCH_POKEMON")) {

	} else if (sonIguales(parametros[2], "GET_POKEMON")) {

	}
}

void gestionarProcesoSuscriptor(char* parametros[], int conexion) {

}


int tamanioParametrosAppeared(char* parametros[]) {
	int tamanio = 0;

	for (int i = 0; i < 3; i++) {
		tamanio += strlen(parametros[i]);
	}

	return tamanio;
}
*/


