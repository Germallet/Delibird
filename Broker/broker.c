#include "broker.h"
#include <commons/log.h>
#include <commons/collections/list.h>
#include "../Utils/socket.h"

t_log* logger;

void NuevoCliente()
{
	log_info(logger, "Se conecto un cliente!");
}

int main()
{
	logger = log_create("broker.log", "Broker", true, LOG_LEVEL_INFO);
	log_info(logger, "Broker!");

	int socketServidor = Socket_IniciarEscucha(4444, &NuevoCliente);
	log_info(logger, "Escucha iniciada");

	pthread_mutex_t mx_main;
	pthread_mutex_init(&mx_main, NULL);
	pthread_mutex_lock(&mx_main);
	pthread_mutex_lock(&mx_main);

	log_info(logger, "Saliendo");
	log_destroy(logger);
}
