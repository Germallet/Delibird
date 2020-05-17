#include "protocolo.h"
#include "stream.h"

// NEW_POKEMON
extern void Serializar_NEW_POKEMON(Stream* stream, DATOS_NEW_POKEMON* datos);
extern DATOS_NEW_POKEMON Deserializar_NEW_POKEMON(Stream* paquete);
// APPEARED_POKEMON
extern void Serializar_APPEARED_POKEMON(Stream* stream, DATOS_APPEARED_POKEMON* datos);
extern DATOS_APPEARED_POKEMON Deserializar_APPEARED_POKEMON(Stream* paquete);
// CATCH_POKEMON
extern void Serializar_CATCH_POKEMON(Stream* stream, DATOS_CATCH_POKEMON* datos);
extern DATOS_CATCH_POKEMON Deserializar_CATCH_POKEMON(Stream* paquete);
// CAUGHT_POKEMON
extern void Serializar_CAUGHT_POKEMON(Stream* stream, DATOS_CAUGHT_POKEMON* datos);
extern DATOS_CAUGHT_POKEMON Deserializar_CAUGHT_POKEMON(Stream* paquete);
// GET_POKEMON
extern void Serializar_GET_POKEMON(Stream* stream, DATOS_GET_POKEMON* datos);
extern DATOS_GET_POKEMON Deserializar_GET_POKEMON(Stream* paquete);
// LOCALIZED_POKEMON
extern void Serializar_LOCALIZED_POKEMON(Stream* stream, DATOS_LOCALIZED_POKEMON* datos);
extern DATOS_LOCALIZED_POKEMON Deserializar_LOCALIZED_POKEMON(Stream* stream);
// uint32
extern void Serializar_uint32(Stream* stream, uint32_t dato);
extern uint32_t Deserializar_uint32(Stream* stream);
