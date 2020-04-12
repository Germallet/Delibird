#include "tp0.h"
#include "socket.h"

int main()
{
	t_log* logger = log_create("tp0.log", "Game & Watch", true, LOG_LEVEL_INFO);
	t_config* config = config_create("tp0.config");
	char* ip = config_get_string_value(config, "IP");
	char* puerto = config_get_string_value(config, "PUERTO");

	log_info(logger, "Iniciando");

	int conexion = Socket_Crear(ip, puerto);
	if (conexion == -1)
	{
		log_error(logger, "No se pudo crear la conexion");
		terminar_programa(conexion, logger, config);
		return 1;
	}

	// Socket_Enviar
	char* mensaje = "Mensaje!";
	if (Socket_Enviar(MENSAJE, mensaje, strlen(mensaje)+1, conexion) == -1)
	{
		log_error(logger, "No se pudo enviar el mensaje");
		terminar_programa(conexion, logger, config);
		return 1;
	}
	// free(stream); LOS METODOS NO LIBERAN EL STREAM ENVIADO
	log_info(logger, "Mensaje enviado: %s", mensaje);

	// Socket_Recibir -> SOLO LEE EL CODIGO DE OPERACION
	Paquete* paqueteRecibido = 0;
	if (Socket_Recibir(conexion, &paqueteRecibido) == -1)
	{
		log_error(logger, "No se pudo recibir el mensaje");
		terminar_programa(conexion, logger, config);
		return 1;
	}
	if (paqueteRecibido->codigoOperacion != MENSAJE)
	{
		log_error(logger, "Mensaje recibido invalido");
		terminar_programa(conexion, logger, config);
		return 1;
	}
	// Socket_ProcesarPaquete -> LEE EL TAMANIO Y EL RESTO DEL PAQUETE EN STREAM
	if (Socket_ProcesarPaquete(conexion, paqueteRecibido) == -1)
	{
		log_error(logger, "Error al procesar paquete");
		terminar_programa(conexion, logger, config);
		return 1;
	}

	log_info(logger, "Mensaje recibido: %s", paqueteRecibido->stream);
	Socket_LiberarPaquete(paqueteRecibido); // LIBERAR PAQUETE

	//fin
	log_info(logger, "FIN");
	terminar_programa(conexion, logger, config);
}

void terminar_programa(int conexion, t_log* logger, t_config* config)
{
	log_destroy(logger);
	config_destroy(config);
	Socket_LiberarConexion(conexion);
}
