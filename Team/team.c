#include "team.h"
#include "pokemon.h"
#include "posicion.h"
#include "entrenador.h"
#include "suscripcion.h"
#include "planificacion.h"
#include "interrupciones.h"
#include "../Utils/hiloTimer.h"
#include <stdlib.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/collections/list.h>

t_log* logger;
t_config* config;
pthread_mutex_t mutex_team;

static void inicializar_datos()
{
	//LOGGER
	logger = log_create("team.log", "Team", true, LOG_LEVEL_INFO);

	if(logger==NULL)
	{
		printf("No se pudo crear el team logger");
		exit(1);
	}

	//CONFIG
	config = config_create("team.config");

	if(config==NULL)
	{
		log_error(logger,"No se pudo encontrar el archivo team.config");
		exit(2);
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

	//HILO TIMER
	//inicializar_hilo_reconexion(); TODO reconexion

	//INTERRUPCIONES
	inicializar_interrupciones();

	//INTERRUPCIONES
	deadlocks = list_create();

	//MUTEX
	pthread_mutex_init(&(mutex_team), NULL);
	pthread_mutex_lock(&(mutex_team));
}

static void esperar_fin_de_ciclo() { pthread_mutex_lock(&mutex_team); }

static void solicitar_pokemons_para_objetivo_global_test()
{
	agregar_pokemon_a_mapa("Pikachu", crear_posicion("1|1"));
	agregar_pokemon_a_mapa("Squirtle", crear_posicion("9|7"));
	agregar_pokemon_a_mapa("Onix", crear_posicion("2|2"));
}

//-----------HILO PRINCIPAL-----------//
int main()
{
	inicializar_datos();
	obtener_entrenadores();
	identificar_objetivo_global();
	solicitar_pokemons_para_objetivo_global_test();
	//solicitar_pokemons_para_objetivo_global();
	//conectarse_y_suscribirse_a_colas();
	//TODO: conectarse_con_gameboy();

	while(true)
	{
		while(hay_interrupciones_para_ejecutar()) ejecutar_interrupcion();
		planificar_entrenador_si_es_necesario();

		if(entrenador_EXEC!=NULL)
		{
			if(list_get(entrenador_EXEC->pokemons_atrapados, 0)!=NULL) log_info(logger,"%s (%d)", ((Pokemon*) list_get(entrenador_EXEC->pokemons_atrapados, 0))->especie, ((Pokemon*) list_get(entrenador_EXEC->pokemons_atrapados, 0))->cantidad);
			if(list_get(entrenador_EXEC->pokemons_atrapados, 1)!=NULL) log_info(logger,"%s (%d)", ((Pokemon*) list_get(entrenador_EXEC->pokemons_atrapados, 1))->especie, ((Pokemon*) list_get(entrenador_EXEC->pokemons_atrapados, 0))->cantidad);
		}

		ejecutar_entrenador_actual();
		esperar_fin_de_ciclo();
	}
}






