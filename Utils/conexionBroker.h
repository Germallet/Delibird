#pragma once
#include "../Utils/net.h"

typedef struct {
	Cliente* clienteBroker;
	BROKER_DATOS_CONECTADO* datosConectado;
	char* ip;
	int puerto;
	Eventos* eventos;
	void (*alConectarse)(Cliente*);
	void (*alReconectarse)(Cliente*);
} ConexionBroker;

ConexionBroker* ConectarseABroker(char* ip, int puerto, Eventos* eventos, void (*alConectarse)(Cliente*), void (*alReconectarse)(Cliente*), int tiempoReintentoConexion);
