#include "team.h"
#include "pokemon.h"
#include "posicion.h"
#include "entrenador.h"
#include "suscripcion.h"
#include "planificacion.h"
#include "interrupciones.h"
#include "servidorTeam.h"
#include "../Utils/hiloTimer.h"
#include <stdlib.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/collections/list.h>

int cantidad_ciclos;
t_log* logger;
t_config* config;
pthread_mutex_t mutex_team;

static void inicializar_datos()
{
	//CONFIG
	config = config_create("team.config");

	if(config==NULL)
	{
		log_error(logger,"No se pudo encontrar el archivo team.config");
		exit(2);
	}

	//LOGGER
	logger = log_create(config_get_string_value(config,"LOG_FILE"), "Team", true, LOG_LEVEL_INFO);

	if(logger==NULL)
	{
		printf("No se pudo crear el team logger");
		exit(1);
	}

	//COLAS
	entrenador_EXEC = NULL;
	cola_NEW = list_create();
	cola_READY = list_create();
	cola_BLOCKED = list_create();
	cola_EXIT = list_create();

	//POKEMONS
	pokemons_necesarios = list_create();
	pokemons_mapa = list_create();
	especies_localizadas = list_create();

	//DEADLOCK
	deadlocks = list_create();

	//INTERRUPCIONES
	inicializar_interrupciones();

	//MENSAJES ESPERADOS
	id_mensajes_esperados = list_create();

	//CONTADOR DE CICLOS
	cantidad_ciclos = 0;
	quantum = 0;

	//MUTEX
	pthread_mutex_init(&(mutex_team), NULL);
	pthread_mutex_lock(&(mutex_team));
}

static void esperar_fin_de_ciclo()
{
	pthread_mutex_lock(&mutex_team);
	cantidad_ciclos++;
	if(es_planificacion_tipo("RR"))
	{
		if(hay_entrenador_en_ejecucion())
			aumentar_quantum();
		else
			reiniciar_quantum();
	}
}

static void solicitar_pokemons_para_objetivo_global_test()
{
	agregar_pokemon_a_mapa("Pikachu", crear_posicion("1|1"));
	agregar_pokemon_a_mapa("Squirtle", crear_posicion("9|7"));
	agregar_pokemon_a_mapa("Onix", crear_posicion("2|2"));
	agregar_pokemon_a_mapa("Squirtle", crear_posicion("3|5"));
	agregar_pokemon_a_mapa("Gengar", crear_posicion("7|5"));
}

//-----------HILO PRINCIPAL-----------//
int main()
{
	inicializar_datos();
	obtener_entrenadores();
	identificar_objetivo_global();
	IniciarServidorTeam(config_get_string_value(config,"IP_TEAM"), config_get_int_value(config,"PUERTO_TEAM"));
	solicitar_pokemons_para_objetivo_global_test();
	solicitar_pokemons_para_objetivo_global();
	conectarse_y_suscribirse_a_colas();

	while(true)
	{
		ejecutar_interrupciones_existentes();
		planificar_entrenador_si_es_necesario();
		ejecutar_entrenador_actual();
		esperar_fin_de_ciclo();
	}
}
