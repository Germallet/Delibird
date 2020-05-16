#include "serializacion.h"

// NEW_POKEMON
void Serializar_NEW_POKEMON(Stream* stream, DATOS_NEW_POKEMON* datos)
{
	Stream_SerializarString(stream, datos->pokemon);
	Serializar_uint32(stream, datos->posicion.posX);
	Serializar_uint32(stream, datos->posicion.posY);
	Serializar_uint32(stream, datos->cantidad);
}
bool Deserializar_NEW_POKEMON(Stream* paquete, DATOS_NEW_POKEMON* datos)
{
	if (!Stream_DeserializarString(paquete, &(datos->pokemon))) return false;
	if (!Deserializar_uint32(paquete, &((datos->posicion).posX))) return false;
	if (!Deserializar_uint32(paquete, &((datos->posicion).posY))) return false;
	if (!Deserializar_uint32(paquete, &(datos->cantidad))) return false;
	return true;
}

// APPEARED_POKEMON
void Serializar_APPEARED_POKEMON(Stream* stream, DATOS_APPEARED_POKEMON* datos)
{
	Serializar_uint32(stream, datos->idCorrelativa);
	Stream_SerializarString(stream, datos->pokemon);
	Serializar_uint32(stream, datos->posicion.posX);
	Serializar_uint32(stream, datos->posicion.posY);
}
bool Deserializar_APPEARED_POKEMON(Stream* paquete, DATOS_APPEARED_POKEMON* datos)
{
	if (!Deserializar_uint32(paquete, &((datos->idCorrelativa)))) return false;
	if (!Stream_DeserializarString(paquete, &(datos->pokemon))) return false;
	if (!Deserializar_uint32(paquete, &((datos->posicion).posX))) return false;
	if (!Deserializar_uint32(paquete, &((datos->posicion).posY))) return false;
	return true;
}

// CATCH_POKEMON
void Serializar_CATCH_POKEMON(Stream* stream, DATOS_CATCH_POKEMON* datos)
{
	Stream_SerializarString(stream, datos->pokemon);
	Serializar_uint32(stream, datos->posicion.posX);
	Serializar_uint32(stream, datos->posicion.posY);
}
bool Deserializar_CATCH_POKEMON(Stream* paquete, DATOS_CATCH_POKEMON* datos)
{
	if (!Stream_DeserializarString(paquete, &(datos->pokemon))) return false;
	if (!Deserializar_uint32(paquete, &((datos->posicion).posX))) return false;
	if (!Deserializar_uint32(paquete, &((datos->posicion).posY))) return false;
	return true;
}

// CAUGHT_POKEMON
void Serializar_CAUGHT_POKEMON(Stream* stream, DATOS_CAUGHT_POKEMON* datos)
{
	Serializar_uint32(stream, datos->idCorrelativa);
	Serializar_uint32(stream, datos->capturado);
}
bool Deserializar_CAUGHT_POKEMON(Stream* paquete, DATOS_CAUGHT_POKEMON* datos)
{
	if (!Deserializar_uint32(paquete, &(datos->idCorrelativa))) return false;
	if (!Deserializar_uint32(paquete, &(datos->capturado))) return false;
	return true;
}

// GET_POKEMON
void Serializar_GET_POKEMON(Stream* stream, DATOS_GET_POKEMON* datos)
{
	Stream_SerializarString(stream, datos->pokemon);
}
bool Deserializar_GET_POKEMON(Stream* stream, DATOS_GET_POKEMON* datos)
{
	if (!Stream_DeserializarString(stream, &(datos->pokemon))) return false;
	return true;
}

// LOCALIZED_POKEMON
void Serializar_LOCALIZED_POKEMON(Stream* stream, DATOS_LOCALIZED_POKEMON* datos)
{
	Stream_SerializarString(stream, datos->pokemon);
	Serializar_uint32(stream, datos->cantidad);

	for (int i = 0; i < datos->cantidad; i++) {
		Serializar_uint32(stream, datos->posiciones[i].posX);
		Serializar_uint32(stream, datos->posiciones[i].posY);
	}

	Serializar_uint32(stream, datos->idCorrelativa);
}
bool Deserializar_LOCALIZED_POKEMON(Stream* stream, DATOS_LOCALIZED_POKEMON* datos)
{
	if (!Stream_DeserializarString(stream, &(datos->pokemon))) return false;
	if (!Deserializar_uint32(stream, &(datos->cantidad))) return false;

	datos->posiciones = malloc((datos->cantidad)*sizeof(Posicion));

	for (int i = 0; i < datos->cantidad; i++) {
		if (!Deserializar_uint32(stream, &((datos->posiciones[i]).posX))) return false;
		if (!Deserializar_uint32(stream, &((datos->posiciones[i]).posY))) return false;
	}

	if (!Deserializar_uint32(stream, &(datos->idCorrelativa))) return false;
	return true;
}

// uint32
void Serializar_uint32(Stream* stream, uint32_t dato)
{
	Stream_Serializar(stream, &dato, sizeof(uint32_t));
}
bool Deserializar_uint32(Stream* stream, void* dato)
{
	return Stream_Deserializar(stream, &dato, sizeof(uint32_t));
}
