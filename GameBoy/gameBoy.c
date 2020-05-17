#include "gameBoy.h"
#include "../Utils/protocolo.h"
#include "../Utils/serializacion.h"

t_log* logger;
t_config* config;

//todo ver como achicar un toque el main y delegar en otros archivos algunas funciones

int main(int argc, char* argv[])
{
	logger = log_create("gameBoy.log", "GameBoy", true, LOG_LEVEL_INFO);
	log_info(logger, "Te damos la bienvenida a mundo GameBoy!");

	config = config_create("gameBoy.config");

	// CREACION EVENTOS
	Eventos* eventos = Eventos_Crear0();

	// GESTION DE MENSAJES
	if (argc <= 3) TerminarProgramaConError("QUE QUERES QUE HAGA SI NO ME PONES LOS PARAMETROS?");

	if (sonIguales(argv[1], "TEAM")) {

		char* ipTeam = config_get_string_value(config, "IP_TEAM");
		int puertoTeam = config_get_int_value(config, "PUERTO_TEAM");

		Cliente* clienteTeam = CrearCliente(ipTeam, puertoTeam, eventos);

		if (clienteTeam == NULL) TerminarProgramaConError("NO ME PUDE CONECTAR AL TEAM");

		ConectadoConProceso("TEAM");

		if (sonIguales(argv[2],"APPEARED_POKEMON"))
			EnviarMensaje(clienteTeam, APPEARED_POKEMON, convertir_APPEARED_POKEMON(argc, argv), (void*) &SerializarM_APPEARED_POKEMON);
		else TerminarProgramaConError("TEAM NO ENTIENDE TU OPERACION");

		DestruirCliente(clienteTeam);

	} else if (sonIguales(argv[1],"BROKER")) {

		char* ipBroker = config_get_string_value(config, "IP_BROKER");
		int puertoBroker = config_get_int_value(config, "PUERTO_BROKER");

		Cliente* clienteBroker = CrearCliente(ipBroker, puertoBroker, eventos);

		if (clienteBroker == NULL) TerminarProgramaConError("NO ME PUDE CONECTAR AL BROKER");

		ConectadoConProceso("BROKER");

		if (sonIguales(argv[2], "NEW_POKEMON"))
			EnviarMensaje(clienteBroker, NEW_POKEMON, convertir_NEW_POKEMON(argc, argv), (void*) &SerializarM_NEW_POKEMON);
		  else if (sonIguales(argv[2], "APPEARED_POKEMON"))
			  EnviarMensaje(clienteBroker, APPEARED_POKEMON, convertir_APPEARED_POKEMON_ID(argc, argv), (void*) &SerializarM_APPEARED_POKEMON_ID);
		  else if (sonIguales(argv[2], "CATCH_POKEMON"))
			  EnviarMensaje(clienteBroker, CATCH_POKEMON, convertir_CATCH_POKEMON(argc, argv), (void*) &SerializarM_CATCH_POKEMON);
		  else if (sonIguales(argv[2], "CAUGHT_POKEMON"))
			  EnviarMensaje(clienteBroker, CAUGHT_POKEMON, convertir_CAUGHT_POKEMON_ID(argc, argv), (void*) &SerializarM_CAUGHT_POKEMON_ID);
		  else if (sonIguales(argv[2], "GET_POKEMON"))
			  EnviarMensaje(clienteBroker, GET_POKEMON, convertir_GET_POKEMON(argc, argv), (void*) &SerializarM_GET_POKEMON);
		  else TerminarProgramaConError("BROKER NO ENTIENDE TU OPERACION");

		DestruirCliente(clienteBroker);

	} else if (sonIguales(argv[1], "GAMECARD")) {

		char* ipGameCard = config_get_string_value(config, "IP_GAMECARD");
		int puertoGameCard = config_get_int_value(config, "PUERTO_GAMECARD");

		Cliente* clienteGameCard = CrearCliente(ipGameCard, puertoGameCard, eventos);

		if (clienteGameCard == NULL) TerminarProgramaConError("NO ME PUDE CONECTAR A LA GAMECARD");

		ConectadoConProceso("GAMECARD");

		if (sonIguales(argv[2], "NEW_POKEMON"))
			EnviarMensaje(clienteGameCard, NEW_POKEMON, convertir_NEW_POKEMON_ID(argc, argv), (void*) &SerializarM_NEW_POKEMON_ID);
		  else if (sonIguales(argv[2], "CATCH_POKEMON"))
			  EnviarMensaje(clienteGameCard, CATCH_POKEMON, convertir_CATCH_POKEMON_ID(argc, argv), (void*) &SerializarM_CATCH_POKEMON_ID);
		  else if (sonIguales(argv[2], "GET_POKEMON"))
			  EnviarMensaje(clienteGameCard, GET_POKEMON, convertir_GET_POKEMON_ID(argc, argv), (void*) &SerializarM_GET_POKEMON_ID);
		  else TerminarProgramaConError("GAMECARD NO ENTIENDE TU OPERACION");

		DestruirCliente(clienteGameCard);

	} else if (sonIguales(argv[1], "SUSCRIPTOR")) {

		char* ipBroker = config_get_string_value(config, "IP_BROKER");
		int puertoBroker = config_get_int_value(config, "PUERTO_BROKER");

		CodigoDeCola* cola = convertirCodigo(argv[2]);

		Eventos_AgregarOperacion(eventos, BROKER_CONECTADO, (EventoOperacion) &ConexionBroker);
		loggearMensajesRecibidos(eventos, *cola);
		Cliente* clienteBroker = CrearCliente(ipBroker, puertoBroker, eventos);

		if (clienteBroker == NULL) TerminarProgramaConError("NO ME PUDE CONECTAR AL BROKER");
		clienteBroker->info = cola;

		if (clienteBroker->info == NULL) TerminarProgramaConError("ESCRIBIME BIEN LA OPERACION, DALE");

		if (argc != 4) TerminarProgramaConError("MANDAME BIEN LOS PARAMETROS, SABANDIJA");

		long tiempo = strtol(argv[3], NULL, 10);

		if (Socket_Enviar(BROKER_CONECTAR, NULL, 0, clienteBroker->socket) < 0) {
			free(clienteBroker->info);
			TerminarProgramaConError("NO LE PUDE AVISAR AL BROKER QUE ME QUIERO CONECTAR");
		}

		ConectadoConCola(argv[2]);

		sleep(tiempo);

		DestruirCliente(clienteBroker);

	} else TerminarProgramaConError("TENGO LA SOSPECHA QUE ESCRIBISTE MAL EL PROCESO");

	// TERMINAR PROGRAMA
	TerminarPrograma(logger, config);

	return 0;
}

