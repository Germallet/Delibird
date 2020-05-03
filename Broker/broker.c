#include "broker.h"
#include "servidor.h"
#include "memoria.h"
#include "cola.h"
#include <commons/config.h>
#include <string.h>
#include <pthread.h>

char* Config_String(char* variable);
int Config_Int(char* variable);
void EsperarHilos();
void Finalizar();

t_config* config;

int main()
{
	config = config_create("broker.config");
	logger = log_create(Config_String("LOG_FILE"), "Broker", true, LOG_LEVEL_INFO);

	IniciarMemoria();
	CrearColas();
	IniciarServidorBroker(Config_String("IP_BROKER"), Config_Int("PUERTO_BROKER"));

	EsperarHilos();
	Finalizar();
}

char* Config_String(char* variable) { return config_get_string_value(config, variable); }
int Config_Int(char* variable) { return config_get_int_value(config, variable); }

void EsperarHilos()
{
	pthread_mutex_t mx_main;
	pthread_mutex_init(&mx_main, NULL);
	pthread_mutex_lock(&mx_main);
	pthread_mutex_lock(&mx_main);
}

void Finalizar()
{
	DestruirColas();
	config_destroy(config);
	log_destroy(logger);
}
