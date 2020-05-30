#include "gameCard.h"


t_log* logger;
t_config* config;

int main()
{
	logger = log_create("gameCard.log", "GameCard", true, LOG_LEVEL_INFO);
	config = config_create("gameBoy.config");
	log_info(logger, "Bienvenido a la GameCard!");

	char* puntoMontaje = config_get_string_value(config,"PUNTO_MONTAJE_TALLGRASS");
	int tiempoReintentoConexion = config_get_int_value(config,"TIEMPO_DE_REINTENTO_CONEXION");
	int tiempoReintentoOperacion = config_get_int_value(config,"TIEMPO_DE_REINTENTO_OPERACION");
	int tiempoRetardoOperacion = config_get_int_value(config,"TIEMPO_RETARDO_OPERACION");
	char* ipBroker = config_get_int_value(config,"IP_BROKER");
	int puertoBroker = config_get_int_value(config,"PUERTO_BROKER");


	SuscribirseColas(ipBroker,puertoBroker);

	TerminarPrograma(logger,config);
	return 0;
}


void SuscribirseColas(char* ip,int puerto) {

}

//void EnviarID(Cliente* cliente, uint32_t identificador)
//{
//	DATOS_ID_MENSAJE* id_mensaje = malloc(sizeof(DATOS_ID_MENSAJE));
//
//	id_mensaje->id = identificador;
//	log_error(logger, "Enviando ACK %d", id_mensaje->id);
//	EnviarMensaje(cliente, BROKER_ACK, id_mensaje, (void*) &SerializarM_ID_MENSAJE);
//
//	free(id_mensaje);
//
//}


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
