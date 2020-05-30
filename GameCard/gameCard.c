#include "gameCard.h"
#include "mensajes.h"

t_config* config;

int main()
{
	logger = log_create("gameCard.log", "GameCard", true, LOG_LEVEL_INFO);
	config = config_create("gameCard.config");
	log_info(logger, "Bienvenido a la GameCard!");
	char* miIp = config_get_string_value(config,"IP_GAMECARD");
	int miPuerto = config_get_int_value(config,"PUERTO_GAMECARD");

//	char* puntoMontaje = config_get_string_value(config,"PUNTO_MONTAJE_TALLGRASS");
//	int tiempoReintentoConexion = config_get_int_value(config,"TIEMPO_DE_REINTENTO_CONEXION");
//	int tiempoReintentoOperacion = config_get_int_value(config,"TIEMPO_DE_REINTENTO_OPERACION");
//	int tiempoRetardoOperacion = config_get_int_value(config,"TIEMPO_RETARDO_OPERACION");
//	char* ipBroker = config_get_string_value(config,"IP_BROKER");
//	int puertoBroker = config_get_int_value(config,"PUERTO_BROKER");
//
//	Eventos* eventos = Eventos_Crear0();

//	Cliente clienteBroker = CrearCliente(ipBroker,puertoBroker,eventos);

//	SuscribirseColas(clienteBroker);

	SocketEscucha(miIp, miPuerto);

	//EN ALGUN LADO METER UN FORK PARA TENER VARIOS PROCESO GAME CARD

	TerminarPrograma(logger,config);
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
