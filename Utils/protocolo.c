#include "protocolo.h"
#include <stdlib.h>

void* Serializar_ID_MENSAJE(DATOS_ID_MENSAJE* datos, int* tamanioBuffer)
{
	*tamanioBuffer = sizeof(DATOS_ID_MENSAJE);
	void* buffer = malloc(*tamanioBuffer);

	int desplazamiento = 0;
	memcpy(buffer + desplazamiento, &(datos->id), sizeof(datos->id));

	return buffer;
}

// FUNCIONES INDIVIDUALES PARA CADA SERIALIZAR
//2
void* Serializar_NEW_POKEMON(DATOS_NEW_POKEMON* datos, int* tamanioBuffer)
{
	*tamanioBuffer = datos->largoPokemon + sizeof(uint32_t)*4;
	void* buffer = malloc(*tamanioBuffer);

	int desplazamiento = 0;
	memcpy(buffer, &(datos->largoPokemon), sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(buffer + desplazamiento, datos->pokemon, datos->largoPokemon);
	desplazamiento += datos->largoPokemon;
	memcpy(buffer + desplazamiento, &((datos->posicion).posX), sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(buffer + desplazamiento, &((datos->posicion).posY), sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(buffer + desplazamiento, &(datos->cantidad), sizeof(uint32_t));

	return buffer;
}

void* Serializar_NEW_POKEMON_ID(DATOS_NEW_POKEMON_ID* datos, int* tamanioBuffer)
{
	*tamanioBuffer = datos->datos.largoPokemon + sizeof(uint32_t)*5;
	void* buffer = malloc(*tamanioBuffer);

	int desplazamiento = 0;
	memcpy(buffer, &(datos->datos.largoPokemon), sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(buffer + desplazamiento, datos->datos.pokemon, datos->datos.largoPokemon);
	desplazamiento += datos->datos.largoPokemon;
	memcpy(buffer + desplazamiento, &(datos->datos.posicion.posX), sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(buffer + desplazamiento, &(datos->datos.posicion.posY), sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(buffer + desplazamiento, &(datos->datos.cantidad), sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(buffer + desplazamiento, &(datos->id), sizeof(uint32_t));

	return buffer;
}

//3
void* Serializar_APPEARED_POKEMON_ID(DATOS_APPEARED_POKEMON_ID* datos, int* tamanioBuffer)
{
	*tamanioBuffer = datos->datos.largoPokemon + sizeof(uint32_t)*5;
	void* buffer = malloc(*tamanioBuffer);

	int desplazamiento = 0;
	memcpy(buffer, &(datos->datos.largoPokemon), sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(buffer + desplazamiento, datos->datos.pokemon, datos->datos.largoPokemon);
	desplazamiento += datos->datos.largoPokemon;
	memcpy(buffer + desplazamiento, &(datos->datos.posicion.posX), sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(buffer + desplazamiento, &(datos->datos.posicion.posY), sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(buffer + desplazamiento, &(datos->id), sizeof(uint32_t));

	return buffer;
}

//4
void* Serializar_CATCH_POKEMON(DATOS_CATCH_POKEMON* datos, int* tamanioBuffer)
{
	*tamanioBuffer = datos->largoPokemon + sizeof(uint32_t)*3;
	void* buffer = malloc(*tamanioBuffer);

	int desplazamiento = 0;
	memcpy(buffer, &(datos->largoPokemon), sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(buffer + desplazamiento, datos->pokemon, datos->largoPokemon);
	desplazamiento += datos->largoPokemon;
	memcpy(buffer + desplazamiento, &((datos->posicion).posX), sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(buffer + desplazamiento, &((datos->posicion).posY), sizeof(uint32_t));

	return buffer;
}

void* Serializar_CATCH_POKEMON_ID(DATOS_CATCH_POKEMON_ID* datos, int* tamanioBuffer)
{
	*tamanioBuffer = datos->datos.largoPokemon + sizeof(uint32_t)*4;
	void* buffer = malloc(*tamanioBuffer);

	int desplazamiento = 0;
	memcpy(buffer, &(datos->datos.largoPokemon), sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(buffer + desplazamiento, datos->datos.pokemon, datos->datos.largoPokemon);
	desplazamiento += datos->datos.largoPokemon;
	memcpy(buffer + desplazamiento, &(datos->datos.posicion.posX), sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(buffer + desplazamiento, &(datos->datos.posicion.posY), sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(buffer + desplazamiento, &(datos->id), sizeof(uint32_t));

	return buffer;
}

//5
void* Serializar_CAUGHT_POKEMON_ID(DATOS_CAUGHT_POKEMON_ID* datos, int* tamanioBuffer)
{
	*tamanioBuffer = sizeof(uint32_t)*2;
	void* buffer = malloc(*tamanioBuffer);

	int desplazamiento = 0;
	memcpy(buffer + desplazamiento, &(datos->id), sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(buffer + desplazamiento, &(datos->datos.capturado), sizeof(uint32_t));

	return buffer;
}

//6
void* Serializar_GET_POKEMON(DATOS_GET_POKEMON* datos, int* tamanioBuffer)
{
	*tamanioBuffer = datos->largoPokemon + sizeof(uint32_t);
	void* buffer = malloc(*tamanioBuffer);

	int desplazamiento = 0;
	memcpy(buffer, &(datos->largoPokemon), sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(buffer+desplazamiento, datos->pokemon, datos->largoPokemon);

	return buffer;
}

//7
void* Serializar_LOCALIZED_POKEMON(DATOS_LOCALIZED_POKEMON* datos, int* tamanioBuffer){

	*tamanioBuffer = datos->largoPokemon + sizeof(uint32_t) + (datos->cantidad)*(sizeof(uint32_t)*2);
	void* buffer = malloc(*tamanioBuffer);

	int desplazamiento = 0;
	memcpy(buffer, &(datos->largoPokemon), sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(buffer + desplazamiento, datos->pokemon, datos->largoPokemon);
	desplazamiento += datos->largoPokemon;
	memcpy(buffer + desplazamiento, &(datos->cantidad), sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	for (int i = 0; i < datos->cantidad; i++) {
		memcpy(buffer + desplazamiento, &((datos->posiciones[i]).posX), sizeof(uint32_t));
		desplazamiento += sizeof(uint32_t);
		memcpy(buffer + desplazamiento, &((datos->posiciones[i]).posY), sizeof(uint32_t));
		desplazamiento += sizeof(uint32_t);
	}

	return buffer;
}

bool Deserializar_ID_MENSAJE(Paquete* paquete, DATOS_ID_MENSAJE* datos)
{
	if (!Paquete_Deserializar(paquete, &(datos->id), sizeof(uint32_t))) return false;
	return true;
}

// FUNCIONES INDIVIDUALES PARA CADA DESERIALIZAR
//2
bool Deserializar_NEW_POKEMON(Paquete* paquete, DATOS_NEW_POKEMON* datos)
{
	if (!Paquete_Deserializar(paquete, &(datos->largoPokemon), sizeof(uint32_t))) return false;
	if (!Paquete_DeserializarString(paquete, &(datos->pokemon), datos->largoPokemon)) return false;
	if (!Paquete_Deserializar(paquete, &((datos->posicion).posX), sizeof(uint32_t))) return false;
	if (!Paquete_Deserializar(paquete, &((datos->posicion).posY), sizeof(uint32_t))) return false;
	if (!Paquete_Deserializar(paquete, &(datos->cantidad), sizeof(uint32_t))) return false;
	return true;
}

//3
bool Deserializar_APPEARED_POKEMON(Paquete* paquete, DATOS_APPEARED_POKEMON* datos)
{
	if (!Paquete_Deserializar(paquete, &(datos->largoPokemon), sizeof(uint32_t))) return false;
	if (!Paquete_DeserializarString(paquete, &(datos->pokemon), datos->largoPokemon)) return false;
	if (!Paquete_Deserializar(paquete, &((datos->posicion).posX), sizeof(uint32_t))) return false;
	if (!Paquete_Deserializar(paquete, &((datos->posicion).posY), sizeof(uint32_t))) return false; //BORRE EL ID, FUNCION APARTE
	return true;
}

//4
bool Deserializar_CATCH_POKEMON(Paquete* paquete, DATOS_CATCH_POKEMON* datos)
{
	if (!Paquete_Deserializar(paquete, &(datos->largoPokemon), sizeof(uint32_t))) return false;
	if (!Paquete_DeserializarString(paquete, &(datos->pokemon), datos->largoPokemon)) return false;
	if (!Paquete_Deserializar(paquete, &((datos->posicion).posX), sizeof(uint32_t))) return false;
	if (!Paquete_Deserializar(paquete, &((datos->posicion).posY), sizeof(uint32_t))) return false;
	return true;
}

//5
bool Deserializar_CAUGHT_POKEMON(Paquete* paquete, DATOS_CAUGHT_POKEMON* datos)
{
	if (!Paquete_Deserializar(paquete, &(datos->capturado), sizeof(uint32_t))) return false; //BORRE EL ID FUNCION APARTE
	return true;
}

//6
bool Deserializar_GET_POKEMON(Paquete* paquete, DATOS_GET_POKEMON* datos)
{
	if (!Paquete_Deserializar(paquete, &(datos->largoPokemon), sizeof(uint32_t))) return false;
	if (!Paquete_DeserializarString(paquete, &(datos->pokemon), datos->largoPokemon)) return false;
	return true;
}

//7
bool Deserializar_LOCALIZED_POKEMON(Paquete* paquete, DATOS_LOCALIZED_POKEMON* datos)
{
	if (!Paquete_Deserializar(paquete, &(datos->largoPokemon), sizeof(uint32_t))) return false;
	if (!Paquete_DeserializarString(paquete, &(datos->pokemon), datos->largoPokemon)) return false;
	if (!Paquete_Deserializar(paquete, &(datos->cantidad), sizeof(uint32_t))) return false;

	for (int i = 0; i < datos->cantidad; i++) {
		if (!Paquete_Deserializar(paquete, &((datos->posiciones[i]).posX), sizeof(uint32_t))) return false;
		if (!Paquete_Deserializar(paquete, &((datos->posiciones[i]).posY), sizeof(uint32_t))) return false;
	}
	return true;
}

void* Serializar_BROKER_RECONECTAR(BROKER_DATOS_RECONECTAR* datos, int* tamanioBuffer)
{
	*tamanioBuffer = sizeof(BROKER_DATOS_RECONECTAR);
	void* stream = malloc(*tamanioBuffer);

	int desplazamiento = 0;
	memcpy(stream, &(datos->id), sizeof(datos->id));
	desplazamiento += sizeof(datos->id);

	return stream;
}
bool Deserializar_BROKER_RECONECTAR(Paquete* paquete, BROKER_DATOS_RECONECTAR* datos)
{
	if (!Paquete_Deserializar(paquete, &(datos->id), sizeof(uint32_t))) return false;
	return true;
}

void* Serializar_BROKER_CONECTADO(BROKER_DATOS_CONECTADO* datos, int* tamanioBuffer)
{
	return Serializar_BROKER_RECONECTAR(datos, tamanioBuffer);
}
bool Deserializar_BROKER_CONECTADO(Paquete* paquete, BROKER_DATOS_CONECTADO* datos)
{
	return Deserializar_BROKER_RECONECTAR(paquete, datos);
}

void* Serializar_BROKER_SUSCRIBIRSE(BROKER_DATOS_SUSCRIBIRSE* datos, int* tamanioBuffer)
{
	*tamanioBuffer = sizeof(uint32_t);
	void* stream = malloc(*tamanioBuffer);

	int desplazamiento = 0;
	memcpy(stream, &(datos->cola), sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	return stream;
}
bool Deserializar_BROKER_SUSCRIBIRSE(Paquete* paquete, BROKER_DATOS_SUSCRIBIRSE* datos)
{
	if (!Paquete_Deserializar(paquete, &(datos->cola), sizeof(uint32_t))) return false;
	return true;
}


