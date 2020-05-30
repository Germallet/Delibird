#include <commons/log.h>
#include <commons/config.h>
#include <commons/collections/list.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include "../Utils/socket.h"

t_log* logger;

typedef struct{
	char directory;
} DatosDirectorio;

typedef struct {
	DatosDirectorio directory;
	uint32_t size;
	uint32_t* blocks;
	char open;
} DatosArchivosPokemon;

void informar(char* log);
void informarError(char* error);
void TerminarProgramaConError(char* error);
void TerminarPrograma(t_log* logger, t_config* config);
void EsperarHilos();
