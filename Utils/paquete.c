#include "paquete.h"
#include <stdlib.h>
#include <string.h>

static size_t Paquete_Tamanio(Paquete* paquete)
{
	return sizeof(paquete->codigoOperacion) + sizeof(paquete->tamanio) + paquete->tamanio;
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
	if (paquete->tamanio > 0)
		if (recv(numSocket, paquete->stream, paquete->tamanio, MSG_WAITALL) == -1)
			return -1;
	return 0;
}

void Paquete_Liberar(Paquete* paquete)
{
	if (paquete == NULL)
		return;
	free(paquete->stream);
	free(paquete);
}

bool Paquete_Deserializar(Paquete* paquete, void* datos, int tamanio)
{
	if (paquete->tamanio < paquete->desplazamiento + tamanio)
		return false;
	memcpy(datos, paquete->stream + paquete->desplazamiento, tamanio);
	paquete->desplazamiento += tamanio;
	return true;
}

bool Paquete_DeserializarString(Paquete* paquete, char** string, int tamanio)
{
	*string = malloc(tamanio+1);
	if (!Paquete_Deserializar(paquete, *string, tamanio))
		return false;
	(*string)[tamanio] = '\0';
	return true;
}

bool Paquete_StreamLeido(Paquete* paquete)
{
	return paquete->tamanio == paquete->desplazamiento;
}
