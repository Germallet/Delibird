#include "gameBoy.h"

t_log* logger;

//TODO DECIDIR ENTRE VERIFICAR EN EL MAIN O EN CADA FUNCION Y AGREGAR COMO PARAMETRO OTRO.
int main(int argc, char* argv[])
{
	logger = log_create("gameBoy.log", "GameBoy", true, LOG_LEVEL_INFO);
	log_info(logger,"Te damos la bienvenida a mundo GameBoy!");

	// CREAR CONFIG Y OBTENER VALORES (IP=PUERTO)
	t_config* config = config_create("gameBoy.config");
	char* ipBroker = config_get_string_value(config, "IP_BROKER");
	int puertoBroker = config_get_int_value(config, "PUERTO_BROKER");
	char* ipTeam = config_get_string_value(config, "IP_TEAM");
	int puertoTeam = config_get_int_value(config, "PUERTO_TEAM");
	char* ipGameCard = config_get_string_value(config, "IP_GAMECARD");
	int puertoGameCard = config_get_int_value(config, "PUERTO_GAMECARD");

	// CREACION EVENTOS
	Eventos* eventos = Eventos_Crear0();

	// GESTION DE MENSAJES
	if (argc <= 3) {
		log_error(logger, "TE FALTAN PARAMETROS");
		exit(-1);
	}

	if (sonIguales(argv[1],"TEAM")) {

		Cliente* clienteTeam = CrearCliente(ipTeam,puertoTeam,eventos);

		if(clienteTeam == NULL) {
			log_error(logger, "NO SE PUDO CONECTAR AL TEAM");
			exit(-1);
		}

		ConectadoConProceso("TEAM");

		if (sonIguales(argv[2],"APPEARED_POKEMON")) {
			send_APPEARED_POKEMON(argv,clienteTeam->socket);
		} else {
			log_error(logger, "TEAM NO ENTIENDE TU OPERACION");
			exit (-1);
		}

		DestruirCliente(clienteTeam);

	} else if (sonIguales(argv[1],"BROKER")) {

		Cliente* clienteBroker = CrearCliente(ipBroker, puertoBroker, eventos); //CREO ACA DEBERIA CONECTARSE A LAS COLAS DEL BROKER

		if(clienteBroker == NULL) {
			log_error(logger, "NO SE PUDO CONECTAR AL MENSAJE");
			exit(-1);
		}

		ConectadoConProceso("BROKER");

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
		  else {
		  	log_error(logger, "BROKER NO ENTIENDE TU MENSAJE");
		  	exit (-1);
		  }

		DestruirCliente(clienteBroker);

	} else if (sonIguales(argv[1],"GAMECARD")) {

		Cliente* clienteGameCard = CrearCliente(ipGameCard, puertoGameCard, eventos);

		if(clienteGameCard == NULL) {
			log_error(logger, "NO SE PUDO CONECTAR A LA GAMECARD");
			exit(-1);
		}

		ConectadoConProceso("GAMECARD");

		if (sonIguales(argv[2], "NEW_POKEMON"))
			send_NEW_POKEMON(argv,clienteGameCard->socket);
		  else if (sonIguales(argv[2], "CATCH_POKEMON"))
			  send_CATCH_POKEMON(argv,clienteGameCard->socket);
		  else if (sonIguales(argv[2], "GET_POKEMON"))
			  send_GET_POKEMON(argv,clienteGameCard->socket);
		  else {
			  log_error(logger, "GAMECARD NO ENTIENDE TU MENSAJE");
			  exit (-1);
		  	}

		DestruirCliente(clienteGameCard);

	} else {
		log_error(logger, "TENEMOS LA SOSPECHA QUE ESCRIBISTE MAL EL PROCESO");
		exit (-1);
	}


	// TERMINAR PROGRAMA
	terminarPrograma(logger, config);

	return 0;
}

void terminarPrograma(t_log* logger, t_config* config) {

	log_destroy(logger);
	config_destroy(config);
}

bool sonIguales(char* a, char* b) {
	return strcmp(a,b) == 0;
}

void send_NEW_POKEMON(char* parametros[], int numSocket) {

	if(cantidadParametros(parametros) != 7) {
		log_error(logger, "Mandaste mal los parametros sabandija");
		exit(-1);
	}

	DATOS_NEW_POKEMON* datos = malloc(sizeof(DATOS_NEW_POKEMON));

	datos->largoPokemon = (uint32_t) strlen(parametros[2]);
	datos->pokemon = parametros[2];
	(datos->posicion).posX = strtol(parametros[3],NULL,10);
	(datos->posicion).posY = strtol(parametros[4],NULL,10);
	datos->cantidad = strtol(parametros[5],NULL,10);

	int tamanioBuffer;

	void* buffer = Serializar_NEW_POKEMON(datos, &tamanioBuffer);
	int r = Socket_Enviar(NEW_POKEMON, buffer, tamanioBuffer, numSocket);

	free(datos);

	if (r > 0)
	    log_info(logger, "Se envio NEW_POKEMON correctamente");
	else {
		log_error(logger, "No se envio correctamente el mensaje");
		exit(-1);
	}
}

