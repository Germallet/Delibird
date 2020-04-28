#include "protocolo.h"
#include <stdlib.h>

/* PREGUNTAS
 * 1) EN LOS strlen TENEMOS QUE PONER -1 POR EL /0 ???
 *
 */


// FUNCIONES INDIVIDUALES PARA CADA SERIALIZAR

//2
void* Serializar_NEW_POKEMON(DATOS_NEW_POKEMON* datos, int* tamanioBuffer)
{
	*tamanioBuffer = strlen(datos->pokemon) + sizeof(uint32_t)*4;
	void* paqueteSerializado = malloc(*tamanioBuffer);

	int desplazamiento = 0;
	memcpy(paqueteSerializado, &(datos->largoPokemon), sizeof(datos->largoPokemon));
	desplazamiento += sizeof(datos->largoPokemon);
	memcpy(paqueteSerializado + desplazamiento, &(datos->pokemon), strlen(datos->pokemon));
	desplazamiento += strlen(datos->pokemon);
	memcpy(paqueteSerializado + desplazamiento, &((datos->posicion).posX), sizeof((datos->posicion).posX));
	desplazamiento += sizeof((datos->posicion).posX);
	memcpy(paqueteSerializado + desplazamiento, &((datos->posicion).posY), sizeof((datos->posicion).posY));
	desplazamiento += sizeof((datos->posicion).posY);
	memcpy(paqueteSerializado + desplazamiento, &(datos->cantidad), sizeof(datos->cantidad));

	return paqueteSerializado;
}

//3
void* Serializar_APPEARED_POKEMON(DATOS_APPEARED_POKEMON* datos, int* tamanioBuffer)
{
	*tamanioBuffer = strlen(datos->pokemon) + sizeof(uint32_t)*3;
	void* paqueteSerializado = malloc(*tamanioBuffer);

	int desplazamiento = 0;
		memcpy(paqueteSerializado, &(datos->largoPokemon), sizeof(datos->largoPokemon));
		desplazamiento += sizeof(datos->largoPokemon);
		memcpy(paqueteSerializado + desplazamiento, &(datos->pokemon), strlen(datos->pokemon));
		desplazamiento += strlen(datos->pokemon);
		memcpy(paqueteSerializado + desplazamiento, &((datos->posicion).posX), sizeof((datos->posicion).posX));
		desplazamiento += sizeof((datos->posicion).posX);
		memcpy(paqueteSerializado + desplazamiento, &((datos->posicion).posY), sizeof((datos->posicion).posY));
		desplazamiento += sizeof((datos->posicion).posY);
		memcpy(paqueteSerializado + desplazamiento, &(datos->ID_MENSAJE), sizeof(datos->ID_MENSAJE));

	return paqueteSerializado;
}

//4
void* Serializar_CATCH_POKEMON(DATOS_CATCH_POKEMON* datos, int* tamanioBuffer)
{
	*tamanioBuffer = strlen(datos->pokemon) + sizeof(uint32_t)*2;
	void* paqueteSerializado = malloc(*tamanioBuffer);

	int desplazamiento = 0;
	memcpy(paqueteSerializado, &(datos->largoPokemon), sizeof(datos->largoPokemon));
	desplazamiento += sizeof(datos->largoPokemon);
	memcpy(paqueteSerializado+desplazamiento, &(datos->pokemon), strlen(datos->pokemon));
	desplazamiento += strlen(datos->pokemon);
	memcpy(paqueteSerializado + desplazamiento, &((datos->posicion).posX), sizeof((datos->posicion).posX));
	desplazamiento += sizeof((datos->posicion).posX);
	memcpy(paqueteSerializado + desplazamiento, &((datos->posicion).posY), sizeof((datos->posicion).posY));
	desplazamiento += sizeof((datos->posicion).posY);

	return paqueteSerializado;
}

//5
void* Serializar_CAUGHT_POKEMON(DATOS_CAUGHT_POKEMON* datos, int* tamanioBuffer)
{
	*tamanioBuffer = sizeof(uint32_t)*2;
	void* paqueteSerializado = malloc(*tamanioBuffer);

	int desplazamiento = 0;
	memcpy(paqueteSerializado, &(datos->ID_MENSAJE), sizeof(datos->ID_MENSAJE));
	desplazamiento += sizeof(datos->ID_MENSAJE);
	memcpy(paqueteSerializado + desplazamiento, &(datos->capturado), sizeof(datos->capturado));
	desplazamiento += sizeof(datos->capturado);

	return paqueteSerializado;
}

//6
void* Serializar_GET_POKEMON(DATOS_GET_POKEMON* datos, int* tamanioBuffer)
{
	*tamanioBuffer = strlen(datos->pokemon);
	void* paqueteSerializado = malloc(*tamanioBuffer);

	int desplazamiento = 0;
	memcpy(paqueteSerializado, &(datos->largoPokemon), sizeof(datos->largoPokemon));
	desplazamiento += sizeof(datos->largoPokemon);
	memcpy(paqueteSerializado+desplazamiento, &(datos->pokemon), strlen(datos->pokemon));

	return paqueteSerializado;
}

