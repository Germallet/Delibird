#include "protocolo.h"
#include <stdlib.h>

DATOS_APPEARED_POKEMON* Deserializar_APPEARED_POKEMON()
{
	DATOS_APPEARED_POKEMON* datos = malloc(sizeof(DATOS_APPEARED_POKEMON));
	return datos;
}
void* Serializar_APPEARED_POKEMON(DATOS_APPEARED_POKEMON* datos, int* tamanioBuffer)
{
	*tamanioBuffer = Paquete_Tamanio(datos);
	void* paqueteSerializado = malloc(*tamanioBuffer);

	int desplazamiento = 0;
	memcpy(paqueteSerializado, APPEARED_POKEMON, sizeof(APPEARED_POKEMON));
	desplazamiento += sizeof(APPEARED_POKEMON);
	memcpy(paqueteSerializado + desplazamiento, &(datos->nombre), sizeof(datos->nombre));
	desplazamiento += sizeof(datos->nombre);
	memcpy(paqueteSerializado + desplazamiento, &(datos->posicion->posX), sizeof(datos->posicion->posX));
	desplazamiento += sizeof(datos->posicion->posX);
	memcpy(paqueteSerializado + desplazamiento, &(datos->posicion->posY), sizeof(datos->posicion->posY));
	desplazamiento += sizeof(datos->posicion->posY);
	memcpy(paqueteSerializado + desplazamiento, &(datos->ID_MENSAJE), sizeof(datos->ID_MENSAJE));

	return paqueteSerializado;
}


void* Serializar_NEW_POKEMON(DATOS_NEW_POKEMON* datos, int* tamanioBuffer)
{
	*tamanioBuffer = Paquete_Tamanio(datos);
	void* paqueteSerializado = malloc(*tamanioBuffer);

	int desplazamiento = 0;
	memcpy(paqueteSerializado, NEW_POKEMON, sizeof(NEW_POKEMON));
	desplazamiento += sizeof(NEW_POKEMON);
	memcpy(paqueteSerializado + desplazamiento, &(datos->nombre), sizeof(datos->nombre));
	desplazamiento += sizeof(datos->nombre);
	memcpy(paqueteSerializado + desplazamiento, &(datos->posicion->posX), sizeof(datos->posicion->posX));
	desplazamiento += sizeof(datos->posicion->posX);
	memcpy(paqueteSerializado + desplazamiento, &(datos->posicion->posY), sizeof(datos->posicion->posY));
	desplazamiento += sizeof(datos->posicion->posY);
	memcpy(paqueteSerializado + desplazamiento, &(datos->cantidad), sizeof(datos->cantidad));

	return paqueteSerializado;
}

void* Serializar_GET_POKEMON(DATOS_GET_POKEMON* datos, int* tamanioBuffer)
{
	*tamanioBuffer = Paquete_Tamanio(datos);
	void* paqueteSerializado = malloc(*tamanioBuffer);

	int desplazamiento = 0;
	memcpy(paqueteSerializado, GET_POKEMON, sizeof(GET_POKEMON));
	desplazamiento += sizeof(GET_POKEMON);
	memcpy(paqueteSerializado + desplazamiento, &(datos->nombre), sizeof(datos->nombre));

	return paqueteSerializado;
}

void* Serializar_CATCH_POKEMON(DATOS_CATCH_POKEMON* datos, int* tamanioBuffer)
{
	*tamanioBuffer = Paquete_Tamanio(datos);
	void* paqueteSerializado = malloc(*tamanioBuffer);

	int desplazamiento = 0;
	memcpy(paqueteSerializado, CATCH_POKEMON, sizeof(CATCH_POKEMON));
	desplazamiento += sizeof(CATCH_POKEMON);
	memcpy(paqueteSerializado + desplazamiento, &(datos->nombre), sizeof(datos->nombre));
	desplazamiento += sizeof(datos->nombre);
	memcpy(paqueteSerializado + desplazamiento, &(datos->posicion->posX), sizeof(datos->posicion->posX));
	desplazamiento += sizeof(datos->posicion->posX);
	memcpy(paqueteSerializado + desplazamiento, &(datos->posicion->posY), sizeof(datos->posicion->posY));

	return paqueteSerializado;
}

void* Serializar_CAUGHT_POKEMON(DATOS_CAUGHT_POKEMON* datos, int* tamanioBuffer)
{
	*tamanioBuffer = Paquete_Tamanio(datos);
	void* paqueteSerializado = malloc(*tamanioBuffer);

	int desplazamiento = 0;
	memcpy(paqueteSerializado, CAUGHT_POKEMON, sizeof(CAUGHT_POKEMON));
	desplazamiento += sizeof(CAUGHT_POKEMON);
	memcpy(paqueteSerializado + desplazamiento, &(datos->ID_MENSAJE), sizeof(datos->ID_MENSAJE));
	desplazamiento += sizeof(datos->ID_MENSAJE);
	memcpy(paqueteSerializado + desplazamiento, &(datos->capturado), sizeof(datos->capturado));
	desplazamiento += sizeof(datos->capturado);

	return paqueteSerializado;
}

