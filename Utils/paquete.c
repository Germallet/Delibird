#include "paquete.h"
#include <stdlib.h>
#include <string.h>

static size_t Paquete_Tamanio(Paquete* paquete)
{
	return sizeof(paquete->codigoOperacion) + paquete->tamanio + sizeof(paquete->tamanio);
}

void* Paquete_Serializar(Paquete* paquete, size_t* tamanioFinal)
{
	*tamanioFinal = Paquete_Tamanio(paquete);
	void* paqueteSerializado = malloc(*tamanioFinal);

	int desplazamiento = 0;
	memcpy(paqueteSerializado, &(paquete->codigoOperacion), sizeof(paquete->codigoOperacion));
	desplazamiento += sizeof(paquete->codigoOperacion);
	memcpy(paqueteSerializado + desplazamiento, &(paquete->tamanio), sizeof(paquete->tamanio));
	desplazamiento += sizeof(paquete->tamanio);
	memcpy(paqueteSerializado + desplazamiento, paquete->stream, paquete->tamanio);
	desplazamiento += paquete->tamanio;

	return paqueteSerializado;
}

int Paquete_Procesar(int numSocket, Paquete* paquete)
{
	if (recv(numSocket, &(paquete->tamanio), sizeof(paquete->tamanio), MSG_WAITALL) == -1)
		return -1;
	paquete->stream = malloc(paquete->tamanio);
	if (recv(numSocket, paquete->stream, paquete->tamanio, MSG_WAITALL) == -1)
		return -1;
	return 0;
}

void Paquete_Liberar(Paquete* paquete)
{
	free(paquete->stream);
	free(paquete);
}
