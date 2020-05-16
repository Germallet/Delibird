#include "protocolo.h"
#include "stream.h"

// NEW_POKEMON
extern void Serializar_NEW_POKEMON(Stream* stream, DATOS_NEW_POKEMON* datos);
extern bool Deserializar_NEW_POKEMON(Stream* paquete, DATOS_NEW_POKEMON* datos);
// APPEARED_POKEMON
extern void Serializar_APPEARED_POKEMON(Stream* stream, DATOS_APPEARED_POKEMON* datos);
extern bool Deserializar_APPEARED_POKEMON(Stream* paquete, DATOS_APPEARED_POKEMON* datos);
// CATCH_POKEMON
extern void Serializar_CATCH_POKEMON(Stream* stream, DATOS_CATCH_POKEMON* datos);
extern bool Deserializar_CATCH_POKEMON(Stream* paquete, DATOS_CATCH_POKEMON* datos);
// CAUGHT_POKEMON
extern void Serializar_CAUGHT_POKEMON(Stream* stream, DATOS_CAUGHT_POKEMON* datos);
extern bool Deserializar_CAUGHT_POKEMON(Stream* paquete, DATOS_CAUGHT_POKEMON* datos);
// GET_POKEMON
extern void Serializar_GET_POKEMON(Stream* stream, DATOS_GET_POKEMON* datos);
extern bool Deserializar_GET_POKEMON(Stream* paquete, DATOS_GET_POKEMON* datos);
// LOCALIZED_POKEMON
extern void Serializar_LOCALIZED_POKEMON(Stream* stream, DATOS_LOCALIZED_POKEMON* datos);
extern bool Deserializar_LOCALIZED_POKEMON(Stream* stream, DATOS_LOCALIZED_POKEMON* datos);
// uint32
extern void Serializar_uint32(Stream* stream, uint32_t dato);
extern bool Deserializar_uint32(Stream* stream, void* dato);