void TerminarProgramaConError(char* error)
{
		log_error(logger, error);
		TerminarPrograma(logger, config);
		exit(-1);
}

void TerminarPrograma(t_log* logger, t_config* config)
{
	log_destroy(logger);
	config_destroy(config);
}

bool sonIguales(char* a, char* b)
{
	return strcmp(a, b) == 0;
}

//./GameBoy BROKER NEW_POKEMON pikachu 3 1 3
//    1        2        3         4    5 6 7
//    0        1        2         3    4 5 6
DATOS_NEW_POKEMON* convertir_NEW_POKEMON(int cantParametros, char* parametros[])
{
	if (cantParametros != 7) TerminarProgramaConError("MANDAME BIEN LOS PARAMETROS, SABANDIJA");

	DATOS_NEW_POKEMON* datos = malloc(sizeof(DATOS_NEW_POKEMON));

	//datos->largoPokemon = (uint32_t) strlen(parametros[3]);
	datos->pokemon = parametros[3];
	datos->posicion.posX = strtol(parametros[4], NULL, 10);
	datos->posicion.posY = strtol(parametros[5], NULL, 10);
	datos->cantidad = strtol(parametros[6], NULL, 10);

	return datos;
}

//./GameBoy BROKER NEW_POKEMON pikachu 3 1 3 ID
//    1        2        3         4    5 6 7  8
//    0        1        2         3    4 5 6  7
DATOS_NEW_POKEMON_ID* convertir_NEW_POKEMON_ID(int cantParametros, char* parametros[])
{
	if (cantParametros != 8) TerminarProgramaConError("MANDAME BIEN LOS PARAMETROS, SABANDIJA");

	DATOS_NEW_POKEMON_ID* datos = malloc(sizeof(DATOS_NEW_POKEMON_ID));

	datos->datos = *convertir_NEW_POKEMON(cantParametros - 1, parametros);
	datos->id = strtol(parametros[7], NULL, 10);

	return datos;
}

