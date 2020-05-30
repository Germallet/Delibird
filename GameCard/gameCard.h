#include <commons/log.h>
#include <commons/config.h>
#include <commons/collections/list.h>
#include <stdbool.h>
#include <stdlib.h>
#include <fuse.h>
#include <stdint.h>
#include "../Utils/socket.h"
#include "mensajes.h"
//#include "../Utils/protocolo.h"

typedef struct{
	char directory;
} DatosDirectorio;

typedef struct {
	DatosDirectorio directory;
	uint32_t size;
	uint32_t* blocks;
	char open;
}DatosArchivosPokemon;

void TerminarProgramaConError(char* error);
void TerminarPrograma(t_log* logger, t_config* config);
