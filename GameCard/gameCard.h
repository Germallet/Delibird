#include <commons/log.h>
#include <commons/config.h>
#include <commons/bitarray.h>
#include <commons/collections/list.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include "../Utils/socket.h"
#include "../Utils/hiloTimer.h"

t_log* logger;

typedef struct {
	char directory;
	uint32_t size;
	uint32_t* blocks;
	char open;
} DatosArchivosPokemon;

void conectarse();
void reconexion();
void tallGrass_init();
void TerminarProgramaConError(char* error);
void TerminarPrograma(t_log* logger, t_config* config);
void EsperarHilos();
