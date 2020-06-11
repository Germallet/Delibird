#include "protocolo.h"
#include "serializacion.h"
#include "paquete.h"
#include <stdio.h>

// NEW_POKEMON
Stream* SerializarM_NEW_POKEMON(DATOS_NEW_POKEMON* datos)
{
	Stream* stream = Stream_CrearEscrituraNueva(strlen(datos->pokemon) + sizeof(uint32_t)*5);
	Serializar_NEW_POKEMON(stream, datos);
	return stream;
}
Stream* SerializarM_NEW_POKEMON_ID(DATOS_NEW_POKEMON_ID* datos)
{
	Stream* stream = Stream_CrearEscrituraNueva(strlen(datos->datos.pokemon) + sizeof(uint32_t)*5);
	Serializar_uint32(stream, datos->id);
	Serializar_NEW_POKEMON(stream, &(datos->datos));
	return stream;
}

// APPEARED_POKEMON
Stream* SerializarM_APPEARED_POKEMON(DATOS_APPEARED_POKEMON* datos)
{
	Stream* stream = Stream_CrearEscrituraNueva(strlen(datos->pokemon) + sizeof(uint32_t)*4);
	Serializar_APPEARED_POKEMON(stream, datos);
	return stream;
}
Stream* SerializarM_APPEARED_POKEMON_ID(DATOS_APPEARED_POKEMON_ID* datos)
{
	Stream* stream = Stream_CrearEscrituraNueva(strlen(datos->datos.pokemon) + sizeof(uint32_t)*4);
	Serializar_uint32(stream, datos->id);
	Serializar_APPEARED_POKEMON(stream, &(datos->datos));
	return stream;
}

// CATCH_POKEMON
Stream* SerializarM_CATCH_POKEMON(DATOS_CATCH_POKEMON* datos)
{
	Stream* stream = Stream_CrearEscrituraNueva(strlen(datos->pokemon) + sizeof(uint32_t)*3);
	Serializar_CATCH_POKEMON(stream, datos);
	return stream;
}
Stream* SerializarM_CATCH_POKEMON_ID( DATOS_CATCH_POKEMON_ID* datos)
{
	Stream* stream = Stream_CrearEscrituraNueva(strlen(datos->datos.pokemon) + sizeof(uint32_t)*4);
	Serializar_uint32(stream, datos->id);
	Serializar_CATCH_POKEMON(stream, &(datos->datos));
	return stream;
}

// CAUGHT_POKEMON
Stream* SerializarM_CAUGHT_POKEMON(DATOS_CAUGHT_POKEMON* datos)
{
	Stream* stream = Stream_CrearEscrituraNueva(sizeof(uint32_t));
	Serializar_CAUGHT_POKEMON(stream, datos);
	return stream;
}
Stream* SerializarM_CAUGHT_POKEMON_ID(DATOS_CAUGHT_POKEMON_ID* datos)
{
	Stream* stream = Stream_CrearEscrituraNueva(sizeof(uint32_t)*2);
	Serializar_uint32(stream, datos->idCorrelativa);
	Serializar_CAUGHT_POKEMON(stream, &(datos->datos));
	return stream;
}

// GET_POKEMON
Stream* SerializarM_GET_POKEMON(DATOS_GET_POKEMON* datos)
{
	Stream* stream = Stream_CrearEscrituraNueva(sizeof(uint32_t) + strlen(datos->pokemon));
	Serializar_GET_POKEMON(stream, datos);
	return stream;
}
Stream* SerializarM_GET_POKEMON_ID(DATOS_GET_POKEMON_ID* datos)
{
	Stream* stream = Stream_CrearEscrituraNueva(sizeof(uint32_t) + strlen(datos->datos.pokemon) + sizeof(uint32_t));
	Serializar_uint32(stream, datos->id);
	Serializar_GET_POKEMON(stream, &(datos->datos));
	return stream;
}

// LOCALIZED_POKEMON
Stream* SerializarM_LOCALIZED_POKEMON(DATOS_LOCALIZED_POKEMON* datos)
{
	Stream* stream = Stream_CrearEscrituraNueva(strlen(datos->pokemon) + sizeof(uint32_t) + (datos->cantidad)*(sizeof(uint32_t)*2));
	Serializar_LOCALIZED_POKEMON(stream, datos);
	return stream;
}
Stream* SerializarM_LOCALIZED_POKEMON_ID(DATOS_LOCALIZED_POKEMON_ID* datos)
{
	Stream* stream = Stream_CrearEscrituraNueva(strlen(datos->datos.pokemon) + sizeof(uint32_t)*2 + (datos->datos.cantidad)*(sizeof(uint32_t)*2));
	Serializar_uint32(stream, datos->id);
	Serializar_LOCALIZED_POKEMON(stream, &(datos->datos));
	return stream;
}

// ID_MENSAJE
Stream* SerializarM_ID_MENSAJE(DATOS_ID_MENSAJE* datos)
{
	Stream* stream = Stream_CrearEscrituraNueva(sizeof(uint32_t));
	Serializar_uint32(stream, datos->id);
	return stream;
}
bool DeserializarM_ID_MENSAJE(Paquete* paquete, DATOS_ID_MENSAJE* datos)
{
	Stream* stream = Stream_CrearLecturaPaquete(paquete);
	if (!Stream_Deserializar(stream, &(datos->id), sizeof(uint32_t))) { Stream_Destruir(stream); return false; }
	Stream_Destruir(stream);
	return true;
}

