#include "gameBoy.h"

t_log* logger;

int main(int argc, char* argv[])
{
	logger = log_create("gameBoy.log", "GameBoy", true, LOG_LEVEL_INFO);
	log_info(logger,"Te damos la bienvenida a mundo GameBoy!");

	t_config* config = config_create("gameBoy.config");

	// CREACION EVENTOS
	Eventos* eventos = Eventos_Crear0();

	// GESTION DE MENSAJES
	if (argc <= 3) {
		log_error(logger, "TE FALTAN PARAMETROS");
		terminarPrograma(logger, config);
		exit(-1);
	}

	if (sonIguales(argv[1],"TEAM")) {
		char* ipTeam = config_get_string_value(config, "IP_TEAM");
		int puertoTeam = config_get_int_value(config, "PUERTO_TEAM");

		Cliente* clienteTeam = CrearCliente(ipTeam,puertoTeam,eventos);

		if(clienteTeam == NULL) {
			log_error(logger, "NO SE PUDO CONECTAR AL TEAM");
			exit(-1);
		}

		ConectadoConProceso("TEAM");

		if (sonIguales(argv[2],"APPEARED_POKEMON"))
			send_MESSAGE(APPEARED_POKEMON, convertir_APPEARED_POKEMON_ID(argc, argv), (void*) &Serializar_APPEARED_POKEMON_ID, clienteTeam->socket);
		  else {
			log_error(logger, "TEAM NO ENTIENDE TU OPERACION");
			terminarPrograma(logger, config);
			exit (-1);
		}

		DestruirCliente(clienteTeam);

	} else if (sonIguales(argv[1],"BROKER")) {

		char* ipBroker = config_get_string_value(config, "IP_BROKER");
		int puertoBroker = config_get_int_value(config, "PUERTO_BROKER");

		Cliente* clienteBroker = CrearCliente(ipBroker, puertoBroker, eventos);

		if(clienteBroker == NULL) {
			log_error(logger, "NO SE PUDO CONECTAR AL BROKER");
			terminarPrograma(logger, config);
			exit(-1);
		}

		ConectadoConProceso("BROKER");

		if (sonIguales(argv[2], "NEW_POKEMON"))
			send_MESSAGE(NEW_POKEMON, convertir_NEW_POKEMON(argc, argv), (void*) &Serializar_NEW_POKEMON, clienteBroker->socket);
		  else if (sonIguales(argv[2], "APPEARED_POKEMON"))
			send_MESSAGE(APPEARED_POKEMON, convertir_APPEARED_POKEMON_ID(argc, argv), (void*) &Serializar_APPEARED_POKEMON_ID, clienteBroker->socket);
		  else if (sonIguales(argv[2], "CATCH_POKEMON"))
			send_MESSAGE(CATCH_POKEMON, convertir_CATCH_POKEMON(argc, argv), (void*) &Serializar_CATCH_POKEMON, clienteBroker->socket);
		  else if (sonIguales(argv[2], "CAUGHT_POKEMON"))
			send_MESSAGE(CAUGHT_POKEMON, convertir_CAUGHT_POKEMON_ID(argc, argv), (void*) &Serializar_CAUGHT_POKEMON_ID, clienteBroker->socket);
		  else if (sonIguales(argv[2], "GET_POKEMON"))
			send_MESSAGE(GET_POKEMON, convertir_GET_POKEMON(argc, argv), (void*) &Serializar_GET_POKEMON, clienteBroker->socket);
		  else {
		  	log_error(logger, "BROKER NO ENTIENDE TU MENSAJE");
		  	exit (-1);
		  }

		DestruirCliente(clienteBroker);

	} else if (sonIguales(argv[1],"GAMECARD")) {

		char* ipGameCard = config_get_string_value(config, "IP_GAMECARD");
		int puertoGameCard = config_get_int_value(config, "PUERTO_GAMECARD");

		Cliente* clienteGameCard = CrearCliente(ipGameCard, puertoGameCard, eventos);

		if(clienteGameCard == NULL) {
			log_error(logger, "NO SE PUDO CONECTAR A LA GAMECARD");
			terminarPrograma(logger, config);
			exit(-1);
		}

		ConectadoConProceso("GAMECARD");

		if (sonIguales(argv[2], "NEW_POKEMON"))
			send_MESSAGE(NEW_POKEMON, convertir_NEW_POKEMON_ID(argc, argv), (void*) &Serializar_NEW_POKEMON_ID, clienteGameCard->socket);
		  else if (sonIguales(argv[2], "CATCH_POKEMON"))
			send_MESSAGE(CATCH_POKEMON, convertir_CATCH_POKEMON_ID(argc, argv), (void*) &Serializar_CATCH_POKEMON_ID, clienteGameCard->socket);
		  else if (sonIguales(argv[2], "GET_POKEMON"))
			send_MESSAGE(GET_POKEMON, convertir_GET_POKEMON(argc, argv), (void*) &Serializar_GET_POKEMON, clienteGameCard->socket);
		  else {
			  log_error(logger, "GAMECARD NO ENTIENDE TU MENSAJE");
			  exit (-1);
		  }

		DestruirCliente(clienteGameCard);

	} else if (sonIguales(argv[1],"SUSCRIPTOR")) {

		if (argc != 4) {
			log_error(logger,"No pusiste el tiempo sabandija");
			exit(-1);
		}

		char* ipBroker = config_get_string_value(config, "IP_BROKER");
		int puertoBroker = config_get_int_value(config, "PUERTO_BROKER");

		Eventos_AgregarOperacion(eventos, BROKER_CONECTADO, (EventoOperacion) &conexionBroker);

		Cliente* clienteBroker = CrearCliente(ipBroker, puertoBroker, eventos);

		if(clienteBroker == NULL) {
			log_error(logger, "NO SE PUDO CONECTAR AL BROKER");
			terminarPrograma(logger, config);
			exit(-1);
		}

		clienteBroker->info = (void*) convertirCodigo(argv[2]);

		if(clienteBroker->info == NULL) {
			log_error(logger,"Codigo invalido sabandija");
			exit(-1);
		}

		long tiempo = strtol(argv[3],NULL,10);

		if (Socket_Enviar(BROKER_CONECTAR, NULL, 0, clienteBroker->socket) < 0) {
			free(clienteBroker->info);
			log_error(logger,"No me pude conectar al broker");
			exit(-1);
		}

		log_error(logger,"Me conecte bien!!!");

		sleep(tiempo);

		DestruirCliente(clienteBroker);

	} else {
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

DATOS_NEW_POKEMON* convertir_NEW_POKEMON(int cantParametros, char*parametros[]) {

	if (cantParametros != 7){
		log_error(logger, "Mandaste mal los parametros sabandija");
		exit(-1);
	}

	DATOS_NEW_POKEMON* datos = malloc(strlen(parametros[3])+sizeof(uint32_t)*4);

	datos->largoPokemon = (uint32_t) strlen(parametros[3]);
	datos->pokemon = parametros[3];
	datos->posicion.posX = strtol(parametros[4],NULL,10);
	datos->posicion.posY = strtol(parametros[5],NULL,10);
	datos->cantidad = strtol(parametros[6],NULL,10);

	return datos;
}

DATOS_NEW_POKEMON_ID* convertir_NEW_POKEMON_ID(int cantParametros, char*parametros[]) {

	if (cantParametros != 8){
		log_error(logger, "Mandaste mal los parametros sabandija");
		exit(-1);
	}

	DATOS_NEW_POKEMON_ID* datosConID = malloc(sizeof(DATOS_NEW_POKEMON_ID));

	datosConID->datos = *convertir_NEW_POKEMON(cantParametros, parametros);

	datosConID->id = strtol(parametros[7],NULL,10);

	return datosConID;
}

DATOS_APPEARED_POKEMON_ID* convertir_APPEARED_POKEMON_ID(int cantParametros, char*parametros[]) {

	if (cantParametros != 7){
		log_error(logger, "Mandaste mal los parametros sabandija");
		exit(-1);
	}

	DATOS_APPEARED_POKEMON_ID* datos = malloc(sizeof(DATOS_APPEARED_POKEMON_ID));

	datos->datos.largoPokemon = (uint32_t) strlen(parametros[3]);
	datos->datos.pokemon = parametros[3];
	datos->datos.posicion.posX = strtol(parametros[4],NULL,10);
	datos->datos.posicion.posY = strtol(parametros[5],NULL,10);
	datos->id = strtol(parametros[6],NULL,10);

	return datos;
}

DATOS_CATCH_POKEMON* convertir_CATCH_POKEMON(int cantParametros, char*parametros[]) {

	if (cantParametros != 6){
		log_error(logger, "Mandaste mal los parametros sabandija");
		exit(-1);
	}

	DATOS_CATCH_POKEMON* datos = malloc(sizeof(DATOS_CATCH_POKEMON));

	datos->largoPokemon = (uint32_t) strlen(parametros[3]);
	datos->pokemon = parametros[3];
	datos->posicion.posX = strtol(parametros[4],NULL,10);
	datos->posicion.posY = strtol(parametros[5],NULL,10);

	return datos;
}

DATOS_CATCH_POKEMON_ID* convertir_CATCH_POKEMON_ID(int cantParametros, char*parametros[]) {

	if (cantParametros != 7){
		log_error(logger, "Mandaste mal los parametros sabandija");
		exit(-1);
	}

	DATOS_CATCH_POKEMON_ID* datosConID = malloc(sizeof(DATOS_CATCH_POKEMON_ID));
	datosConID->datos = *convertir_CATCH_POKEMON(cantParametros, parametros);

	datosConID->id = strtol(parametros[7],NULL,10);

	return datosConID;
}

DATOS_CAUGHT_POKEMON_ID* convertir_CAUGHT_POKEMON_ID(int cantParametros, char*parametros[]) {

	if (cantParametros != 5){
		log_error(logger, "Mandaste mal los parametros sabandija");
		exit(-1);
	}

	DATOS_CAUGHT_POKEMON_ID* datos = malloc(sizeof(DATOS_CAUGHT_POKEMON_ID));

	datos->id = strtol(parametros[3],NULL,10);
	datos->datos.capturado = strtol(parametros[4],NULL,10);

	return datos;
}

DATOS_GET_POKEMON* convertir_GET_POKEMON(int cantParametros, char*parametros[]) {

	if (cantParametros != 4){
		log_error(logger, "Mandaste mal los parametros sabandija");
		exit(-1);
	}

	DATOS_GET_POKEMON* datos = malloc(sizeof(DATOS_GET_POKEMON));

	datos->largoPokemon = (uint32_t) strlen(parametros[3]);
	datos->pokemon = parametros[3];

	return datos;
}

////./GameBoy BROKER NEW_POKEMON pikachu 3 1 3 ID()
////    1        2        3         4    5 6 7  8
////    0        1        2         3    4 5 6  7
//void send_NEW_POKEMON(DATOS_NEW_POKEMON datos, int numSocket) {
//
//	int tamanioBuffer;
//	void* buffer = Serializar_NEW_POKEMON(&datos, &tamanioBuffer);
//	if(Socket_Enviar(NEW_POKEMON, buffer, tamanioBuffer, numSocket) < 0) {
//		log_error(logger, "No se envio correctamente el mensaje");
//		exit(-1);
//	}
//
//	free(buffer);
//}
//
//void send_NEW_POKEMON_ID(DATOS_NEW_POKEMON_ID datos, int numSocket) {
//
//	int tamanioBuffer;
//	void* buffer = Serializar_NEW_POKEMON_ID(&datos, &tamanioBuffer);
//	if(Socket_Enviar(NEW_POKEMON, buffer, tamanioBuffer, numSocket) < 0) {
//		log_error(logger, "No se envio correctamente el mensaje");
//		exit(-1);
//	}
//
//	free(buffer);
//}
//
////./GameBoy BROKER APPEARED_POKEMON pikachu 3 1 ID()
////    1        2        3             4     5 6  7
////    0        1        2             3     4 5  6
//void send_APPEARED_POKEMON_ID(DATOS_APPEARED_POKEMON_ID datos, int numSocket) {
//
//	int tamanioBuffer;
//	void* buffer = Serializar_APPEARED_POKEMON_ID(&datos, &tamanioBuffer);
//
//	if(Socket_Enviar(APPEARED_POKEMON, buffer, tamanioBuffer, numSocket) < 0) {
//		log_error(logger, "No se envio correctamente el mensaje");
//		exit(-1);
//	}
//
//	free(buffer);
//}
//
////./GameBoy BROKER CATCH_POKEMON pikachu 3 1 ID()
////    1        2        3          4     5 6  7
////    0        1        2          3     4 5  6
//void send_CATCH_POKEMON(DATOS_CATCH_POKEMON datos, int numSocket) {
//
//	int tamanioBuffer;
//	void* buffer = Serializar_CATCH_POKEMON(&datos, &tamanioBuffer);
//
//	if(Socket_Enviar(CATCH_POKEMON, buffer, tamanioBuffer, numSocket) < 0) {
//		log_error(logger, "No se envio correctamente el mensaje");
//		exit(-1);
//	}
//
//	free(buffer);
//}
//
//void send_CATCH_POKEMON_ID(DATOS_CATCH_POKEMON_ID datos, int numSocket) {
//
//	int tamanioBuffer;
//	void* buffer = Serializar_CATCH_POKEMON_ID(&datos, &tamanioBuffer);
//
//	if(Socket_Enviar(CATCH_POKEMON, buffer, tamanioBuffer, numSocket) < 0) {
//		log_error(logger, "No se envio correctamente el mensaje");
//		exit(-1);
//	}
//
//	free(buffer);
//}
//
////./GameBoy BROKER CAUGHT_POKEMON bool ID
////    1        2        3          4    5
////    0        1        2          3    4
//void send_CAUGHT_POKEMON_ID(DATOS_CAUGHT_POKEMON_ID datos, int numSocket) {
//
//	int tamanioBuffer;
//
//	void* buffer = Serializar_CAUGHT_POKEMON_ID(&datos, &tamanioBuffer);
//
//	if(Socket_Enviar(CAUGHT_POKEMON, buffer, tamanioBuffer, numSocket) < 0) {
//		log_error(logger, "No se envio correctamente el mensaje");
//		exit(-1);
//	}
//
//	free(buffer);
//}
//
////./GameBoy BROKER GET_POKEMON pokemon
////    1        2        3         4
////    0        1        2         3
//void send_GET_POKEMON(DATOS_GET_POKEMON datos, int numSocket) {
//
//	int tamanioBuffer;
//
//	void* buffer = Serializar_GET_POKEMON(&datos, &tamanioBuffer);
//	if(Socket_Enviar(GET_POKEMON, buffer, tamanioBuffer, numSocket) < 0) {
//		log_error(logger, "No se envio correctamente el mensaje");
//		exit(-1);
//	}
//
//	free(buffer);
//}
//
//
////./GameBoy BROKER LOCALIZED_POKEMON pokemon cantidad coord*2     ALGUN ID POR ACA QUE NI IDEA
////    1        2          3            4       5        6 7 ..
////    0        1          2            3       4        5 6 ..
//void send_LOCALIZED_POKEMON(DATOS_LOCALIZED_POKEMON datos, int numSocket) {
//
//	int tamanioBuffer;
//
//	void* buffer = Serializar_LOCALIZED_POKEMON(datos, &tamanioBuffer);
//	if(Socket_Enviar(LOCALIZED_POKEMON, buffer, tamanioBuffer, numSocket) < 0) {
//		log_error(logger, "No se envio correctamente el mensaje");
//		exit(-1);
//	}
//
//	free(buffer);
//}

CodigoDeCola* convertirCodigo(char* codigo) {

	CodigoDeCola* code = malloc(sizeof(CodigoDeCola));

	if (sonIguales(codigo, "NEW_POKEMON"))
		*code = COLA_NEW_POKEMON;
	else if (sonIguales(codigo, "APPEARED_POKEMON"))
		*code = COLA_APPEARED_POKEMON;
	else if (sonIguales(codigo, "CATCH_POKEMON"))
		*code = COLA_CATCH_POKEMON;
	else if (sonIguales(codigo, "CAUGHT_POKEMON"))
		*code = COLA_CAUGHT_POKEMON;
	else if (sonIguales(codigo, "GET_POKEMON"))
		*code = COLA_GET_POKEMON;
	else if (sonIguales(codigo, "LOCALIZED_POKEMON"))
		*code = COLA_LOCALIZED_POKEMON;
	else {
		free(code);
		return NULL;
	}

	return code;
}

void conexionBroker(Cliente* cliente, Paquete* paquete) {

	BROKER_DATOS_SUSCRIBIRSE datos;

	datos.cola = *((CodigoDeCola*) (cliente->info));

	int tamanioBuffer;
	void* buffer = Serializar_BROKER_SUSCRIBIRSE(&datos, &tamanioBuffer);

	if(Socket_Enviar(BROKER_SUSCRIBIRSE, buffer, tamanioBuffer, cliente->socket) < 0)
		log_error(logger, "No se pudo conectar a la cola");

	free(buffer);
	free(cliente->info);

}

void ConectadoConProceso(char* proceso) {
	log_info(logger, "Se conectó correctamente al proceso %s",proceso);
}
void DesconectadoProceso(char* proceso) {
	log_info(logger, "Se desconectó correctamente al proceso %s",proceso);
}


void send_MESSAGE(CodigoDeOperacion codOp, void* datos, Serializar funcion, int numSocket) {

	int tamanioBuffer;
	void* buffer = funcion(&datos, &tamanioBuffer);

	if(Socket_Enviar(codOp, buffer, tamanioBuffer, numSocket) < 0) {
		printf("No se envio correctamente el mensaje");
		exit(-1);
	}

	free(buffer);
	free(datos);
}
