#include "pokemon.h"
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
	free(((Pokemon_Mapa*) pokemon)->posicion);
	free(pokemon);
}

//-----------LISTA POKEMON-----------//
t_list* crear_lista_pokemon(char* pokemons)
{
	t_list* lista_pokemon = list_create();

	char** array_pokemons = string_split(pokemons, "|");
	int cantidad_pokemons = 3; //TODO calcular length de vector cualquiera

	for(int i=0;i<cantidad_pokemons;i++)
		if(array_pokemons[i]!=NULL) agregar_pokemon(lista_pokemon, array_pokemons[i]);

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
	pokemon->especie = especie_pokemon;
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
}

//-----------PLANIFICACION-----------//
void pokemon_y_entrenador_mas_cercanos_entre_si(char* especie_pokemon, Pokemon_Mapa* pokemon, void* entrenador)
{
	bool es_especie_que_busco(void* pokemon) { return strcmp(((Pokemon_Mapa*) pokemon)->especie, especie_pokemon)==0; }

	t_list* pokemons_que_busco = list_filter(pokemons_mapa, &es_especie_que_busco);

	if(list_is_empty(pokemons_que_busco))
	{
		pokemon=NULL;
		entrenador=NULL;
	}
	else
	{
		pokemon = (Pokemon_Mapa*) pokemons_que_busco->head->data;
		entrenador = entrenador_mas_cercano(entrenadores_disponibles(), pokemon->posicion);
	}

	int distancia_entre(Posicion* posicion1, Posicion* posicion2) { return abs(posicion2->posX-posicion1->posX)+abs(posicion2->posY-posicion1->posY); }

	void pokemon_y_entrenador_mas_cercano_entre_dos(Pokemon_Mapa* pokemon_1, Pokemon_Mapa* pokemon_2, Pokemon_Mapa* pokemon_cercano, Entrenador* entrenador_cercano)
	{
		Entrenador* entrenador_1 = entrenador_mas_cercano(entrenadores_disponibles(), pokemon_1->posicion);
		Entrenador* entrenador_2 = entrenador_mas_cercano(entrenadores_disponibles(), pokemon_2->posicion);

		int distancia_1 = distancia_entre(pokemon_1->posicion, entrenador_1->posicion);
		int distancia_2 = distancia_entre(pokemon_2->posicion, entrenador_2->posicion);

		if(distancia_1>distancia_2)
		{
			pokemon_cercano = pokemon_2;
			entrenador_cercano = entrenador_2;
		}
		else
		{
			pokemon_cercano = pokemon_1;
			entrenador_cercano = entrenador_1;
		}
	}

	for(int i=1; i < pokemons_mapa->elements_count;i++)
		pokemon_y_entrenador_mas_cercano_entre_dos(pokemon, list_get(pokemons_mapa,i), pokemon, entrenador);

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



