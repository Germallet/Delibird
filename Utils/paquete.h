#pragma once
#include <netdb.h>
#include <stdbool.h>

typedef struct
{
	uint32_t codigoOperacion;
	uint32_t tamanio;
	void* stream;
	int desplazamiento;
} Paquete;

extern void* Paquete_Serializar(Paquete* paquete, size_t* tamanioFinal);
extern int Paquete_Procesar(int numSocket, Paquete* paquete);
extern void Paquete_Liberar(Paquete* paquete);
extern bool Paquete_Deserializar(Paquete* paquete, void* datos, int tamanio);
extern bool Paquete_DeserializarString(Paquete* paquete, char** string, int tamanio);
extern bool Paquete_StreamLeido(Paquete* paquete);
