#include "gameBoy.h"

t_log* logger;

//TODO DECIDIR ENTRE VERIFICAR EN EL MAIN O EN CADA FUNCION Y AGREGAR COMO PARAMETRO OTRO.
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
	Eventos* eventos = Eventos_Crear0();

	printf("Se recibieron %d parametros \n", argc);

	// GESTION DE MENSAJES

	if (argc <= 3) {
		log_error(logger, "Faltan parametros en el mensaje que busca enviar");
		exit(-1);
	}

	if (sonIguales(argv[1],"TEAM") && sonIguales(argv[2],"APPEARED_POKEMON")) {

		Cliente* clienteTeam = CrearCliente(ipTeam,puertoTeam,eventos);

		if(clienteTeam == NULL) {
			log_error(logger, "NO SE PUDO CONECTAR AL TEAM");
			exit(-1);
		}

		if (cantidadParametros(argv) > 2) {
			log_error(logger, "ERROR CANTIDAD DE PARAMETROS");
			exit(-1);
		}

		ConectadoConProceso("Team");

		send_APPEARED_POKEMON(argv,clienteTeam->socket);

		DestruirCliente(clienteTeam);

	} else if (sonIguales(argv[1],"BROKER")) {

		Cliente* clienteBroker = CrearCliente(ipBroker, puertoBroker, eventos); //CREO ACA DEBERIA CONECTARSE A LAS COLAS DEL BROKER

		if(clienteBroker == NULL) {
			log_error(logger, "NO SE PUDO CONECTAR AL BROKER");
			exit(-1);
		}

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

		DestruirCliente(clienteBroker);

	} else if (sonIguales(argv[1],"GAMECARD")) {

		Cliente* clienteGameCard = CrearCliente(ipGameCard, puertoGameCard, eventos);

		if(clienteGameCard == NULL) {
			log_error(logger, "NO SE PUDO CONECTAR A LA GAMECARD");
			exit(-1);
		}

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

//TODO Eliminar esta funcion
bool sonIguales(char* a, char* b) {
	return strcmp(a,b) == 0;
}

void send_NEW_POKEMON(char* parametros[], int numSocket) {

	if(cantidadParametros(parametros) != 7) {
		log_error(logger, "Faltan parametros");
		exit(-1);
	}

	DATOS_NEW_POKEMON* datos = malloc(sizeof(DATOS_NEW_POKEMON));

	datos->largoPokemon = (uint32_t) strlen(parametros[2]);
	datos->pokemon = parametros[2];
	(datos->posicion).posX = strtol(parametros[3],NULL,10);
	(datos->posicion).posY = strtol(parametros[4],NULL,10);
	datos->cantidad = strtol(parametros[5],NULL,10);

	int* tamanioBuffer = NULL;

	void* buffer = Serializar_NEW_POKEMON(datos, tamanioBuffer);
	int r = Socket_Enviar(NEW_POKEMON, buffer, *tamanioBuffer, numSocket);

	free(datos);

	if (r == 0)
	    log_info(logger, "Se envio NEW_POKEMON correctamente");
	else {
		log_error(logger, "No se envio correctamente el mensaje");
		exit(-1);
	}
}

void send_APPEARED_POKEMON(char* parametros[], int numSocket) {

	if(cantidadParametros(parametros) != 7) {
		log_error(logger, "Faltan parametros");
		exit(-1);
	}

	DATOS_APPEARED_POKEMON* datos = malloc(sizeof(DATOS_APPEARED_POKEMON));

	datos->largoPokemon = (uint32_t) strlen(parametros[2]);
	datos->pokemon = parametros[2];
	(datos->posicion).posX = strtol(parametros[3],NULL,10);
	(datos->posicion).posY = strtol(parametros[4],NULL,10);
	datos->ID_MENSAJE = strtol(parametros[5],NULL,10);

	int* tamanioBuffer = NULL;

	void* buffer = Serializar_APPEARED_POKEMON(datos, tamanioBuffer);
	int r = Socket_Enviar(APPEARED_POKEMON, buffer, *tamanioBuffer, numSocket);

	free(datos);

	if (r == 0)
		log_info(logger, "Se envio NEW_POKEMON correctamente");
	else {
		log_error(logger, "No se envio correctamente el mensaje");
		exit(-1);
	}
}

void send_CATCH_POKEMON(char* parametros[], int numSocket) {

	if(cantidadParametros(parametros) != 6) {
		log_error(logger, "Faltan parametros");
		exit(-1);
	}

	DATOS_CATCH_POKEMON* datos = malloc(sizeof(DATOS_CATCH_POKEMON));


	datos->largoPokemon = (uint32_t) strlen(parametros[2]);
	datos->pokemon = parametros[2];
	(datos->posicion).posX = strtol(parametros[3],NULL,10);
	(datos->posicion).posY = strtol(parametros[4],NULL,10);

	int* tamanioBuffer = NULL;

	void* buffer = Serializar_CATCH_POKEMON(datos, tamanioBuffer);
	int r = Socket_Enviar(CATCH_POKEMON, buffer, *tamanioBuffer, numSocket);

	free(datos);

	if (r == 0)
		log_info(logger, "Se envio NEW_POKEMON correctamente");
	else {
		log_error(logger, "No se envio correctamente el mensaje");
		exit(-1);
	}
}

void send_CAUGHT_POKEMON(char* parametros[], int numSocket) {

	if(cantidadParametros(parametros) != 5) {
		log_error(logger, "Faltan parametros");
		exit(-1);
	}

	DATOS_CAUGHT_POKEMON* datos = malloc(sizeof(DATOS_CAUGHT_POKEMON));

	datos->ID_MENSAJE = strtol(parametros[2],NULL,10);
	datos->capturado = strtol(parametros[3],NULL,10);

	int* tamanioBuffer = NULL;

	void* buffer = Serializar_CAUGHT_POKEMON(datos, tamanioBuffer);
	int r = Socket_Enviar(CAUGHT_POKEMON, buffer, *tamanioBuffer, numSocket);

	free(datos);

	if (r == 0)
		log_info(logger, "Se envio NEW_POKEMON correctamente");
	else {
		log_error(logger, "No se envio correctamente el mensaje");
		exit(-1);
	}
}

void send_GET_POKEMON(char* parametros[], int numSocket) {

	if(cantidadParametros(parametros) != 4) {
		log_error(logger, "Faltan parametros");
		exit(-1);
	}

	DATOS_GET_POKEMON* datos = malloc(sizeof(DATOS_GET_POKEMON));

	datos->largoPokemon = (uint32_t) strlen(parametros[2]);
	datos->pokemon = parametros[2];

	int* tamanioBuffer = NULL;

	void* buffer = Serializar_GET_POKEMON(datos, tamanioBuffer);
	int r = Socket_Enviar(GET_POKEMON, buffer, *tamanioBuffer, numSocket);

	free(datos);

	if (r == 0)
		log_info(logger, "Se envio NEW_POKEMON correctamente");
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
	log_info(logger, "Se conectó correctamente al proceso: %s",proceso);
}
void DesconectadoProceso(char* proceso) {
	log_info(logger, "Se desconectó correctamente al proceso: %s",proceso);
}

int cantidadParametros(char* parametros[]) { //TODO BORRAR NO ES NECESARIA PERO POR AHORA TIENE QUE ESTAR
	int c = 0;

	while (parametros[c] != '\0')
		c++;

	return c;
}
