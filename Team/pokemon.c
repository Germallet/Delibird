#include "pokemon.h"
#include "posicion.h"
#include "entrenador.h"
#include <string.h>
#include <stdlib.h>
#include <commons/string.h>
#include <commons/collections/list.h>

t_list* pokemons_necesarios;
t_list* especies_localizadas;
t_list* pokemons_mapa;

//-----------POKEMON-----------//
Pokemon* crear_pokemon(char* especie)
{
	Pokemon* pokemon = malloc(sizeof(Pokemon));

	pokemon->especie = especie;
	pokemon->cantidad = 1;

	return pokemon;
}

void destruir_pokemon(void* pokemon) { free(pokemon); }

void destruir_pokemon_mapa(void* pokemon)
{
	free(((Pokemon_Mapa*) pokemon)->especie);
	free(((Pokemon_Mapa*) pokemon)->posicion);
	free(pokemon);
}

//-----------LISTA POKEMON-----------//
t_list* crear_lista_pokemon(char* pokemons)
{
	if(strlen(pokemons) == 0)
		return list_create();

	t_list* lista_pokemon = list_create();

	int cantidad_pokemons = 1;
	for(int i=0;pokemons[i]!='\0';i++)
			cantidad_pokemons += pokemons[i] == '|';

	char** array_pokemons = string_split(pokemons, "|");

	for(int i=0;i<cantidad_pokemons;i++)
	{
		if(array_pokemons[i]!=NULL) agregar_pokemon(lista_pokemon, array_pokemons[i]);
		free(array_pokemons[i]);
	}
	free(array_pokemons);

	return lista_pokemon;
}

void agregar_pokemon(t_list* lista_pokemon, char* especie_pokemon)
{
	bool tiene_pokemon = false;

	for(int i=0;i<lista_pokemon->elements_count;i++)
	{
		Pokemon* pokemon_actual = ((Pokemon*) list_get(lista_pokemon, i));
		if(strcmp(pokemon_actual->especie, especie_pokemon)==0)
		{
			pokemon_actual->cantidad++;
			tiene_pokemon = true;
		}
	}

	Pokemon* nuevo_pokemon()
	{
		Pokemon* nuevo_pokemon = malloc(sizeof(Pokemon));
		nuevo_pokemon->especie = especie_pokemon;
		nuevo_pokemon->cantidad = 1;

		return nuevo_pokemon;
	}

	if(!tiene_pokemon)
		list_add(lista_pokemon, nuevo_pokemon());
}

void agregar_pokemon_a_mapa(char* especie_pokemon, Posicion* posicion)
{
	Pokemon_Mapa* pokemon = malloc(sizeof(Pokemon_Mapa));
	pokemon->especie = malloc(strlen(especie_pokemon)+1);
	strcpy(pokemon->especie, especie_pokemon);
	pokemon->posicion = posicion;
	list_add(pokemons_mapa, pokemon);
}

bool tiene_pokemon(t_list* lista_pokemon, char* especie_pokemon)
{
	for(int i=0;i<lista_pokemon->elements_count;i++)
	{
		Pokemon* pokemon = ((Pokemon*) list_get(lista_pokemon, i));
		if(strcmp(pokemon->especie, especie_pokemon)==0)
			return true;
	}
	return false;
}

int cantidad_de_pokemon_en_lista(t_list* lista_pokemon, char* especie_pokemon)
{
	Pokemon* pokemon;

	for(int i=0;i<lista_pokemon->elements_count;i++)
	{
		pokemon = ((Pokemon*) list_get(lista_pokemon, i));
		if(strcmp(pokemon->especie, especie_pokemon)==0)
			return pokemon->cantidad;
	}
	 return 0;
}

Pokemon* tomar_pokemon(t_list* lista_pokemon, char* especie_pokemon)
{
	Pokemon* pokemon;
	bool tomo_pokemon = false;
	for(int i=0;i<lista_pokemon->elements_count && !tomo_pokemon;i++)
	{
		pokemon = ((Pokemon*) list_get(lista_pokemon, i));
		if(strcmp(pokemon->especie, especie_pokemon)==0)
		{
			pokemon->cantidad--;
			if(pokemon->cantidad==0) list_remove_and_destroy_element(lista_pokemon, i, &destruir_pokemon);
			tomo_pokemon = true;
		}
	}
	return tomo_pokemon ? pokemon : NULL;
}

bool es_mismo_pokemon_mapa(Pokemon_Mapa* pokemon_1, Pokemon_Mapa* pokemon_2)
{ return (strcmp(pokemon_1->especie, pokemon_2->especie)==0) && es_misma_posicion(pokemon_1->posicion, pokemon_2->posicion); }

void sacar_pokemon_de_mapa(Pokemon_Mapa* pokemon)
{
	for(int i=0;i<pokemons_mapa->elements_count;i++)
		if(es_mismo_pokemon_mapa(pokemon, (Pokemon_Mapa*) list_get(pokemons_mapa, i))) list_remove(pokemons_mapa, i);
}

void se_asigno_para_capturar(Pokemon_Mapa* pokemon)
{
	tomar_pokemon(pokemons_necesarios, pokemon->especie);
	sacar_pokemon_de_mapa(pokemon);
}

