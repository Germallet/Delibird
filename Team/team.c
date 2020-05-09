#include "team.h"
#include "pokemon.h"
#include "entrenador.h"
#include "interrupciones.h"
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
	cola_BLOCK = list_create();
	cola_EXIT = list_create();

	//POKEMONS
	pokemons_necesarios = list_create();
	pokemons_mapa = list_create();
	pokemons_localizados = list_create();

	//INTERRUPCIONES
	inicializar_interrupciones();

	//MUTEX
	pthread_mutex_init(&(mutex_team), NULL);
	pthread_mutex_lock(&(mutex_team));
}

//-----------HILO PRINCIPAL-----------//
int main()
{
	inicializar_datos();
	inicializar_entrenadores();
	identificar_pokemons_necesarios();

	//TODO: suscribirse_a_colas();

	////////////////////////////////////////////////////

	log_info(logger,"Los pokemon objetivo del team son (%d especies):", pokemons_necesarios->elements_count);
	for(int i =0; i<pokemons_necesarios->elements_count;i++)
	{
		Pokemon* pokemon_actual = ((Pokemon*) list_get(pokemons_necesarios, i));
		log_info(logger,"%s (%d)", pokemon_actual->especie, pokemon_actual->cantidad);
	}

	log_info(logger,"Hay %d entrenadores en la cola NEW", cola_NEW->elements_count);
	for(int i =0; i<cola_NEW->elements_count;i++)
		log_info(logger,"el entrenador %d esta en la posicion %d,%d", ((Entrenador*) (list_get(cola_NEW,i)))->ID, ((Entrenador*) (list_get(cola_NEW,i)))->posicion->posX, ((Entrenador*) (list_get(cola_NEW,i)))->posicion->posY);

	Posicion* posicion = malloc(sizeof(Posicion));
	posicion->posX=5;
	posicion->posY=7;

	int id_mas_cercano = entrenador_mas_cercano(cola_NEW, posicion)->ID;

	log_info(logger, "el entrenador mas cercano a la posicion %d,%d es el entrenador %d", posicion->posX, posicion->posY, id_mas_cercano);

	////////////////////////////////////////////////////

	interrupcion_TERMINAR(NULL);

	while(true)
	{
		while(hay_interrupciones()) ejecutar_interrupcion();
		ejecutar_entrenador_actual();
		pthread_mutex_lock(&mutex_team);
	}
}
