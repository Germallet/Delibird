#include "posicion.h"

Posicion crear_posicion(char* string_posicion)
{
	Posicion posicion;
	posicion.posX = string_posicion[0]-'0';
	posicion.posY = string_posicion[2]-'0';
	return posicion;
}

int distancia_entre(Posicion posicion1, Posicion posicion2) { return abs(posicion2.posX-posicion1.posX)+abs(posicion2.posY-posicion1.posY); }

int distancia_menor(Posicion posicion, t_list* posiciones, int* indice)
{
	if(list_is_empty(posiciones)) return -1;

	int distancia = distancia_entre(posicion, *((Posicion*) list_get(posiciones, 0)));
	*indice = 0;
	for(int i=1; i<posiciones->elements_count;i++)
	{
		int nueva_distancia = distancia_entre(posicion, *((Posicion*) list_get(posiciones, i)));
		if(nueva_distancia<distancia)
		{
			distancia = nueva_distancia;
			*indice = i;
		}
	}
	return distancia;
}

bool es_misma_posicion(Posicion posicion_1, Posicion posicion_2) { return posicion_1.posX==posicion_2.posX && posicion_1.posY == posicion_2.posY; }
