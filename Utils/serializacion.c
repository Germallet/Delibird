#include "serializacion.h"

// NEW_POKEMON
void Serializar_NEW_POKEMON(Stream* stream, DATOS_NEW_POKEMON* datos)
{
	Stream_SerializarString(stream, datos->pokemon);
	Serializar_uint32(stream, datos->posicion.posX);
	Serializar_uint32(stream, datos->posicion.posY);
	Serializar_uint32(stream, datos->cantidad);
}
DATOS_NEW_POKEMON Deserializar_NEW_POKEMON(Stream* stream)
{
	DATOS_NEW_POKEMON datos;
	if (!Stream_DeserializarString(stream, &(datos.pokemon))) return datos;
	datos.posicion.posX = Deserializar_uint32(stream);
	datos.posicion.posY = Deserializar_uint32(stream);
	datos.cantidad = Deserializar_uint32(stream);
	return datos;
}

// APPEARED_POKEMON
void Serializar_APPEARED_POKEMON(Stream* stream, DATOS_APPEARED_POKEMON* datos)
{
	Serializar_uint32(stream, datos->idCorrelativa);
	Stream_SerializarString(stream, datos->pokemon);
	Serializar_uint32(stream, datos->posicion.posX);
	Serializar_uint32(stream, datos->posicion.posY);
}
DATOS_APPEARED_POKEMON Deserializar_APPEARED_POKEMON(Stream* stream)
{
	DATOS_APPEARED_POKEMON datos;
	datos.idCorrelativa = Deserializar_uint32(stream);
	if (!Stream_DeserializarString(stream, &(datos.pokemon))) return datos;
	datos.posicion.posX = Deserializar_uint32(stream);
	datos.posicion.posY = Deserializar_uint32(stream);
	return datos;
}

// CATCH_POKEMON
void Serializar_CATCH_POKEMON(Stream* stream, DATOS_CATCH_POKEMON* datos)
{
	Stream_SerializarString(stream, datos->pokemon);
	Serializar_uint32(stream, datos->posicion.posX);
	Serializar_uint32(stream, datos->posicion.posY);
}
DATOS_CATCH_POKEMON Deserializar_CATCH_POKEMON(Stream* stream)
{
	DATOS_CATCH_POKEMON datos;
	if (!Stream_DeserializarString(stream, &(datos.pokemon))) return datos;
	datos.posicion.posX = Deserializar_uint32(stream);
	datos.posicion.posY = Deserializar_uint32(stream);
	return datos;
}

// CAUGHT_POKEMON
void Serializar_CAUGHT_POKEMON(Stream* stream, DATOS_CAUGHT_POKEMON* datos)
{
	Serializar_uint32(stream, datos->capturado);
}
DATOS_CAUGHT_POKEMON Deserializar_CAUGHT_POKEMON(Stream* stream)
{
	DATOS_CAUGHT_POKEMON datos;
	datos.capturado = Deserializar_uint32(stream);
	return datos;
}

// GET_POKEMON
void Serializar_GET_POKEMON(Stream* stream, DATOS_GET_POKEMON* datos)
{
	Stream_SerializarString(stream, datos->pokemon);
}
DATOS_GET_POKEMON Deserializar_GET_POKEMON(Stream* stream)
{
	DATOS_GET_POKEMON datos;
	if (!Stream_DeserializarString(stream, &(datos.pokemon))) return datos;
	return datos;
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
DATOS_LOCALIZED_POKEMON Deserializar_LOCALIZED_POKEMON(Stream* stream)
{
	DATOS_LOCALIZED_POKEMON datos;

	if (!Stream_DeserializarString(stream, &(datos.pokemon))) return datos;
	datos.cantidad = Deserializar_uint32(stream);

	datos.posiciones = malloc(datos.cantidad*sizeof(Posicion));

	for (int i = 0; i < datos.cantidad; i++) {
		datos.posiciones[i].posX = Deserializar_uint32(stream);
		datos.posiciones[i].posY = Deserializar_uint32(stream);
	}

	datos.idCorrelativa = Deserializar_uint32(stream);

	return datos;
}

// uint32
void Serializar_uint32(Stream* stream, uint32_t dato)
{
	Stream_Serializar(stream, &dato, sizeof(uint32_t));
}
uint32_t Deserializar_uint32(Stream* stream)
{
	uint32_t resultado;
	Stream_Deserializar(stream, &resultado, sizeof(uint32_t));
	return resultado;
}