//./GameBoy BROKER APPEARED_POKEMON pikachu 3 1
//    1        2        3             4     5 6
//    0        1        2             3     4 5
DATOS_APPEARED_POKEMON* convertir_APPEARED_POKEMON(int cantParametros, char* parametros[])
{
	if (cantParametros != 6) TerminarProgramaConError("MANDAME BIEN LOS PARAMETROS, SABANDIJA");

	DATOS_APPEARED_POKEMON* datos = malloc(sizeof(DATOS_APPEARED_POKEMON_ID));

	//datos->largoPokemon = (uint32_t) strlen(parametros[3]);
	datos->pokemon = parametros[3];
	datos->posicion.posX = strtol(parametros[4], NULL, 10);
	datos->posicion.posY = strtol(parametros[5], NULL, 10);

	return datos;
}

//./GameBoy BROKER APPEARED_POKEMON pikachu 3 1 ID
//    1        2        3             4     5 6  7
//    0        1        2             3     4 5  6
DATOS_APPEARED_POKEMON_ID* convertir_APPEARED_POKEMON_ID(int cantParametros, char* parametros[])
{
	if (cantParametros != 7) TerminarProgramaConError("MANDAME BIEN LOS PARAMETROS, SABANDIJA");

	DATOS_APPEARED_POKEMON_ID* datos = malloc(sizeof(DATOS_APPEARED_POKEMON_ID));

	datos->datos = *convertir_APPEARED_POKEMON(cantParametros - 1, parametros);
	datos->id = strtol(parametros[6], NULL, 10); //datos->idCorrelativo_NEW = strtol(parametros[6], NULL, 10);

	return datos;
}

//./GameBoy BROKER CATCH_POKEMON pikachu 3 1
//    1        2        3          4     5 6
//    0        1        2          3     4 5
DATOS_CATCH_POKEMON* convertir_CATCH_POKEMON(int cantParametros, char* parametros[])
{
	if (cantParametros != 6) TerminarProgramaConError("MANDAME BIEN LOS PARAMETROS, SABANDIJA");

	DATOS_CATCH_POKEMON* datos = malloc(sizeof(DATOS_CATCH_POKEMON));

	//datos->largoPokemon = (uint32_t) strlen(parametros[3]);
	datos->pokemon = parametros[3];
	datos->posicion.posX = strtol(parametros[4], NULL, 10);
	datos->posicion.posY = strtol(parametros[5], NULL, 10);

	return datos;
}

//./GameBoy BROKER CATCH_POKEMON pikachu 3 1 ID
//    1        2        3          4     5 6  7
//    0        1        2          3     4 5  6
DATOS_CATCH_POKEMON_ID* convertir_CATCH_POKEMON_ID(int cantParametros, char* parametros[])
{
	if (cantParametros != 7) TerminarProgramaConError("MANDAME BIEN LOS PARAMETROS, SABANDIJA");

	DATOS_CATCH_POKEMON_ID* datos = malloc(sizeof(DATOS_CATCH_POKEMON_ID));

	datos->datos = *convertir_CATCH_POKEMON(cantParametros - 1, parametros);
	datos->id = strtol(parametros[7], NULL, 10);

	return datos;
}

//./GameBoy BROKER CAUGHT_POKEMON bool ID
//    1        2        3          4    5
//    0        1        2          3    4
DATOS_CAUGHT_POKEMON* convertir_CAUGHT_POKEMON_ID(int cantParametros, char* parametros[])
{
	if (cantParametros != 5) TerminarProgramaConError("MANDAME BIEN LOS PARAMETROS, SABANDIJA");

	DATOS_CAUGHT_POKEMON* datos = malloc(sizeof(DATOS_CAUGHT_POKEMON));

	datos->idCorrelativa = strtol(parametros[3], NULL, 10);
	datos->capturado = strtol(parametros[4], NULL, 10);

	return datos;
}

