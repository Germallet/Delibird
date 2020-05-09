#include "pokemon.h"
#include "entrenador.h"
#include <string.h>
#include <stdlib.h>
#include <commons/string.h>
#include <commons/collections/list.h>

t_list* pokemons_necesarios;
t_list* pokemons_localizados;
t_list* pokemons_mapa;

//-----------POKEMON-----------//
Pokemon* crear_pokemon(char* especie)
{
	Pokemon* pokemon = malloc(sizeof(Pokemon));

	pokemon->especie = especie;
	pokemon->cantidad = 1;

	return pokemon;
}

void destruir_pokemon(void* pokemon_void)
{
	Pokemon* pokemon = (Pokemon*) pokemon_void;
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

	return list_any_satisfy(pokemons_localizados, &es_misma_especie);
}

void se_localizo(char* especie_pokemon) { list_add(pokemons_localizados, especie_pokemon); }

//-----------TEAM-POKEMON-----------//
void identificar_pokemons_necesarios()
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

Pokemon_Mapa* pokemon_mas_cercano_a_entrenador(char* especie_pokemon)
{
	bool es_especie_que_busco(void* pokemon) { return strcmp(((Pokemon_Mapa*) pokemon)->especie, especie_pokemon)==0; }

	t_list* pokemons_que_busco = list_filter(pokemons_mapa, &es_especie_que_busco);

	if(list_is_empty(pokemons_que_busco))
			return NULL;

	if(pokemons_que_busco->elements_count==1)
		return (Pokemon_Mapa*) pokemons_que_busco->head->data;

	Pokemon_Mapa* pokemon = list_get(pokemons_que_busco, 0);

	int distancia_entre(Posicion* posicion1, Posicion* posicion2) { return abs(posicion2->posX-posicion1->posX)+abs(posicion2->posY-posicion1->posY); }

	Pokemon_Mapa* pokemon_mas_cercano_entre_dos(Pokemon_Mapa* pokemon_1, Pokemon_Mapa* pokemon_2)
	{
		int distancia_1 = distancia_entre(pokemon_1->posicion, entrenador_mas_cercano(entrenadores_disponibles(), pokemon_1->posicion)->posicion);
		int distancia_2 = distancia_entre(pokemon_2->posicion, entrenador_mas_cercano(entrenadores_disponibles(), pokemon_2->posicion)->posicion);

		return distancia_1>distancia_2 ? pokemon_2 : pokemon_1;
	}

	for(int i=1; i < pokemons_mapa->elements_count;i++) { pokemon = pokemon_mas_cercano_entre_dos(pokemon, list_get(pokemons_mapa,i)); }

	return pokemon;
}


bool necesito_pokemon(char* pokemon) { return tiene_pokemon(pokemons_necesarios, pokemon); }



