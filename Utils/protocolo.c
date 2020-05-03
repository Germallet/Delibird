#include "protocolo.h"
#include <stdlib.h>

//TODO EN LOS strlen TENEMOS QUE PONER -1 POR EL /0 ???
//TODO Agregar free()
//TODO cuando ger termine las colas hay que hacer la de suscriptor
//TODO HABRIA QUE VER COMO VERIFICAR SI TOODO SE COPIO BIEN

// FUNCIONES INDIVIDUALES PARA CADA SERIALIZAR
//2
void* Serializar_NEW_POKEMON(DATOS_NEW_POKEMON* datos, int* tamanioBuffer)
{
	*tamanioBuffer = datos->largoPokemon + sizeof(uint32_t)*4; //TODO VER CON GERMAN SI ESTA BIEN
	void* paqueteSerializado = malloc(*tamanioBuffer);

	int desplazamiento = 0;
	memcpy(paqueteSerializado, &(datos->largoPokemon), sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(paqueteSerializado + desplazamiento, &(datos->pokemon), datos->largoPokemon);
	desplazamiento += datos->largoPokemon;
	memcpy(paqueteSerializado + desplazamiento, &((datos->posicion).posX), sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(paqueteSerializado + desplazamiento, &((datos->posicion).posY), sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(paqueteSerializado + desplazamiento, &(datos->cantidad), sizeof(uint32_t));

	return paqueteSerializado;
}

//3
void* Serializar_APPEARED_POKEMON(DATOS_APPEARED_POKEMON* datos, int* tamanioBuffer)
{
	*tamanioBuffer = datos->largoPokemon + sizeof(uint32_t)*4;
	void* paqueteSerializado = malloc(*tamanioBuffer);

	int desplazamiento = 0;
	memcpy(paqueteSerializado, &(datos->largoPokemon), sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(paqueteSerializado + desplazamiento, &(datos->pokemon), datos->largoPokemon);
	desplazamiento += datos->largoPokemon;
	memcpy(paqueteSerializado + desplazamiento, &((datos->posicion).posX), sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(paqueteSerializado + desplazamiento, &((datos->posicion).posY), sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(paqueteSerializado + desplazamiento, &(datos->ID_MENSAJE), sizeof(uint32_t));

	return paqueteSerializado;
}

//4
void* Serializar_CATCH_POKEMON(DATOS_CATCH_POKEMON* datos, int* tamanioBuffer)
{
	*tamanioBuffer = datos->largoPokemon + sizeof(uint32_t)*3;
	void* paqueteSerializado = malloc(*tamanioBuffer);

	int desplazamiento = 0;
	memcpy(paqueteSerializado, &(datos->largoPokemon), sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(paqueteSerializado+desplazamiento, &(datos->pokemon), datos->largoPokemon);
	desplazamiento += datos->largoPokemon;
	memcpy(paqueteSerializado + desplazamiento, &((datos->posicion).posX), sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(paqueteSerializado + desplazamiento, &((datos->posicion).posY), sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	return paqueteSerializado;
}

//5
void* Serializar_CAUGHT_POKEMON(DATOS_CAUGHT_POKEMON* datos, int* tamanioBuffer)
{
	*tamanioBuffer = sizeof(uint32_t)*2;
	void* paqueteSerializado = malloc(*tamanioBuffer);

	int desplazamiento = 0;
	memcpy(paqueteSerializado, &(datos->ID_MENSAJE), sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(paqueteSerializado + desplazamiento, &(datos->capturado), sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	return paqueteSerializado;
}

//6
void* Serializar_GET_POKEMON(DATOS_GET_POKEMON* datos, int* tamanioBuffer)
{
	*tamanioBuffer = datos->largoPokemon + sizeof(uint32_t);
	void* paqueteSerializado = malloc(*tamanioBuffer);

	int desplazamiento = 0;
	memcpy(paqueteSerializado, &(datos->largoPokemon), sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(paqueteSerializado+desplazamiento, datos->pokemon, datos->largoPokemon);

	return paqueteSerializado;
}

//7 TODO serializar localized pokemon hacer
void* Serializar_LOCALIZED_POKEMON(DATOS_LOCALIZED_POKEMON* datos, int* tamanioBuffer){

	*tamanioBuffer = datos->largoPokemon + sizeof(uint32_t) + (datos->cantidad)*(sizeof(uint32_t)*2);
	void* paqueteSerializado = malloc(*tamanioBuffer);
/*
	int desplazamiento = 0;
	memcpy(paqueteSerializado, &(datos->ID_MENSAJE), sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(paqueteSerializado, &(datos->largoPokemon), sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(paqueteSerializado+desplazamiento, &(datos->pokemon), datos->largoPokemon);
	desplazamiento += datos->largoPokemon;
	memcpy(paqueteSerializado + desplazamiento, &(datos->cantidad), sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	for (int i = 0; i < datos->cantidad; i++){
		memcpy(paqueteSerializado + desplazamiento, &(datos->posiciones->posX), sizeof(uint32_t));
		desplazamiento += sizeof(uint32_t);
		memcpy(paqueteSerializado + desplazamiento, &(datos->posiciones->posY), sizeof(uint32_t));
		desplazamiento += sizeof(uint32_t);
	}
*/
	return paqueteSerializado;
}


// FUNCION DESERIALIZAR GENERAL, DEVUELVE -1 SI NO ES UN MENSAJE CORRECTO
/*
bool Deserializar(int socket, void* datos) {

	CodigoDeOperacion* codigoDeOperacion = NULL;

	int verif = recv(socket,codigoDeOperacion,sizeof(codigoDeOperacion),0);

	if (verif != -1) {
		if (*codigoDeOperacion == NEW_POKEMON) {
			return Deserializar_NEW_POKEMON(socket,datos);
		} else if (*codigoDeOperacion == APPEARED_POKEMON) {
			return Deserializar_APPEARED_POKEMON(socket,datos);
		} else if (*codigoDeOperacion == CATCH_POKEMON) {
			return Deserializar_CATCH_POKEMON(socket,datos);
		} else if (*codigoDeOperacion == CAUGHT_POKEMON) {
			return Deserializar_CAUGHT_POKEMON(socket,datos);
		} else if (*codigoDeOperacion == GET_POKEMON) {
			return Deserializar_GET_POKEMON(socket,datos);
		} else if (*codigoDeOperacion == LOCALIZED_POKEMON) {
			return Deserializar_LOCALIZED_POKEMON(socket,datos);
		}
	}
	return false;
}
*/

// FUNCIONES INDIVIDUALES PARA CADA DESSERIALIZAR SIN VERIFICACION
//2
bool Deserializar_NEW_POKEMON(int socket,DATOS_NEW_POKEMON* datos) {

	datos = malloc(sizeof(DATOS_NEW_POKEMON));

	uint32_t verificador = 0;

	verificador += recv(socket,&(datos->largoPokemon),sizeof(uint32_t),0);
	verificador += recv(socket,datos->pokemon,datos->largoPokemon,0);
	verificador += recv(socket,&((datos->posicion).posX),sizeof(uint32_t),0);
	verificador += recv(socket,&((datos->posicion).posY),sizeof(uint32_t),0);
	verificador += recv(socket,&(datos->cantidad),sizeof(uint32_t),0);

	return verificador == sizeof(DATOS_NEW_POKEMON);
}

//3
bool Deserializar_APPEARED_POKEMON(int socket,DATOS_APPEARED_POKEMON* datos) {

	datos = malloc(sizeof(DATOS_APPEARED_POKEMON));

	uint32_t verificador = 0;

	verificador += recv(socket,&(datos->largoPokemon),sizeof(uint32_t),0);
	verificador += recv(socket,datos->pokemon,datos->largoPokemon,0);
	verificador += recv(socket,&((datos->posicion).posX),sizeof(uint32_t),0);
	verificador += recv(socket,&((datos->posicion).posY),sizeof(uint32_t),0);
	verificador += recv(socket,&(datos->ID_MENSAJE),sizeof(uint32_t),0);

	return verificador == sizeof(DATOS_APPEARED_POKEMON);
}

//4
bool Deserializar_CATCH_POKEMON(int socket,DATOS_CATCH_POKEMON* datos) {

	datos = malloc(sizeof(DATOS_CATCH_POKEMON));

	uint32_t verificador = 0;

	verificador += recv(socket,&(datos->largoPokemon),sizeof(uint32_t),0);
	verificador += recv(socket,datos->pokemon,datos->largoPokemon,0);
	verificador += recv(socket,&((datos->posicion).posX),sizeof(uint32_t),0);
	verificador += recv(socket,&((datos->posicion).posY),sizeof(uint32_t),0);

	return verificador == sizeof(DATOS_CATCH_POKEMON);
}

//5
bool Deserializar_CAUGHT_POKEMON(int socket,DATOS_CAUGHT_POKEMON* datos) {

	datos = malloc(sizeof(DATOS_CAUGHT_POKEMON));

	uint32_t verificador = 0;

	verificador += recv(socket,&(datos->ID_MENSAJE),sizeof(uint32_t),0);
	verificador += recv(socket,&(datos->capturado),sizeof(uint32_t),0);

	return verificador == sizeof(DATOS_CAUGHT_POKEMON);
}

//6
/*bool Deserializar_GET_POKEMON(int socket,DATOS_GET_POKEMON* datos) {

	datos = malloc(sizeof(DATOS_GET_POKEMON));

	uint32_t verificador = 0;

	verificador += recv(socket,&(datos->largoPokemon),sizeof(uint32_t),0);
	verificador += recv(socket,datos->pokemon,datos->largoPokemon,0);

	return verificador == sizeof(DATOS_GET_POKEMON);
}*/
bool Deserializar_GET_POKEMON(Paquete* paquete, DATOS_GET_POKEMON* datos)
{
	if (!Paquete_Deserializar(paquete, &(datos->largoPokemon), sizeof(uint32_t))) return false;
	datos->pokemon = malloc(datos->largoPokemon+1);
	if (!Paquete_Deserializar(paquete, datos->pokemon, datos->largoPokemon)) return false;
	(datos->pokemon)[datos->largoPokemon] = '\0';
	return true;
}

//7
bool Deserializar_LOCALIZED_POKEMON(int socket,DATOS_LOCALIZED_POKEMON* datos) {
/*
	datos = malloc(sizeof(DATOS_LOCALIZED_POKEMON));

	recv(socket,datos->largoPokemon,sizeof(uint32_t),0);
 	recv(socket,datos->pokemon,datos->largoPokemon,0);
	recv(socket,datos->posicion->posX,sizeof(datos->posicion->posX),0);
	recv(socket,datos->posicion->posY,sizeof(datos->posicion->posY),0);
	recv(socket,datos->ID_MENSAJE,sizeof(datos->ID_MENSAJE),0);
*/
	return true;
}

// FUNCIONES QUE DEVUELVEN EL TAMANIO DE LOS MENSAJES
/*
uint32_t size_NEW_POKEMON(DATOS_NEW_POKEMON* datos) {
	return datos->largoPokemon + sizeof(uint32_t)*4;
}

uint32_t size_APPEARED_POKEMON(DATOS_APPEARED_POKEMON* datos) {
	return datos->largoPokemon + sizeof(uint32_t)*4;
}

uint32_t size_GET_POKEMON(DATOS_GET_POKEMON* datos) {
	return datos->largoPokemon + sizeof(uint32_t);
}

uint32_t size_CATCH_POKEMON(DATOS_CATCH_POKEMON* datos) {
	return datos->largoPokemon + sizeof(uint32_t)*3;
}

uint32_t size_CAUGHT_POKEMON(DATOS_CAUGHT_POKEMON* datos) {
	return sizeof(uint32_t)*2;
}
*/

void* Serializar_BROKER_RECONECTAR(Broker_DATOS_RECONECTAR* datos, int* tamanioBuffer)
{
	*tamanioBuffer = sizeof(Broker_DATOS_RECONECTAR);
	void* stream = malloc(*tamanioBuffer);

	int desplazamiento = 0;
	memcpy(stream, &(datos->id), sizeof(datos->id));
	desplazamiento += sizeof(datos->id);

	return stream;
}
bool Deserializar_BROKER_RECONECTAR(int socket, Broker_DATOS_RECONECTAR* datos)
{
	datos = malloc(sizeof(Broker_DATOS_RECONECTAR));

	uint32_t verificador = 0;

	verificador += recv(socket, &(datos->id), sizeof(datos->id), 0);

	return verificador == sizeof(Broker_DATOS_RECONECTAR);
}

void* Serializar_BROKER_CONECTADO(Broker_DATOS_RECONECTAR* datos, int* tamanioBuffer)
{
	return Serializar_BROKER_RECONECTAR(datos, tamanioBuffer);
}
bool Deserializar_BROKER_CONECTADO(int socket, Broker_DATOS_RECONECTAR* datos)
{
	return Deserializar_BROKER_RECONECTAR(socket, datos);
}