////7
//void* Serializar_LOCALIZED_POKEMON(DATOS_LOCALIZED_POKEMON* datos, int* tamanioBuffer)
//{
//	*tamanioBuffer = ALGO NO SE;
//	void* paqueteSerializado = malloc(*tamanioBuffer);
//
//	int desplazamiento = 0;
//	memcpy(paqueteSerializado + desplazamiento, &(datos->nombre), sizeof(datos->nombre));
//	desplazamiento += sizeof(datos->nombre);
//	memcpy(paqueteSerializado + desplazamiento, &(datos->posicion->posX), sizeof(datos->posicion->posX));
//	desplazamiento += sizeof(datos->posicion->posX);
//	memcpy(paqueteSerializado + desplazamiento, &(datos->posicion->posY), sizeof(datos->posicion->posY));
//	desplazamiento += sizeof(datos->posicion->posY);
//	memcpy(paqueteSerializado + desplazamiento, &(datos->ID_MENSAJE), sizeof(datos->ID_MENSAJE));
//
//	return paqueteSerializado;
//}

// FUNCION DESERIALIZAR GENERAL, DEVUELVE -1 SI NO ES UN MENSAJE CORRECTO
void* Deserializar(int socket) {

	CodigoDeOperacion codigoDeOperacion;

	int verif = recv(socket,codigoDeOperacion,sizeof(codigoDeOperacion),0);

	if (verif != -1) {
		if (codigoDeOperacion == NEW_POKEMON) {
			return Deserializar_NEW_POKEMON(socket);
		} else if (codigoDeOperacion == APPEARED_POKEMON) {
			return Deserializar_APPEARED_POKEMON(socket);
		} else if (codigoDeOperacion == CATCH_POKEMON) {
			return Deserializar_CATCH_POKEMON(socket);
		} else if (codigoDeOperacion == CAUGHT_POKEMON) {
			return Deserializar_CAUGHT_POKEMON(socket);
		} else if (codigoDeOperacion == GET_POKEMON) {
			return Deserializar_GET_POKEMON(socket);
		} else if (codigoDeOperacion == LOCALIZED_POKEMON) {
			return Deserializar_LOCALIZED_POKEMON(socket);
		}
	}
	return -1;
}

// FUNCIONES INDIVIDUALES PARA CADA SERIALIZAR

// LE FALTAN LAS VERIFICACIONES DE LOS recv

//2
DATOS_NEW_POKEMON* Deserializar_NEW_POKEMON(int socket) {

	DATOS_NEW_POKEMON* datos = malloc(sizeof(DATOS_NEW_POKEMON));

	recv(socket,datos->nombre,strlen(datos->nombre),0);
	recv(socket,datos->posicion->posX,sizeof(datos->posicion->posX),0);
	recv(socket,datos->posicion->posY,sizeof(datos->posicion->posY),0);
	recv(socket,datos->cantidad,sizeof(datos->cantidad),0);

	return datos;
}

//3
DATOS_APPEARED_POKEMON* Deserializar_APPEARED_POKEMON(int socket) {

	DATOS_APPEARED_POKEMON* datos = malloc(sizeof(DATOS_APPEARED_POKEMON));

	recv(socket,datos->nombre,strlen(datos->nombre),0);
	recv(socket,datos->posicion->posX,sizeof(datos->posicion->posX),0);
	recv(socket,datos->posicion->posY,sizeof(datos->posicion->posY),0);
	recv(socket,datos->ID_MENSAJE,sizeof(datos->ID_MENSAJE),0);

	return datos;
}

//4
DATOS_CATCH_POKEMON* Deserializar_CATCH_POKEMON(int socket) {

	DATOS_CATCH_POKEMON* datos = malloc(sizeof(DATOS_CATCH_POKEMON));

	recv(socket,datos->nombre,strlen(datos->nombre),0);
	recv(socket,datos->posicion->posX,sizeof(datos->posicion->posX),0);
	recv(socket,datos->posicion->posY,sizeof(datos->posicion->posY),0);

	return datos;
}

//5
DATOS_CAUGHT_POKEMON* Deserializar_CAUGHT_POKEMON(int socket) {

	DATOS_CAUGHT_POKEMON* datos = malloc(sizeof(DATOS_CAUGHT_POKEMON));

	recv(socket,datos->ID_MENSAJE,sizeof(datos->ID_MENSAJE),0);
	recv(socket,datos->capturado,sizeof(datos->capturado),0);

	return datos;
}

//6
DATOS_GET_POKEMON* Deserializar_GET_POKEMON(int socket) {

	DATOS_GET_POKEMON* datos = malloc(sizeof(DATOS_GET_POKEMON));

	recv(socket,datos->nombre,strlen(datos->nombre),0);

	return datos;
}

//7
//DATOS_LOCALIZED_POKEMON* Deserializar_LOCALIZED_POKEMON(int socket) {
//
//	DATOS_LOCALIZED_POKEMON* datos = malloc(sizeof(DATOS_LOCALIZED_POKEMON));
//
//	recv(socket,datos->nombre,strlen(datos->nombre),0);
//	recv(socket,datos->posicion->posX,sizeof(datos->posicion->posX),0);
//	recv(socket,datos->posicion->posY,sizeof(datos->posicion->posY),0);
//	recv(socket,datos->ID_MENSAJE,sizeof(datos->ID_MENSAJE),0);
//
//	return datos;
//}

