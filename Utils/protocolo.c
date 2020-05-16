#include "protocolo.h"
#include "serializacion.h"
#include "paquete.h"

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
bool DeserializarM_NEW_POKEMON(Paquete* paquete, DATOS_NEW_POKEMON* datos)
{
	Stream* stream = Stream_CrearLecturaPaquete(paquete);
	if (!Deserializar_NEW_POKEMON(stream, datos)) { Stream_Destruir(stream); return false; }
	Stream_Destruir(stream);
	return true;
}
bool DeserializarM_NEW_POKEMON_ID(Paquete* paquete, DATOS_NEW_POKEMON_ID* datos)
{
	Stream* stream = Stream_CrearLecturaPaquete(paquete);
	if (!Stream_Deserializar(stream, &(datos->id), sizeof(uint32_t))) { Stream_Destruir(stream); return false; }
	if (!Deserializar_NEW_POKEMON(stream, &(datos->datos))) { Stream_Destruir(stream); return false; }
	Stream_Destruir(stream);
	return true;
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
bool DeserializarM_APPEARED_POKEMON(Paquete* paquete, DATOS_APPEARED_POKEMON* datos)
{
	Stream* stream = Stream_CrearLecturaPaquete(paquete);
	if (!Deserializar_APPEARED_POKEMON(stream, datos)) { Stream_Destruir(stream); return false; }
	Stream_Destruir(stream);
	return true;
}
bool DeserializarM_APPEARED_POKEMON_ID(Paquete* paquete, DATOS_APPEARED_POKEMON_ID* datos)
{
	Stream* stream = Stream_CrearLecturaPaquete(paquete);
	if (!Stream_Deserializar(stream, &(datos->id), sizeof(uint32_t))) { Stream_Destruir(stream); return false; }
	if (!Deserializar_APPEARED_POKEMON(stream, &(datos->datos))) { Stream_Destruir(stream); return false; }
	Stream_Destruir(stream);
	return true;
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
bool DeserializarM_CATCH_POKEMON(Paquete* paquete, DATOS_CATCH_POKEMON* datos)
{
	Stream* stream = Stream_CrearLecturaPaquete(paquete);
	if (!Stream_Deserializar(stream, datos, sizeof(uint32_t))) { Stream_Destruir(stream); return false; }
	Stream_Destruir(stream);
	return true;
}
bool DeserializarM_CATCH_POKEMON_ID(Paquete* paquete, DATOS_CATCH_POKEMON_ID* datos)
{
	Stream* stream = Stream_CrearLecturaPaquete(paquete);
	if (!Stream_Deserializar(stream, &(datos->id), sizeof(uint32_t))) { Stream_Destruir(stream); return false; }
	if (!Deserializar_CATCH_POKEMON(stream, &(datos->datos))) { Stream_Destruir(stream); return false; }
	Stream_Destruir(stream);
	return true;
}

// CAUGHT_POKEMON
Stream* SerializarM_CAUGHT_POKEMON(DATOS_CAUGHT_POKEMON* datos)
{
	Stream* stream = Stream_CrearEscrituraNueva(sizeof(uint32_t)*2);
	Serializar_CAUGHT_POKEMON(stream, datos);
	return stream;
}
Stream* SerializarM_CAUGHT_POKEMON_ID(DATOS_CAUGHT_POKEMON_ID* datos)
{
	Stream* stream = Stream_CrearEscrituraNueva(sizeof(uint32_t)*3);
	Serializar_uint32(stream, datos->id);
	Serializar_CAUGHT_POKEMON(stream, &(datos->datos));
	return stream;
}
bool DeserializarM_CAUGHT_POKEMON(Paquete* paquete, DATOS_CAUGHT_POKEMON* datos)
{
	Stream* stream = Stream_CrearLecturaPaquete(paquete);
	if (!Deserializar_CAUGHT_POKEMON(stream, datos)) { Stream_Destruir(stream); return false; }
	Stream_Destruir(stream);
	return true;
}
bool DeserializarM_CAUGHT_POKEMON_ID(Paquete* paquete, DATOS_CAUGHT_POKEMON_ID* datos)
{
	Stream* stream = Stream_CrearLecturaPaquete(paquete);
	if (!Stream_Deserializar(stream, &(datos->id), sizeof(uint32_t))) { Stream_Destruir(stream); return false; }
	if (!Deserializar_CAUGHT_POKEMON(stream, &(datos->datos))) { Stream_Destruir(stream); return false; }
	Stream_Destruir(stream);
	return true;
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
bool DeserializarM_GET_POKEMON(Paquete* paquete, DATOS_GET_POKEMON* datos)
{
	Stream* stream = Stream_CrearLecturaPaquete(paquete);
	if (!Deserializar_GET_POKEMON(stream, datos)) { Stream_Destruir(stream); return false; }
	Stream_Destruir(stream);
	return true;
}
bool DeserializarM_GET_POKEMON_ID(Paquete* paquete, DATOS_GET_POKEMON_ID* datos)
{
	Stream* stream = Stream_CrearLecturaPaquete(paquete);
	if (!Stream_Deserializar(stream, &(datos->id), sizeof(uint32_t))) { Stream_Destruir(stream); return false; }
	if (!Deserializar_GET_POKEMON(stream, &(datos->datos))) { Stream_Destruir(stream); return false; }
	Stream_Destruir(stream);
	return true;
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
bool DeserializarM_LOCALIZED_POKEMON(Paquete* paquete, DATOS_LOCALIZED_POKEMON* datos)
{
	Stream* stream = Stream_CrearLecturaPaquete(paquete);
	if (!Deserializar_LOCALIZED_POKEMON(stream, datos)) { Stream_Destruir(stream); return false; }
	Stream_Destruir(stream);
	return true;
}
bool DeserializarM_LOCALIZED_POKEMON_ID(Paquete* paquete, DATOS_LOCALIZED_POKEMON_ID* datos)
{
	Stream* stream = Stream_CrearLecturaPaquete(paquete);
	if (!Stream_Deserializar(stream, &(datos->id), sizeof(uint32_t))) { Stream_Destruir(stream); return false; }
	if (!Deserializar_LOCALIZED_POKEMON(stream, &(datos->datos))) { Stream_Destruir(stream); return false; }
	Stream_Destruir(stream);
	return true;
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