//./GameBoy BROKER GET_POKEMON Pokemon
//    1        2        3         4
//    0        1        2         3
DATOS_GET_POKEMON* convertir_GET_POKEMON(int cantParametros, char* parametros[])
{
	if (cantParametros != 4) TerminarProgramaConError("MANDAME BIEN LOS PARAMETROS, SABANDIJA");

	DATOS_GET_POKEMON* datos = malloc(sizeof(DATOS_GET_POKEMON));

	//datos->largoPokemon = (uint32_t) strlen(parametros[3]);
	datos->pokemon = parametros[3];

	return datos;
}

//./GameBoy BROKER GET_POKEMON Pokemon ID
//    1        2        3         4    5
//    0        1        2         3    4
DATOS_GET_POKEMON_ID* convertir_GET_POKEMON_ID(int cantParametros, char* parametros[])
{
	if (cantParametros != 5) TerminarProgramaConError("MANDAME BIEN LOS PARAMETROS, SABANDIJA");

	DATOS_GET_POKEMON_ID* datos = malloc(sizeof(DATOS_GET_POKEMON_ID));

	datos->datos = *convertir_GET_POKEMON(cantParametros - 1, parametros);
	datos->id = strtol(parametros[7], NULL, 10);

	return datos;
}

CodigoDeCola* convertirCodigo(char* codigo)
{
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

void ConexionBroker(Cliente* cliente, Paquete* paquete) {
	BROKER_DATOS_SUSCRIBIRSE datos;

	datos.cola = *((CodigoDeCola*) (cliente->info));

	EnviarMensajeSinFree(cliente, BROKER_SUSCRIBIRSE, &datos, (void*) &SerializarM_BROKER_SUSCRIBIRSE);

	free(cliente->info);
}

void ConectadoConProceso(char* proceso) {
	log_info(logger, "Me conecte correctamente al proceso %s", proceso);
}

void ConectadoConCola(char* cola) {
	log_info(logger, "Me suscribi correctamente a la cola: %s", cola);
}

void loggearMensajesRecibidos(Eventos* eventos, CodigoDeCola cola)
{
	if(cola == COLA_NEW_POKEMON)
		Eventos_AgregarOperacion(eventos, NEW_POKEMON, (EventoOperacion) &RecibirMensaje_NEW_POKEMON_ID);
	  else if(cola == COLA_APPEARED_POKEMON)
		Eventos_AgregarOperacion(eventos, APPEARED_POKEMON, (EventoOperacion) &RecibirMensaje_APPEARED_POKEMON_IDx2);
	  else if(cola == COLA_CATCH_POKEMON)
		Eventos_AgregarOperacion(eventos, CATCH_POKEMON, (EventoOperacion) &RecibirMensaje_CATCH_POKEMON_ID);
	  else if(cola == COLA_CAUGHT_POKEMON)
		Eventos_AgregarOperacion(eventos, CAUGHT_POKEMON, (EventoOperacion) &RecibirMensaje_CAUGHT_POKEMON_IDx2);
	  else if(cola == COLA_GET_POKEMON)
		Eventos_AgregarOperacion(eventos, GET_POKEMON, (EventoOperacion) &RecibirMensaje_GET_POKEMON_ID);
	  else if(cola == COLA_LOCALIZED_POKEMON)
		Eventos_AgregarOperacion(eventos, LOCALIZED_POKEMON, (EventoOperacion) &RecibirMensaje_LOCALIZED_POKEMON_IDx2);
}

void RecibirMensaje_NEW_POKEMON_ID(Cliente* cliente, Paquete* paqueteRecibido)
{
	Stream* stream = Stream_CrearLecturaPaquete(paqueteRecibido);
	uint32_t id = Deserializar_uint32(stream);
	DATOS_NEW_POKEMON datos = Deserializar_NEW_POKEMON(stream);

	if (!stream->error)
	{
		char* textoDatos = DatosAString_NEW_POKEMON(&datos);
		log_info(logger, "Recibido mensaje (id: %d): %s", id, textoDatos);
		free(textoDatos);
		EnviarID(cliente, id);
	}
	else
		log_error(logger, "Error al deserializar NEW_POKEMON");

	Stream_Destruir(stream);
}

void RecibirMensaje_APPEARED_POKEMON_IDx2(Cliente* cliente, Paquete* paqueteRecibido)
{
	Stream* stream = Stream_CrearLecturaPaquete(paqueteRecibido);
	uint32_t id = Deserializar_uint32(stream);
	uint32_t idCorrelativa = Deserializar_uint32(stream);
	DATOS_APPEARED_POKEMON datos = Deserializar_APPEARED_POKEMON(stream);

	if (!stream->error)
	{
		char* textoDatos = DatosAString_APPEARED_POKEMON(&datos);
		log_info(logger, "Recibido mensaje (id: %d, correlativo: %d): %s", id, idCorrelativa, textoDatos);
		free(textoDatos);
		EnviarID(cliente, id);
	}
	else
		log_error(logger, "Error al deserializar APPEARED_POKEMON");

	Stream_Destruir(stream);
}

void RecibirMensaje_CATCH_POKEMON_ID(Cliente* cliente, Paquete* paqueteRecibido)
{
	Stream* stream = Stream_CrearLecturaPaquete(paqueteRecibido);
	uint32_t id = Deserializar_uint32(stream);
	DATOS_CATCH_POKEMON datos = Deserializar_CATCH_POKEMON(stream);

	if (!stream->error)
	{
		char* textoDatos = DatosAString_CATCH_POKEMON(&datos);
		log_info(logger, "Recibido mensaje (id: %d): %s", id, textoDatos);
		free(textoDatos);
		EnviarID(cliente, id);
	}
	else
		log_error(logger, "Error al deserializar CATCH_POKEMON");

	Stream_Destruir(stream);
}

void RecibirMensaje_CAUGHT_POKEMON_IDx2(Cliente* cliente, Paquete* paqueteRecibido)
{
	Stream* stream = Stream_CrearLecturaPaquete(paqueteRecibido);
	uint32_t id = Deserializar_uint32(stream);
	uint32_t idCorrelativa = Deserializar_uint32(stream);
	DATOS_CAUGHT_POKEMON datos = Deserializar_CAUGHT_POKEMON(stream);

	if (!stream->error)
	{
		char* textoDatos = DatosAString_CAUGHT_POKEMON(&datos);
		log_info(logger, "Recibido mensaje (id: %d, correlativo: %d): %s", id, idCorrelativa, textoDatos);
		free(textoDatos);
		EnviarID(cliente, id);
	}
	else
		log_error(logger, "Error al deserializar CAUGHT_POKEMON");

	Stream_Destruir(stream);
}

void RecibirMensaje_GET_POKEMON_ID(Cliente* cliente, Paquete* paqueteRecibido)
{
	Stream* stream = Stream_CrearLecturaPaquete(paqueteRecibido);
	uint32_t id = Deserializar_uint32(stream);
	DATOS_GET_POKEMON datos = Deserializar_GET_POKEMON(stream);

	if (!stream->error)
	{
		char* textoDatos = DatosAString_GET_POKEMON(&datos);
		log_info(logger, "Recibido mensaje (id: %d): %s", id, textoDatos);
		free(textoDatos);
		EnviarID(cliente, id);
	}
	else
		log_error(logger, "Error al deserializar GET_POKEMON");

	Stream_Destruir(stream);
}

void RecibirMensaje_LOCALIZED_POKEMON_IDx2(Cliente* cliente, Paquete* paqueteRecibido)
{
	Stream* stream = Stream_CrearLecturaPaquete(paqueteRecibido);
	uint32_t id = Deserializar_uint32(stream);
	uint32_t idCorrelativa = Deserializar_uint32(stream);
	DATOS_LOCALIZED_POKEMON datos = Deserializar_LOCALIZED_POKEMON(stream);

	if (!stream->error)
	{
		char* textoDatos = DatosAString_LOCALIZED_POKEMON(&datos);
		log_info(logger, "Recibido mensaje (id: %d, correlativo: %d): %s", id, idCorrelativa, textoDatos);
		free(textoDatos);
		EnviarID(cliente, id);
	}
	else
		log_error(logger, "Error al deserializar LOCALIZED_POKEMON");

	Stream_Destruir(stream);
}

void EnviarID(Cliente* cliente, uint32_t identificador)
{
	DATOS_ID_MENSAJE* id_mensaje = malloc(sizeof(DATOS_ID_MENSAJE));

	id_mensaje->id = identificador;
	log_error(logger, "Enviando ACK %d", id_mensaje->id);
	EnviarMensaje(cliente, BROKER_ACK, id_mensaje, (void*) &SerializarM_ID_MENSAJE);

	free(id_mensaje);

}
