#include "gameBoy.h"

t_log* logger;

//TODO verificar que se recibieron la cantidad exacta de parametros y se recibieron bien

int main(int argc, char *argv[])
{
	logger = log_create("gameBoy.log", "GameBoy", true, LOG_LEVEL_INFO);
	log_info(logger,"Hola como estan");

	// CREAR CONFIG Y OBTENER VALORES (IP=PUERTO)
	t_config* config = config_create("gameBoy.config");
	char* ipBroker = config_get_string_value(config, "IP_BROKER");
	char* puertoBroker = config_get_string_value(config, "PUERTO_BROKER");
	char* ipTeam = config_get_string_value(config, "IP_TEAM");
	char* puertoTeam = config_get_string_value(config, "PUERTO_TEAM");
	char* ipGameCard = config_get_string_value(config, "IP_GAMECARD");
	char* puertoGameCard = config_get_string_value(config, "PUERTO_GAMECARD");

	//CREACION EVENTOS
	Eventos* eventos = Eventos_Crear(&ConectadoConProceso, &DesconectadoProceso, &ErrorMensaje);
	Eventos_AgregarOperacion(eventos, APPEARED_POKEMON, &ClienteOperacion_SUSCRIPTOR);


	// GESTION DE MENSAJES

	if (sonIguales(argv[1],"TEAM") && sonIguales(argv[2],"APPEARED_POKEMON")) {

		Cliente* clienteTeam = CrearCliente(ipTeam,puertoTeam,eventos);
		ConectadoConProceso("Team");

		send_APPEARED_POKEMON(argv,clienteTeam->socket);
		DestruirCliente(clienteTeam);

	} else if (sonIguales(argv[1],"BROKER")) {

		Cliente* clienteBroker = CrearCliente(ipBroker, puertoBroker, eventos); //CREO ACA DEBERIA CONECTARSE A LAS COLAS DEL BROKER
		ConectadoConProceso("Broker");

		if (sonIguales(argv[2], "NEW_POKEMON"))
			send_NEW_POKEMON(argv,clienteBroker->socket);
		  else if (sonIguales(argv[2], "APPEARED_POKEMON"))
			  send_APPEARED_POKEMON(argv,clienteBroker->socket);
		  else if (sonIguales(argv[2], "CATCH_POKEMON"))
			  send_CATCH_POKEMON(argv,clienteBroker->socket);
		  else if (sonIguales(argv[2], "CAUGHT_POKEMON"))
			  send_CAUGHT_POKEMON(argv,clienteBroker->socket);
		  else if (sonIguales(argv[2], "GET_POKEMON"))
			  send_GET_POKEMON(argv,clienteBroker->socket);

		DestruirCliente(clienteBroker);;

	} else if (sonIguales(argv[1],"GAMECARD")) {
		Cliente* clienteGameCard = CrearCliente(ipGameCard, puertoGameCard, eventos);
		ConectadoConProceso("GameCard");

		if (sonIguales(argv[2], "NEW_POKEMON"))
			send_NEW_POKEMON(argv,clienteGameCard->socket);
		  else if (sonIguales(argv[2], "CATCH_POKEMON"))
			  send_CATCH_POKEMON(argv,clienteGameCard->socket);
		  else if (sonIguales(argv[2], "GET_POKEMON"))
			  send_GET_POKEMON(argv,clienteGameCard->socket);

		DestruirCliente(clienteGameCard);
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

void send_NEW_POKEMON(char* parametros[], int numSocket) {

	DATOS_NEW_POKEMON* datos = malloc(sizeof(DATOS_NEW_POKEMON));

	datos->largoPokemon = (uint32_t) strlen(parametros[2]);
	datos->pokemon = parametros[2];
	(datos->posicion).posX = strtol(parametros[3],NULL,10);
	(datos->posicion).posY = strtol(parametros[4],NULL,10);
	datos->cantidad = strtol(parametros[5],NULL,10);

	int* tamanioBuffer = NULL;

	void* buffer = Serializar_NEW_POKEMON(datos, tamanioBuffer);
	int r = Socket_Enviar(NEW_POKEMON, buffer, *tamanioBuffer, numSocket);

	if (r == 0) {
		log_info(logger, "Se envio NEW_POKEMON correctamente");
	}
}

void send_APPEARED_POKEMON(char* parametros[], int numSocket) {

	DATOS_APPEARED_POKEMON* datos = malloc(sizeof(DATOS_APPEARED_POKEMON));

	datos->largoPokemon = (uint32_t) strlen(parametros[2]);
	datos->pokemon = parametros[2];
	(datos->posicion).posX = strtol(parametros[3],NULL,10);
	(datos->posicion).posY = strtol(parametros[4],NULL,10);
	datos->ID_MENSAJE = strtol(parametros[5],NULL,10);

	int* tamanioBuffer = NULL;

	void* buffer = Serializar_APPEARED_POKEMON(datos, tamanioBuffer);
	int r = Socket_Enviar(APPEARED_POKEMON, buffer, *tamanioBuffer, numSocket);

	if (r == 0) {
		log_info(logger, "Se envio APPEARED_POKEMON correctamente");
	}
}

void send_CATCH_POKEMON(char* parametros[], int numSocket) {

	DATOS_CATCH_POKEMON* datos = malloc(sizeof(DATOS_CATCH_POKEMON));

	datos->largoPokemon = (uint32_t) strlen(parametros[2]);
	datos->pokemon = parametros[2];
	(datos->posicion).posX = strtol(parametros[3],NULL,10);
	(datos->posicion).posY = strtol(parametros[4],NULL,10);

	int* tamanioBuffer = NULL;

	void* buffer = Serializar_CATCH_POKEMON(datos, tamanioBuffer);
	int r = Socket_Enviar(CATCH_POKEMON, buffer, *tamanioBuffer, numSocket);

	if (r == 0) {
		log_info(logger, "Se envio CATCH_POKEMON correctamente");
	}
}

void send_CAUGHT_POKEMON(char* parametros[], int numSocket) {

	DATOS_CAUGHT_POKEMON* datos = malloc(sizeof(DATOS_CAUGHT_POKEMON));

	datos->ID_MENSAJE = strtol(parametros[2],NULL,10);
	datos->capturado = strtol(parametros[3],NULL,10);

	int* tamanioBuffer = NULL;

	void* buffer = Serializar_CAUGHT_POKEMON(datos, tamanioBuffer);
	int r = Socket_Enviar(CAUGHT_POKEMON, buffer, *tamanioBuffer, numSocket);

	if (r == 0) {
		log_info(logger, "Se envio CAUGHT_POKEMON correctamente");
	}
}

void send_GET_POKEMON(char* parametros[], int numSocket) {

	DATOS_GET_POKEMON* datos = malloc(sizeof(DATOS_GET_POKEMON));

	datos->largoPokemon = (uint32_t) strlen(parametros[2]);
	datos->pokemon = parametros[2];

	int* tamanioBuffer = NULL;

	void* buffer = Serializar_GET_POKEMON(datos, tamanioBuffer);
	int r = Socket_Enviar(GET_POKEMON, buffer, *tamanioBuffer, numSocket);

	if (r == 0) {
		log_info(logger, "Se envio GET_POKEMON correctamente");
	}
}

void send_LOCALIZED_POKEMON(char* parametros[], int numSocket) {
	// TODO hacer funcion para mandar mensajes localized
}

void conectarseCola(void* colaMensaje, int tiempoDeterminado) {
	// TODO funcion para suscribirnos a una cola de mensajes y recbirlos por un tiempo determinado
}

void ConectadoConProceso(char* proceso) {
	log_info(logger, "Se conectó correctamente al proceso: %s",proceso);
}
void DesconectadoProceso(char* proceso) {
	log_info(logger, "Se desconectó correctamente al proceso: %s",proceso);
}

void ErrorMensaje(ErrorDeEscucha error, char* proceso, CodigoDeOperacion codigo) {
	if (error == 4)
		log_error(logger, "Error al enviar al proceso: %s, el mensaje: %d",proceso,codigo);
}

void ClienteOperacion_SUSCRIPTOR(Cliente* conexion, Paquete* paqueteRecibido) {
	log_info(logger, "Se suscribio a la cola tal y recibimos el mensaje tal", paqueteRecibido->stream);
}
