#include "../Utils/protocolo.h"
#include <commons/collections/list.h>

extern t_list* pokemons_necesarios;
extern t_list* pokemons_localizados;
extern t_list* pokemons_mapa;

typedef struct
{
	char* especie;
	uint32_t cantidad;
}Pokemon;

typedef struct
{
	char* especie;
	Posicion* posicion;
}Pokemon_Mapa;


Pokemon* crear_pokemon(char* especie);
t_list* crear_lista_pokemon(char* pokemons);
void agregar_pokemon(t_list* lista_pokemon, char* especie_pokemon);
void destruir_pokemon(void* pokemon_void);
void identificar_pokemons_necesarios();
void agregar_pokemon_a_mapa(char* especie_pokemon, Posicion* posicion);
bool necesito_pokemon(char* pokemon);
bool esta_localizada(char* especie_pokemon);
void se_localizo(char* especie_pokemon);
