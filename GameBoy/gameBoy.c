#include "gameBoy.h"

t_log* logger;

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
		terminarPrograma(logger, config);
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
			send_APPEARED_POKEMON(argc, argv, clienteTeam->socket);
		} else {
			log_error(logger, "TEAM NO ENTIENDE TU OPERACION");
			terminarPrograma(logger, config);
			exit (-1);
		}

		DestruirCliente(clienteTeam);

	} else if (sonIguales(argv[1],"BROKER")) {

		Cliente* clienteBroker = CrearCliente(ipBroker, puertoBroker, eventos); //CREO ACA DEBERIA CONECTARSE A LAS COLAS DEL BROKER

		if(clienteBroker == NULL) {
			log_error(logger, "NO SE PUDO CONECTAR AL BROKER");
			terminarPrograma(logger, config);
			exit(-1);
		}

		ConectadoConProceso("BROKER");

		if (sonIguales(argv[2], "NEW_POKEMON"))
			send_NEW_POKEMON(argc, argv, clienteBroker->socket);
		  else if (sonIguales(argv[2], "APPEARED_POKEMON"))
			send_APPEARED_POKEMON(argc, argv, clienteBroker->socket);
		  else if (sonIguales(argv[2], "CATCH_POKEMON"))
			send_CATCH_POKEMON(argc, argv, clienteBroker->socket);
		  else if (sonIguales(argv[2], "CAUGHT_POKEMON"))
			send_CAUGHT_POKEMON(argc, argv, clienteBroker->socket);
		  else if (sonIguales(argv[2], "GET_POKEMON"))
			send_GET_POKEMON(argc, argv, clienteBroker->socket);
		  else {
		  	log_error(logger, "BROKER NO ENTIENDE TU MENSAJE");
		  	exit (-1);
		  }

		DestruirCliente(clienteBroker);

	} else if (sonIguales(argv[1],"GAMECARD")) {

		Cliente* clienteGameCard = CrearCliente(ipGameCard, puertoGameCard, eventos);

		if(clienteGameCard == NULL) {
			log_error(logger, "NO SE PUDO CONECTAR A LA GAMECARD");
			terminarPrograma(logger, config);
			exit(-1);
		}

		ConectadoConProceso("GAMECARD");

		if (sonIguales(argv[2], "NEW_POKEMON"))
			send_NEW_POKEMON(argc, argv, clienteGameCard->socket);
		  else if (sonIguales(argv[2], "CATCH_POKEMON"))
			  send_CATCH_POKEMON(argc, argv, clienteGameCard->socket);
		  else if (sonIguales(argv[2], "GET_POKEMON"))
			  send_GET_POKEMON(argc, argv, clienteGameCard->socket);
		  else {
			  log_error(logger, "GAMECARD NO ENTIENDE TU MENSAJE");
			  exit (-1);
		  	}

		DestruirCliente(clienteGameCard);

	} else  {
		log_error(logger, "TENEMOS LA SOSPECHA QUE ESCRIBISTE MAL EL PROCESO");
		terminarPrograma(logger, config);
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

//void* convertirANEW_POKEMON(int cantParametros, char*parametros[]) {
//
//	if (cantParametros != 7 && cantParametros != 8){
//		log_error(logger, "Mandaste mal los parametros sabandija");
//		exit(-1);
//	}
//
//	DATOS_NEW_POKEMON* datos = malloc(sizeof(DATOS_NEW_POKEMON));
//
//	datos->largoPokemon = (uint32_t) strlen(parametros[3]);
//	datos->pokemon = parametros[3];
//	(datos->posicion).posX = strtol(parametros[4],NULL,10);
//	(datos->posicion).posY = strtol(parametros[5],NULL,10);
//	datos->cantidad = strtol(parametros[6],NULL,10);
//
//	if (cantParametros == 8) {
//		DATOS_MENSAJE* datosMensaje = malloc(sizeof(DATOS_MENSAJE));
//		uint32_t ID = strtol(parametros[7],NULL,10);
//		return datosMensaje;
//	} else
//		return datos;
//}

//./GameBoy BROKER NEW_POKEMON pikachu 3 1 3 ID()
//    1        2        3         4    5 6 7  8
//    0        1        2         3    4 5 6  7
void send_NEW_POKEMON(int cantParametros, char* parametros[], int numSocket) {

	if (cantParametros != 7 && cantParametros != 8){
	    log_error(logger, "Mandaste mal los parametros sabandija");
	    exit(-1);
	}

	DATOS_NEW_POKEMON* datos = malloc(sizeof(DATOS_NEW_POKEMON));

	datos->largoPokemon = (uint32_t) strlen(parametros[3]);
	datos->pokemon = parametros[3];
	(datos->posicion).posX = strtol(parametros[4],NULL,10);
	(datos->posicion).posY = strtol(parametros[5],NULL,10);
	datos->cantidad = strtol(parametros[6],NULL,10);

	int tamanioBuffer;
	void* buffer = Serializar_NEW_POKEMON(datos, &tamanioBuffer);

	if (cantParametros == 8) {
		uint32_t ID = strtol(parametros[7],NULL,10);
		buffer = Serializar_ID_MENSAJE(&ID, buffer, &tamanioBuffer);
	}

	int r = Socket_Enviar(NEW_POKEMON, buffer, tamanioBuffer, numSocket);

	if (r > 0)
	    log_info(logger, "Se envio NEW_POKEMON correctamente");
	else {
		log_error(logger, "No se envio correctamente el mensaje");
		exit(-1);
	}

	free(buffer);
	free(datos);
}

//./GameBoy BROKER APPEARED_POKEMON pikachu 3 1 ID()
//    1        2        3             4     5 6  7
//    0        1        2             3     4 5  6
void send_APPEARED_POKEMON(int cantParametros, char* parametros[], int numSocket) {

	if(cantParametros != 7) {
		log_error(logger, "Mandaste mal los parametros sabandija");
		exit(-1);
	}

	DATOS_APPEARED_POKEMON* datos = malloc(sizeof(DATOS_APPEARED_POKEMON));

	datos->largoPokemon = (uint32_t) strlen(parametros[3]);
	datos->pokemon = parametros[3];
	(datos->posicion).posX = strtol(parametros[4],NULL,10);
	(datos->posicion).posY = strtol(parametros[5],NULL,10);

	uint32_t ID = strtol(parametros[6],NULL,10);

	int tamanioBuffer;

	void* buffer = Serializar_APPEARED_POKEMON(datos, &tamanioBuffer);
	buffer = Serializar_ID_MENSAJE(&ID, buffer, &tamanioBuffer);

	int r = Socket_Enviar(APPEARED_POKEMON, buffer, tamanioBuffer, numSocket);

	free(buffer);
	free(datos);

	if (r > 0)
		log_info(logger, "Se envio APPEARED_POKEMON correctamente");
	else {
		log_error(logger, "No se envio correctamente el mensaje");
		exit(-1);
	}
}

//./GameBoy BROKER CATCH_POKEMON pikachu 3 1 ID()
//    1        2        3          4     5 6  7
//    0        1        2          3     4 5  6
void send_CATCH_POKEMON(int cantParametros, char* parametros[], int numSocket) {

	if(cantParametros != 6 && cantParametros != 7) {
		log_error(logger, "Mandaste mal los parametros sabandija");
		exit(-1);
	}

	DATOS_CATCH_POKEMON* datos = malloc(sizeof(DATOS_CATCH_POKEMON));

	datos->largoPokemon = (uint32_t) strlen(parametros[3]);
	datos->pokemon = parametros[3];
	(datos->posicion).posX = strtol(parametros[4],NULL,10);
	(datos->posicion).posY = strtol(parametros[5],NULL,10);

	int tamanioBuffer;

	void* buffer = Serializar_CATCH_POKEMON(datos, &tamanioBuffer);

	if (cantParametros == 7) {
		uint32_t ID = strtol(parametros[6],NULL,10);
		buffer = Serializar_ID_MENSAJE(&ID,buffer,&tamanioBuffer);
	}

	int r = Socket_Enviar(CATCH_POKEMON, buffer, tamanioBuffer, numSocket);

	free(buffer);
	free(datos);

	if (r > 0)
		log_info(logger, "Se envio CATCH_POKEMON correctamente");
	else {
		log_error(logger, "No se envio correctamente el mensaje");
		exit(-1);
	}
}

//./GameBoy BROKER CAUGHT_POKEMON bool ID
//    1        2        3          4    5
//    0        1        2          3    4
void send_CAUGHT_POKEMON(int cantParametros, char* parametros[], int numSocket) {

	if(cantParametros != 5) {
		log_error(logger, "Mandaste mal los parametros sabandija");
		exit(-1);
	}

	DATOS_CAUGHT_POKEMON* datos = malloc(sizeof(DATOS_CAUGHT_POKEMON));

	uint32_t ID = strtol(parametros[3],NULL,10);
	datos->capturado = strtol(parametros[4],NULL,10);

	int tamanioBuffer;

	void* buffer = Serializar_CAUGHT_POKEMON(datos, &tamanioBuffer);
	buffer = Serializar_ID_MENSAJE(&ID, buffer, &tamanioBuffer);
	int r = Socket_Enviar(CAUGHT_POKEMON, buffer, tamanioBuffer, numSocket);

	free(buffer);
	free(datos);

	if (r > 0)
		log_info(logger, "Se envio CAUGHT_POKEMON correctamente");
	else {
		log_error(logger, "No se envio correctamente el mensaje");
		exit(-1);
	}
}

//./GameBoy BROKER GET_POKEMON pokemon
//    1        2        3         4
//    0        1        2         3
void send_GET_POKEMON(int cantParametros, char* parametros[], int numSocket) {

	if(cantParametros != 4) {
		log_error(logger, "Mandaste mal los parametros sabandija");
		exit(-1);
	}

	DATOS_GET_POKEMON* datos = malloc(sizeof(DATOS_GET_POKEMON));

	datos->largoPokemon = (uint32_t) strlen(parametros[3]);
	datos->pokemon = parametros[3];

	int tamanioBuffer;

	void* buffer = Serializar_GET_POKEMON(datos, &tamanioBuffer);
	int r = Socket_Enviar(GET_POKEMON, buffer, tamanioBuffer, numSocket);

	free(buffer);
	free(datos);

	if (r > 0)
		log_info(logger, "Se envio GET_POKEMON correctamente");
	else {
		log_error(logger, "No se envio correctamente el mensaje");
		exit(-1);
	}
}

//./GameBoy BROKER LOCALIZED_POKEMON pokemon cantidad coord*2     ALGUN ID POR ACA QUE NI IDEA
//    1        2          3            4       5        6 7 ..
//    0        1          2            3       4        5 6 ..
void send_LOCALIZED_POKEMON(DATOS_LOCALIZED_POKEMON* datos, int numSocket) {

//	uint32_t cantPares = strtol(parametros[4], NULL, 10);
//
//	if(cantParametros != 5 + cantPares*2) {
//		log_error(logger, "Mandaste mal los parametros sabandija");
//		exit(-1);
//	}
//
//	DATOS_LOCALIZED_POKEMON* datos = malloc(sizeof(DATOS_LOCALIZED_POKEMON));
//
//	datos->largoPokemon = (uint32_t) strlen(parametros[3]);
//	datos->pokemon = parametros[3];
//	datos->cantidad = cantPares;
//
//	for (int i = 0; i < cantPares; i++) {
//		(datos->posiciones[i]).posX = strtol(parametros[i + 5], NULL, 10);
//		(datos->posiciones[i]).posY = strtol(parametros[i + 6], NULL, 10);
//	}
// TODO ver si hay que dejar esto como estaba o sacar al pingo lo comentado porque ya vamos a recibir un paquete

	int tamanioBuffer;

	void* buffer = Serializar_LOCALIZED_POKEMON(datos, &tamanioBuffer);
	int r = Socket_Enviar(LOCALIZED_POKEMON, buffer, tamanioBuffer, numSocket);

//	free(datos);

	// TODO el tema de la ID faltaria agregar en localized

	if (r > 0)
		log_info(logger, "Se envio LOCALIZED_POKEMON correctamente");
	else {
		log_error(logger, "No se envio correctamente el mensaje");
		exit(-1);
	}

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
