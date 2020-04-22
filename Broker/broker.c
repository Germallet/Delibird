#include "broker.h"
#include <commons/log.h>
#include "../Utils/socket.h"

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
void ClienteOperacion_MENSAJE(DatosConexion* conexion, Paquete* paqueteRecibido)
{
	log_info(logger, "ClienteOperacion_MENSAJE: %s", paqueteRecibido->stream);
}
void ClienteOperacion_NEW_POKEMON(DatosConexion* conexion, Paquete* paqueteRecibido)
{
	log_info(logger, "ClienteOperacion_NEW_POKEMON");
}

int main()
{
	logger = log_create("broker.log", "Broker", true, LOG_LEVEL_INFO);

	// Crear Eventos
	Eventos* eventos = Eventos_Crear(&ClienteConectado, &ClienteDesconectado, &ClienteError);
	Eventos_AgregarOperacion(eventos, MENSAJE, &ClienteOperacion_MENSAJE);
	Eventos_AgregarOperacion(eventos, NEW_POKEMON, &ClienteOperacion_NEW_POKEMON);

	// Iniciar escucha
	Socket_IniciarEscucha(5003, eventos);
	log_info(logger, "Escucha iniciada");

	pthread_mutex_t mx_main;
	pthread_mutex_init(&mx_main, NULL);
	pthread_mutex_lock(&mx_main);
	pthread_mutex_lock(&mx_main);

	log_info(logger, "Saliendo");
	log_destroy(logger);
}
