bool hay_entrenador_en_ejecucion();
bool hay_pokemons_para_atrapar();
bool hay_entrenadores_disponibles();

extern t_list* deadlocks;
extern int quantum;

typedef struct
{
	Entrenador* entrenador1;
	Entrenador* entrenador2;
	char* pokemon1;
	char* pokemon2;
}Deadlock;

void planificar_entrenador_si_es_necesario();
void destruir_interrupcion(void* interrupcion);
void identificar_deadlocks();
void terminar_team();
void reiniciar_quantum();
void aumentar_quantum();
bool necesitamos_pokemons();
bool es_planificacion_tipo(char* tipo);
