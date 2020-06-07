#include "gameCard.h"
#include "mensajes.h"
//#include <fuse.h>
//
//#define D_FILE_OFFSET_BITS 64

t_config* config;

Eventos* eventos;

Cliente* clienteBroker;

t_bitarray* bitmap;
/*
 * TODO ESTRUCTURA DE LOS ARCHIVOS METADATA
 * TODO BITMAP
 * TODO HACER FUNCIONES PARA LEER, ESCRIBIR, BUSCAR EL ARCHIVO, VERIFICAR SI SE PUEDE ABRIR, OBTENER POSICIONES Y CANTIDADES DE ARCHIVO
 * TODO ELIMINAR CANTIDADES Y POSICIONES DE ARCHIVO
 * TODO HACER FILESYSTEM
 */

int main()
{
	logger = log_create("gameCard.log", "GameCard", true, LOG_LEVEL_INFO);
	config = config_create("gameCard.config");
	log_info(logger, "Bienvenido a la GameCard!");
	char* miIp = config_get_string_value(config,"IP_GAMECARD");
	int miPuerto = config_get_int_value(config,"PUERTO_GAMECARD");

	char* puntoMontaje = config_get_string_value(config,"PUNTO_MONTAJE_TALLGRASS");
	int tiempoReintentoConexion = config_get_int_value(config,"TIEMPO_DE_REINTENTO_CONEXION");
	int tiempoReintentoOperacion = config_get_int_value(config,"TIEMPO_DE_REINTENTO_OPERACION");
	int tiempoRetardoOperacion = config_get_int_value(config,"TIEMPO_RETARDO_OPERACION");

	CrearHiloTimer(-1,tiempoReintentoConexion,&reconexion,NULL);

	SocketEscucha(miIp, miPuerto);

	EsperarHilos();

	//EN ALGUN LADO METER UN FORK PARA TENER VARIOS PROCESO GAME CARD

	log_info(logger,puntoMontaje);
	log_info(logger,"/s",tiempoReintentoConexion);
	log_info(logger,"/s",tiempoReintentoOperacion);
	log_info(logger,"/s",tiempoRetardoOperacion);

	TerminarPrograma(logger,config);
	return 0;
}

void conectarse() {

	char* ipBroker = config_get_string_value(config,"IP_BROKER");
	int puertoBroker = config_get_int_value(config,"PUERTO_BROKER");

	eventos = Eventos_Crear0();
	clienteBroker = CrearCliente(ipBroker,puertoBroker,eventos);

	SuscribirseColas(clienteBroker);
}

void reconexion() { if (clienteBroker == NULL) conectarse(); }

void EsperarHilos()
{
	pthread_mutex_t mx_main;
	pthread_mutex_init(&mx_main, NULL);
	pthread_mutex_lock(&mx_main);
	pthread_mutex_lock(&mx_main);
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
