#include "stream.h"
#include <stdlib.h>
#include <string.h>

Stream* Stream_CrearEscrituraNueva(size_t tamanio)
{
	Stream* stream = malloc(sizeof(Stream));
	stream->base = malloc(tamanio);
	stream->desplazamiento = 0;
	stream->tamanio = tamanio;
	stream->error = false;
	return stream;
}

Stream* Stream_CrearEscritura(void* buffer, size_t tamanio)
{
	Stream* stream = malloc(sizeof(Stream));
	stream->base = buffer;
	stream->desplazamiento = 0;
	stream->tamanio = tamanio;
	return stream;
}

Stream* Stream_CrearLectura(void* buffer, size_t tamanio)
{
	Stream* stream = malloc(sizeof(Stream));
	stream->base = buffer;
	stream->desplazamiento = 0;
	stream->tamanio = tamanio;
	return stream;
}

Stream* Stream_CrearLecturaPaquete(Paquete* paquete)
{
	Stream* stream = malloc(sizeof(Stream));
	stream->base = paquete->stream;
	stream->desplazamiento = 0;
	stream->tamanio = paquete->tamanio;
	return stream;
}

void Stream_Serializar(Stream* stream, void* datos, size_t tamanio)
{
	memcpy(stream->base + stream->desplazamiento, datos, tamanio);
	stream->desplazamiento += tamanio;
}

void Stream_SerializarString(Stream* stream, char* string)
{
	size_t tamanio = strlen(string);
	Stream_Serializar(stream, &tamanio, sizeof(tamanio));
	Stream_Serializar(stream, string, tamanio);
}

bool Stream_Deserializar(Stream* stream, void* datos, int tamanio)
{
	if (stream->tamanio < stream->desplazamiento + tamanio)
	{
		return false;
		stream->error = true;
	}
	memcpy(datos, stream->base + stream->desplazamiento, tamanio);
	stream->desplazamiento += tamanio;
	return true;
}

bool Stream_DeserializarString(Stream* stream, char** string)
{
	uint32_t tamanio;
	if (!Stream_Deserializar(stream, &tamanio, sizeof(uint32_t)))
		return false;

	*string = malloc(tamanio+1);
	if (!Stream_Deserializar(stream, *string, tamanio))
		return false;
	(*string)[tamanio] = '\0';
	return true;
}

bool Stream_Leido(Stream* stream)
{
	return stream->tamanio == stream->desplazamiento;
}

void Stream_Destruir(Stream* stream)
{
	free(stream);
}
void Stream_DestruirConBuffer(Stream* stream)
{
	free(stream->base);
	Stream_Destruir(stream);
}
