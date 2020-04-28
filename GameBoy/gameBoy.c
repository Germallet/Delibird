#include "gameBoy.h"

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
//void ClienteOperacion_MENSAJE(DatosConexion* conexion, Paquete* paqueteRecibido)
//{
//	log_info(logger, "ClienteOperacion_MENSAJE: %s", paqueteRecibido->stream);
//}
//void ClienteOperacion_NEW_POKEMON(DatosConexion* conexion, Paquete* paqueteRecibido)
//{
//	log_info(logger, "ClienteOperacion_NEW_POKEMON");
//}

int main(int argc, char *argv[])
{
	logger = log_create("gameBoy.log", "GameBoy", true, LOG_LEVEL_INFO);

	// CREAR CONFIG Y OBTENER VALORES (IP=PUERTO)
	t_config* config = config_create("gameBoy.config");
	char* ipBroker = config_get_string_value(config, "IP_BROKER");
	char* puertoBroker = config_get_string_value(config, "PUERTO_BROKER");
	char* ipTeam = config_get_string_value(config, "IP_TEAM");
	char* puertoTeam = config_get_string_value(config, "PUERTO_TEAM");
	char* ipGameCard = config_get_string_value(config, "IP_GAMECARD");
	char* puertoGameCard = config_get_string_value(config, "PUERTO_GAMECARD");

	//CREACION EVENTOS
	Eventos* eventos = Eventos_Crear(&ClienteConectado, &ClienteDesconectado, &ClienteError);
//	Eventos_AgregarOperacion(eventos, APPEARED_POKEMON, &ClienteOperacion_MENSAJE);
//	Eventos_AgregarOperacion(eventos, NEW_POKEMON, &ClienteOperacion_MENSAJE);
//	Eventos_AgregarOperacion(eventos, GET_POKEMON, &ClienteOperacion_MENSAJE);
//	Eventos_AgregarOperacion(eventos, CATCH_POKEMON, &ClienteOperacion_MENSAJE);
//	Eventos_AgregarOperacion(eventos, CAUGHT_POKEMON, &ClienteOperacion_MENSAJE);


	// GESTION DE MENSAJES

	if (sonIguales(argv[1],"TEAM") && sonIguales(argv[2],"APPEARED_POKEMON")) {
		int conexionTeam = Socket_Crear(ipTeam, puertoTeam, eventos);
		gestionarAppeared(argv,conexionTeam);
		Socket_LiberarConexion(conexionTeam);

	} else if (sonIguales(argv[1],"BROKER")) {
		int conexionBroker = Socket_Crear(ipBroker, puertoBroker, eventos);

		if (sonIguales(argv[2], "NEW_POKEMON"))
			gestionarNew(argv,conexionBroker);
		  else if (sonIguales(argv[2], "APPEARED_POKEMON"))
			gestionarAppeared(argv,conexionBroker);
		  else if (sonIguales(argv[2], "CATCH_POKEMON"))
	    	gestionarAppeared(argv,conexionBroker);
		  else if (sonIguales(argv[2], "CAUGHT_POKEMON"))
	    	gestionarCaught(argv,conexionBroker);
		  else if (sonIguales(argv[2], "GET_POKEMON"))
			gestionarAppeared(argv,conexionBroker);

		Socket_LiberarConexion(conexionBroker);

	} else if (sonIguales(argv[1],"GAMECARD")) {
		int conexionGameCard = Socket_CrearCliente(ipGameCard, puertoGameCard, eventos);

		if (sonIguales(argv[2], "NEW_POKEMON"))
			gestionarNew(argv,conexionGameCard);
		  else if (sonIguales(argv[2], "CATCH_POKEMON"))
			gestionarCatch(argv,conexionGameCard);
		  else if (sonIguales(argv[2], "GET_POKEMON"))
			gestionarGet(argv,conexionGameCard);

		Socket_LiberarConexion(conexionGameCard);
	}

	// TERMINAR PROGRAMA
	terminarPrograma(logger, config);

	return 0;
}

void terminarPrograma(t_log* logger, t_config* config) {

	log_destroy(logger);
	config_destroy(config);
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

void gestionarAppeared(char* parametros[], int numSocket) {

	DATOS_APPEARED_POKEMON* datos;
	datos->nombre = parametros[2];
	(datos->posicion).posX = atoi(parametros[3]);
	(datos->posicion).posY = atoi(parametros[4]);
	datos->ID_MENSAJE = atoi(parametros[5]);

	int* tamanioBuffer = NULL;

	void* buffer = Serializar_APPEARED_POKEMON(datos, tamanioBuffer);
	int r = Socket_Enviar(APPEARED_POKEMON, buffer, &tamanioBuffer, numSocket);

	if (r == 0) {
		log_info(logger, "Se envio APPEARED_POKEMON correctamente");
	}
}
/*
void gestionarNew(char* parametros[], int numSocket) {

	DATOS_NEW_POKEMON* datos;
	datos-> = parametros[2];
	datos.posicion.posX = atoi(parametros[3]);
	datos.posicion.posY = atoi(parametros[4]);
	datos.cantidad = atoi(parametros[5]);

	int* tamanioBuffer = NULL;

	void* buffer = Serializar_NEW_POKEMON(&datos, tamanioBuffer);
	int r = Socket_Enviar(NEW_POKEMON, buffer, tamanioBuffer, numSocket);

	if (r == 0) {
		log_info(logger, "Se envio APPEARED_POKEMON correctamente");
	}
*/
void gestionarGet(char* parametros[], int numSocket) {
/*
	DATOS_GET_POKEMON* datos;
	datos.nombre = atoi(parametros[2]);

	int* tamanioBuffer = NULL;

	void* buffer = Serializar_GET_POKEMON(&datos, tamanioBuffer);
	int r = Socket_Enviar(GET_POKEMON, buffer, tamanioBuffer, numSocket);

	if (r == 0) {
		log_info(logger, "Se envio APPEARED_POKEMON correctamente");
	}
*/
}

void gestionarCatch(char* parametros[], int numSocket) {
/*
	DATOS_CATCH_POKEMON* datos;
	datos.nombre = parametros[2];
	datos.posicion.posX = atoi(parametros[3]);
	datos.posicion.posY = atoi(parametros[4]);

	int* tamanioBuffer = NULL;

	void* buffer = Serializar_APPEARED_POKEMON(&datos, tamanioBuffer);
	int r = Socket_Enviar(APPEARED_POKEMON, buffer, tamanioBuffer, numSocket);

	if (r == 0) {
		log_info(logger, "Se envio APPEARED_POKEMON correctamente");
	}
*/
}

void gestionarCaught(char* parametros[], int numSocket) {
/*
	DATOS_CAUGHT_POKEMON* datos;
	datos.ID_MENSAJE = atoi(parametros[2]);
	datos.capturado = atoi(parametros[3]);

	int* tamanioBuffer = NULL;

	void* buffer = Serializar_CAUGHT_POKEMON(&datos, tamanioBuffer);
	int r = Socket_Enviar(CAUGHT_POKEMON, buffer, tamanioBuffer, numSocket);

	if (r == 0) {
		log_info(logger, "Se envio APPEARED_POKEMON correctamente");
	}
	*/
}