// BROKER_RECONECTAR
Stream* SerializarM_BROKER_RECONECTAR(BROKER_DATOS_RECONECTAR* datos)
{
	Stream* stream = Stream_CrearEscrituraNueva(sizeof(uint32_t));
	Serializar_uint32(stream, datos->id);
	return stream;
}
bool DeserializarM_BROKER_RECONECTAR(Paquete* paquete, BROKER_DATOS_RECONECTAR* datos)
{
	Stream* stream = Stream_CrearLecturaPaquete(paquete);
	if (!Stream_Deserializar(stream, &(datos->id), sizeof(uint32_t))) { Stream_Destruir(stream); return false; }
	Stream_Destruir(stream);
	return true;
}

// BROKER_CONECTADO
Stream* SerializarM_BROKER_CONECTADO(BROKER_DATOS_CONECTADO* datos)
{
	return SerializarM_BROKER_RECONECTAR(datos);
}
bool DeserializarM_BROKER_CONECTADO(Paquete* paquete, BROKER_DATOS_CONECTADO* datos)
{
	return DeserializarM_BROKER_RECONECTAR(paquete, datos);
}

// BROKER_SUSCRIBIRSE
Stream* SerializarM_BROKER_SUSCRIBIRSE(BROKER_DATOS_SUSCRIBIRSE* datos)
{
	Stream* stream = Stream_CrearEscrituraNueva(sizeof(uint32_t));
	Serializar_uint32(stream, datos->cola);
	return stream;
}
bool DeserializarM_BROKER_SUSCRIBIRSE(Paquete* paquete, BROKER_DATOS_SUSCRIBIRSE* datos)
{
	Stream* stream = Stream_CrearLecturaPaquete(paquete);
	if (!Stream_Deserializar(stream, &(datos->cola), sizeof(uint32_t))) { Stream_Destruir(stream); return false; }
	Stream_Destruir(stream);
	return true;
}

char* CodigoDeOperacionAString(CodigoDeOperacion codigoDeOperacion)
{
	switch(codigoDeOperacion)
	{
	case BROKER_CONECTAR:
		return "BROKER_CONECTAR";
	case BROKER_RECONECTAR:
		return "BROKER_RECONECTAR";
	case BROKER_SUSCRIBIRSE:
		return "BROKER_SUSCRIBIRSE";
	case BROKER_ACK:
		return "BROKER_ACK";
	case BROKER_CONECTADO:
		return "BROKER_CONECTADO";
	case BROKER_SUSCRITO:
		return "BROKER_SUSCRITO";
	case BROKER_ID_MENSAJE:
		return "BROKER_ID_MENSAJE";
	case NEW_POKEMON:
		return "NEW_POKEMON";
	case APPEARED_POKEMON:
		return "APPEARED_POKEMON";
	case CATCH_POKEMON:
		return "CATCH_POKEMON";
	case CAUGHT_POKEMON:
		return "CAUGHT_POKEMON";
	case GET_POKEMON:
		return "GET_POKEMON";
	case LOCALIZED_POKEMON:
		return "LOCALIZED_POKEMON";
	}
	return "INVALIDO";
}

char* DatosAString_NEW_POKEMON(DATOS_NEW_POKEMON* datos)
{
	char* mensaje = malloc(80);
	snprintf(mensaje, 80, "NEW_POKEMON: {pokemon: '%s', cantidad: %d}", datos->pokemon, datos->cantidad);
	return mensaje;
}
char* DatosAString_APPEARED_POKEMON(DATOS_APPEARED_POKEMON* datos)
{
	char* mensaje = malloc(80);
	snprintf(mensaje, 80, "APPEARED_POKEMON: {pokemon: '%s', posicion: (%d, %d)}", datos->pokemon, datos->posicion.posX, datos->posicion.posY);
	return mensaje;
}
char* DatosAString_CATCH_POKEMON(DATOS_CATCH_POKEMON* datos)
{
	char* mensaje = malloc(80);
	snprintf(mensaje, 80, "CATCH_POKEMON: {pokemon: '%s', posicion: (%d, %d)}", datos->pokemon, datos->posicion.posX, datos->posicion.posY);
	return mensaje;
}
char* DatosAString_CAUGHT_POKEMON(DATOS_CAUGHT_POKEMON* datos)
{
	char* mensaje = malloc(80);
	snprintf(mensaje, 80, "CAUGHT_POKEMON: {capturado: %s}", datos->capturado == 1 ? "OK" : "FAIL");
	return mensaje;
}
char* DatosAString_GET_POKEMON(DATOS_GET_POKEMON* datos)
{
	char* mensaje = malloc(80);
	snprintf(mensaje, 80, "GET_POKEMON: {pokemon: '%s'}", datos->pokemon);
	return mensaje;
}
char* DatosAString_LOCALIZED_POKEMON(DATOS_LOCALIZED_POKEMON* datos)
{
	char* mensaje = malloc(80);
	snprintf(mensaje, 80, "LOCALIZED_POKEMON: {pokemon: '%s', cantidad: %d}", datos->pokemon, datos->cantidad);
	return mensaje;
}
