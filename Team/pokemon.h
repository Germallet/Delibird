#include "../Utils/protocolo.h"
#include <commons/collections/list.h>

extern t_list* pokemons_necesarios;
extern t_list* especies_localizadas;
extern t_list* pokemons_mapa;

typedef struct
{
	char* especie;
	uint32_t cantidad;
}Pokemon;

typedef struct
{
	char* especie;
	Posicion posicion;
}Pokemon_Mapa;

Pokemon* crear_pokemon(char* especie);
t_list* crear_lista_pokemon(char* pokemons);
Pokemon* tomar_pokemon(t_list* lista_pokemon, char* especie_pokemon);
Pokemon* tomar_pokemon_sin_destruir(t_list* lista_pokemon, char* especie_pokemon);
void agregar_pokemon(t_list* lista_pokemon, char* especie_pokemon);
void destruir_pokemon(void* pokemon_void);
void destruir_pokemon_mapa(void* pokemon_void);
void identificar_objetivo_global();
char* una_especie_que_necesito_y_esta_en_mapa();
void se_asigno_para_capturar(Pokemon_Mapa* pokemon);
void pokemon_y_entrenador_mas_cercanos_entre_si(Pokemon_Mapa** pokemon, void** entrenador); // Donde void* es Entrenador*
void agregar_pokemon_a_mapa(char* especie_pokemon, Posicion posicion);
int cantidad_de_pokemons_en_lista(t_list* lista_pokemon);
int cantidad_de_pokemon_en_lista(t_list* lista_pokemon, char* especie_pokemon);
bool tiene_pokemon(t_list* lista_pokemon, char* especie_pokemon);
bool necesito_especie_pokemon(char* especie);
bool necesito_pokemon_mapa(Pokemon_Mapa* pokemon);
bool esta_localizada(char* especie_pokemon);
void se_localizo(char* especie_pokemon);
