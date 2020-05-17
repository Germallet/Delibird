#pragma once
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include "paquete.h"

typedef struct
{
	void* base;
	uint32_t tamanio;
	uint32_t desplazamiento;
	bool error;
} Stream;

extern Stream* Stream_CrearEscrituraNueva(size_t tamanio);
extern Stream* Stream_CrearEscritura(void* buffer, size_t tamanio);
extern Stream* Stream_CrearLectura(void* buffer, size_t tamanio);
extern Stream* Stream_CrearLecturaPaquete(Paquete* paquete);
extern void Stream_Serializar(Stream* stream, void* datos, size_t tamanio);
extern void Stream_SerializarString(Stream* stream, char* string);
extern bool Stream_Deserializar(Stream* stream, void* datos, int tamanio);
extern bool Stream_DeserializarString(Stream* stream, char** string);
extern bool Stream_Leido(Stream* stream);
extern void Stream_Destruir(Stream* stream);
extern void Stream_DestruirConBuffer(Stream* stream);