bool esta_localizada(char* especie_pokemon)
{
	bool es_misma_especie(void* especie) { return strcmp((char*) especie, especie_pokemon)==0;	}

	return list_any_satisfy(especies_localizadas, &es_misma_especie);
}

void se_localizo(char* especie_pokemon) { list_add(especies_localizadas, especie_pokemon); }

//-----------TEAM-----------//
void identificar_objetivo_global() // identifica especies y cantidades para completar el objetivo global
{
	pokemons_necesarios = list_create();
	for(int i =0; i<cola_NEW->elements_count;i++)
	{
		Entrenador* entrenador_actual = ((Entrenador*) list_get(cola_NEW, i));
		for(int j =0; j<entrenador_actual->pokemons_objetivo->elements_count;j++)
		{
			Pokemon* pokemon_actual = ((Pokemon*) list_get(entrenador_actual->pokemons_objetivo, j));
			for(int cantidad_pokemon=0; cantidad_pokemon<pokemon_actual->cantidad;cantidad_pokemon++)
				agregar_pokemon(pokemons_necesarios,pokemon_actual->especie);
		}

	}
	for(int i = 0; i<cola_NEW->elements_count;i++)
	{
		Entrenador* entrenador_actual = ((Entrenador*) list_get(cola_NEW, i));
		for(int j = 0; j<entrenador_actual->pokemons_atrapados->elements_count;j++)
		{
			Pokemon* pokemon_actual = ((Pokemon*) list_get(entrenador_actual->pokemons_atrapados, j));
			for(int cantidad_pokemon=0; cantidad_pokemon<pokemon_actual->cantidad;cantidad_pokemon++)
				tomar_pokemon(pokemons_necesarios,pokemon_actual->especie);
		}
	}
}

//-----------PLANIFICACION-----------//
int obtener_entrenador_disponible_mas_cercano(Pokemon_Mapa* pokemon, Entrenador** entrenador_mas_cercano) // Retorna la distancia al entrenador
{
	t_list* entrenadores_disponibles = obtener_entrenadores_disponibles();
	if(list_is_empty(entrenadores_disponibles)) return -1;

	t_list* distancias_a_entrenadores = list_map(entrenadores_disponibles, (void*) &obtener_posicion_entrenador);

	int indice;
	int distancia = distancia_menor(pokemon->posicion, distancias_a_entrenadores, &indice);

	*entrenador_mas_cercano = list_get(entrenadores_disponibles, indice);

	list_destroy(entrenadores_disponibles);
	list_destroy(distancias_a_entrenadores);

	return distancia;
}

void pokemon_y_entrenador_mas_cercanos_entre_si(char* especie_pokemon, Pokemon_Mapa** pokemon, void** entrenador)
{
	bool es_especie_que_busco(void* pokemon_que_busco) { return strcmp(((Pokemon_Mapa*) pokemon_que_busco)->especie, especie_pokemon)==0; }

	t_list* pokemons_que_busco = list_filter(pokemons_mapa, &es_especie_que_busco);

	if(!list_is_empty(pokemons_que_busco))
	{
		*pokemon = (Pokemon_Mapa*) list_remove(pokemons_que_busco, 0);
		int distancia = obtener_entrenador_disponible_mas_cercano(*pokemon, (Entrenador**) entrenador);
		Pokemon_Mapa* pokemon_pivot = NULL;
		Entrenador* entrenador_pivot = NULL;
		int nueva_distancia;
		for(int i=1; i < pokemons_que_busco->elements_count;i++)
		{
			pokemon_pivot = list_get(pokemons_que_busco, i);
			nueva_distancia = obtener_entrenador_disponible_mas_cercano(pokemon_pivot, (Entrenador**) entrenador_pivot);
			if(nueva_distancia<distancia)
			{
				*pokemon = pokemon_pivot;
				*entrenador = entrenador_pivot;
				distancia = nueva_distancia;
			}
		}
	}
	list_destroy(pokemons_que_busco);
}

void borrar_especie_de_mapa(char* especie)
{
	for(int i=0;i<pokemons_mapa->elements_count;i++)
	{
		bool es_misma_especie = strcmp(((Pokemon_Mapa*) list_get(pokemons_mapa, i))->especie, especie)==0; ;

		if(es_misma_especie) list_remove_and_destroy_element(pokemons_mapa, i, &destruir_pokemon_mapa);
	}
}

t_list* especies_en_mapa()
{
	t_list* especies_en_mapa = list_create();

	for(int i=0;i<pokemons_mapa->elements_count;i++)
	{
		char* especie_pokemon_mapa = ((Pokemon_Mapa*) list_get(pokemons_mapa, i))->especie;
		bool es_misma_especie(void* especie) { return strcmp((char*) especie_pokemon_mapa, especie)==0; }
		bool tiene_especie = list_find(especies_en_mapa, &es_misma_especie);

		if(!tiene_especie)
			list_add(especies_en_mapa, especie_pokemon_mapa);
	}

	return especies_en_mapa;
}
char* una_especie_que_necesito_y_esta_en_mapa() { return ((Pokemon_Mapa*) list_get(pokemons_mapa, 0))->especie; }
bool necesito_especie_pokemon(char* especie) { return tiene_pokemon(pokemons_necesarios, especie); }
bool necesito_pokemon_mapa(Pokemon_Mapa* pokemon) { return necesito_especie_pokemon(pokemon->especie); }



