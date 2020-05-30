#pragma once
#include "particion.h"

void DP_Inicializar();
Particion* DP_Seleccionar_FF(int tamanio);
Particion* DP_Seleccionar_BF(int tamanio);
void DP_Compactar();
void DP_Eliminar_FIFO();
void DP_Eliminar_LRU();
void DP_Dump();
void DP_Destruir();
