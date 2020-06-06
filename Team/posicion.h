#include "../Utils/protocolo.h"
#include <commons/collections/list.h>

Posicion* crear_posicion(char* string_posicion);
int distancia_menor(Posicion* posicion, t_list* posiciones, int* indice);
bool es_misma_posicion(Posicion* posicion_1, Posicion* posicion_2);
