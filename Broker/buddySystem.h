#pragma once
#include "particion.h"

void BS_Inicializar(int tamanio);
Particion* BS_Seleccionar(int tamanio);
void BS_Eliminar_FIFO();
void BS_Eliminar_LRU();
void BS_Dump(FILE* archivo);
void BS_Destruir();
