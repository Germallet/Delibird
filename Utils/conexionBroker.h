#pragma once
#include "hiloTimer.h"
#include "../Utils/net.h"

typedef struct {
	Cliente* clienteBroker;
	HiloTimer* hiloTimer;
	BROKER_DATOS_CONECTADO* datosConectado;
	char* ip;
	int puerto;
	Eventos* eventos;
	void (*alConectarse)(Cliente*);
	void (*alReconectarse)(Cliente*);
	void (*intentoFallido)();

} ConexionBroker;

ConexionBroker* ConectarseABroker(char* ip, int puerto, Eventos* eventos, void (*alConectarse)(Cliente*), void (*alReconectarse)(Cliente*), /*void (*intentoFallido)(),*/ int tiempoReintentoConexion);
void DestruirConexionBroker(ConexionBroker* conexion);
