#include "team.h"
#include "pokemon.h"
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

	//MUTEX
	pthread_mutex_init(&(mutex_team), NULL);
	pthread_mutex_lock(&(mutex_team));
}

static void esperar_fin_de_ciclo() { pthread_mutex_lock(&mutex_team); }

static void solicitar_pokemons_para_objetivo_global_test()
{
	agregar_pokemon_a_mapa("Pikachu", (Posicion*) crear_posicion("2|3"));
	agregar_pokemon_a_mapa("Charmander", (Posicion*) crear_posicion("6|7"));
	agregar_pokemon_a_mapa("Squirtle", (Posicion*)  crear_posicion("0|0"));
}

//-----------HILO PRINCIPAL-----------//
int main()
{
	inicializar_datos();
	obtener_entrenadores();

	log_info(logger,"Pikachu == %s = TRUE", list_get(((Entrenador*) list_get(cola_NEW, 0))->pokemons_atrapados, 0));

	identificar_objetivo_global();
	solicitar_pokemons_para_objetivo_global_test();
	//solicitar_pokemons_para_objetivo_global();
	//conectarse_y_suscribirse_a_colas();
	//TODO: conectarse_con_gameboy();

	log_info(logger, "necesitamos %d %s", cantidad_de_pokemon_en_lista(pokemons_necesarios, "Pikachu"), "Pikachu");
	log_info(logger, "necesitamos %d %s", cantidad_de_pokemon_en_lista(pokemons_necesarios, "Charmander"), "Charmander");
	log_info(logger, "necesitamos %d %s", cantidad_de_pokemon_en_lista(pokemons_necesarios, "Squirtle"), "Squirtle");

	while(true)
	{
		while(hay_interrupciones_para_ejecutar()) ejecutar_interrupcion();
		planificar_entrenador_si_es_necesario();
		ejecutar_entrenador_actual();
		esperar_fin_de_ciclo();
	}
}






