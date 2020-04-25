#include "protocolo.h"
#include <stdlib.h>

DATOS_APPEARED_POKEMON* Deserializar_APPEARED_POKEMON()
{
	DATOS_APPEARED_POKEMON* datos = malloc(sizeof(DATOS_APPEARED_POKEMON));
	return datos;
}
void* Serializar_APPEARED_POKEMON(DATOS_APPEARED_POKEMON* datos, int* tamanioBuffer)
{
	return NULL;
}
