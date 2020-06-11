#include "broker.h"
#include "servidor.h"
#include "memoria.h"
#include "cola.h"
#include <commons/config.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>

static char* Config_String(char* variable);
static int Config_Int(char* variable);
static void EsperarHilos();
static void Finalizar();
static void EscucharSignal();
static void EscuchaSignal(int signal);

t_config* config;

int main()
{
	config = config_create("broker.config");
	logger = log_create(Config_String("LOG_FILE"), Config_String("LOG_FILE"), true, LOG_LEVEL_INFO);

	IniciarMensajes();
	IniciarMemoria(Config_Int("TAMANO_MEMORIA"), Config_String("ALGORITMO_MEMORIA"), Config_String("ALGORITMO_REEMPLAZO"), Config_String("ALGORITMO_PARTICION_LIBRE"), Config_Int("FRECUENCIA_COMPACTACION"));
	CrearColas();
	EscucharSignal();
	IniciarServidorBroker(Config_String("IP_BROKER"), Config_Int("PUERTO_BROKER"));

	EsperarHilos();
	Finalizar();
}

static char* Config_String(char* variable) { return config_get_string_value(config, variable); }
static int Config_Int(char* variable) { return config_get_int_value(config, variable); }

static void EsperarHilos()
{
	pthread_mutex_t mx_main;
	pthread_mutex_init(&mx_main, NULL);
	pthread_mutex_lock(&mx_main);
	pthread_mutex_lock(&mx_main);
}

static void Finalizar()
{
	FinalizarServidorBroker();
	DestruirColas();
	DestruirMemoria();
	config_destroy(config);
	log_destroy(logger);
	pthread_exit(0);
}

static void EscucharSignal()
{
	log_info(logger, "PID: %d", getpid());
	if (signal(SIGUSR1, EscuchaSignal) == SIG_ERR)
	{
		log_error(logger, "Error al inciar escucha de signal");
		Finalizar();
	}
}

static void EscuchaSignal(int signo)
{
    if (signo == SIGUSR1)
    	Dump();
}
