#include <commons/log.h>
#include <commons/config.h>
#include <commons/bitarray.h>
#include <commons/collections/list.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include "arbol.h"
#include "../Utils/socket.h"
#include "../Utils/hiloTimer.h"

t_log* logger;

void conectarse();
void reconexion();
void tallGrass_init(char* puntoMontaje);
void crearDirectorio(char* path);
void TerminarProgramaConError(char* error);
void TerminarPrograma(t_log* logger, t_config* config);
void EsperarHilos();