void send_APPEARED_POKEMON(char* parametros[], int numSocket) {

	if(cantidadParametros(parametros) != 7) {
		log_error(logger, "Mandaste mal los parametros sabandija");
		exit(-1);
	}

	DATOS_APPEARED_POKEMON* datos = malloc(sizeof(DATOS_APPEARED_POKEMON));

	datos->largoPokemon = (uint32_t) strlen(parametros[2]);
	datos->pokemon = parametros[2];
	(datos->posicion).posX = strtol(parametros[3],NULL,10);
	(datos->posicion).posY = strtol(parametros[4],NULL,10);
	datos->ID_MENSAJE = strtol(parametros[5],NULL,10);

	int tamanioBuffer;

	void* buffer = Serializar_APPEARED_POKEMON(datos, &tamanioBuffer);
	int r = Socket_Enviar(APPEARED_POKEMON, buffer, tamanioBuffer, numSocket);

	free(datos);

	if (r > 0)
		log_info(logger, "Se envio APPEARED_POKEMON correctamente");
	else {
		log_error(logger, "No se envio correctamente el mensaje");
		exit(-1);
	}
}

void send_CATCH_POKEMON(char* parametros[], int numSocket) {

	if(cantidadParametros(parametros) != 6) {
		log_error(logger, "Mandaste mal los parametros sabandija");
		exit(-1);
	}

	DATOS_CATCH_POKEMON* datos = malloc(sizeof(DATOS_CATCH_POKEMON));


	datos->largoPokemon = (uint32_t) strlen(parametros[2]);
	datos->pokemon = parametros[2];
	(datos->posicion).posX = strtol(parametros[3],NULL,10);
	(datos->posicion).posY = strtol(parametros[4],NULL,10);

	int tamanioBuffer;

	void* buffer = Serializar_CATCH_POKEMON(datos, &tamanioBuffer);
	int r = Socket_Enviar(CATCH_POKEMON, buffer, tamanioBuffer, numSocket);

	free(datos);

	if (r > 0)
		log_info(logger, "Se envio CATCH_POKEMON correctamente");
	else {
		log_error(logger, "No se envio correctamente el mensaje");
		exit(-1);
	}
}

void send_CAUGHT_POKEMON(char* parametros[], int numSocket) {

	if(cantidadParametros(parametros) != 5) {
		log_error(logger, "Mandaste mal los parametros sabandija");
		exit(-1);
	}

	DATOS_CAUGHT_POKEMON* datos = malloc(sizeof(DATOS_CAUGHT_POKEMON));

	datos->ID_MENSAJE = strtol(parametros[2],NULL,10);
	datos->capturado = strtol(parametros[3],NULL,10);

	int tamanioBuffer;

	void* buffer = Serializar_CAUGHT_POKEMON(datos, &tamanioBuffer);
	int r = Socket_Enviar(CAUGHT_POKEMON, buffer, tamanioBuffer, numSocket);

	free(datos);

	if (r > 0)
		log_info(logger, "Se envio CAUGHT_POKEMON correctamente");
	else {
		log_error(logger, "No se envio correctamente el mensaje");
		exit(-1);
	}
}

void send_GET_POKEMON(char* parametros[], int numSocket) {

	if(cantidadParametros(parametros) != 4) {
		log_error(logger, "Mandaste mal los parametros sabandija");
		exit(-1);
	}

	DATOS_GET_POKEMON* datos = malloc(sizeof(DATOS_GET_POKEMON));

	datos->largoPokemon = (uint32_t) strlen(parametros[2]);
	datos->pokemon = parametros[2];

	int tamanioBuffer;

	void* buffer = Serializar_GET_POKEMON(datos, &tamanioBuffer);
	int r = Socket_Enviar(GET_POKEMON, buffer, tamanioBuffer, numSocket);

	free(datos);

	if (r > 0)
		log_info(logger, "Se envio GET_POKEMON correctamente");
	else {
		log_error(logger, "No se envio correctamente el mensaje");
		exit(-1);
	}
}

void send_LOCALIZED_POKEMON(char* parametros[], int numSocket) {
	// TODO hacer funcion para mandar mensajes localized
}

void conectarseCola(void* colaMensaje, int tiempoDeterminado) {
	// TODO funcion para suscribirnos a una cola de mensajes y recbirlos por un tiempo determinado
}

void ConectadoConProceso(char* proceso) {
	log_info(logger, "Se conectó correctamente al proceso %s",proceso);
}
void DesconectadoProceso(char* proceso) {
	log_info(logger, "Se desconectó correctamente al proceso %s",proceso);
}

int cantidadParametros(char* parametros[]) {
	int c = 0;

	while (parametros[c] != '\0')
		c++;

	return c;
}
