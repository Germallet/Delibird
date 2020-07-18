#include "conexionBroker.h"
#include "../Utils/hiloTimer.h"
#include "../Utils/socket.h"
#include "../Utils/protocolo.h"
#include <stdio.h>

static void reconexion(void* info)
{
	ConexionBroker* conexion = (ConexionBroker*)info;
	if(info == NULL)
		return;
	if(conexion->clienteBroker != NULL && conexion->clienteBroker->socket != -1)
		return;

	if(conexion->clienteBroker != NULL)
		freeaddrinfo((struct addrinfo*)conexion->clienteBroker->direccion);
	conexion->clienteBroker = CrearCliente(conexion->ip, conexion->puerto, NULL);
	if(conexion->clienteBroker == NULL)
	{
		if(conexion->intentoFallido != NULL)
			conexion->intentoFallido();
	}
	else
	{
		conexion->clienteBroker->eventos = conexion->eventos;

		conexion->clienteBroker->info = conexion;
		if(conexion->datosConectado == NULL)
		{
			Socket_Enviar(BROKER_CONECTAR, NULL, 0, conexion->clienteBroker->socket);
		}
		else
		{
			Stream* stream = SerializarM_BROKER_CONECTADO(conexion->datosConectado);
			Socket_Enviar(BROKER_RECONECTAR, stream->base, stream->tamanio, conexion->clienteBroker->socket);
			Stream_DestruirConBuffer(stream);
		}
	}
}

static void ConexionColas(Cliente* cliente, Paquete* paquete) {
	ConexionBroker* nuevaConexion = (ConexionBroker*) cliente->info;
	bool primeraVez = (nuevaConexion->datosConectado == NULL);

	if(nuevaConexion->datosConectado != NULL)
		free(nuevaConexion->datosConectado);
	BROKER_DATOS_CONECTADO* datos = malloc(sizeof(BROKER_DATOS_CONECTADO));
	DeserializarM_BROKER_CONECTADO(paquete, datos);

	if (primeraVez)
	{
		nuevaConexion->datosConectado = datos;
		if (nuevaConexion->alConectarse != NULL) nuevaConexion->alConectarse(cliente);
	}
	else
	{
		if (nuevaConexion->datosConectado == datos)
		{
			nuevaConexion->datosConectado = datos;
			if (nuevaConexion->alReconectarse != NULL)
				nuevaConexion->alReconectarse(cliente);
		}
		else
		{
			nuevaConexion->datosConectado = datos;
			if (nuevaConexion->alConectarse != NULL)
				nuevaConexion->alConectarse(cliente);
			if (nuevaConexion->alReconectarse != NULL)
				nuevaConexion->alReconectarse(cliente);
		}
	}
	cliente->info = NULL;
}

ConexionBroker* ConectarseABroker(char* ip, int puerto, Eventos* eventos, void (*alConectarse)(Cliente*), void (*alReconectarse)(Cliente*), void (*intentoFallido)(), int tiempoReintentoConexion)
{
	ConexionBroker* nuevaConexion = malloc(sizeof(ConexionBroker));
	nuevaConexion->clienteBroker = NULL;
	nuevaConexion->datosConectado = NULL;
	nuevaConexion->ip = ip;
	nuevaConexion->puerto = puerto;
	nuevaConexion->eventos = eventos;
	nuevaConexion->alConectarse = alConectarse;
	nuevaConexion->alReconectarse = alReconectarse;
	nuevaConexion->intentoFallido = intentoFallido;

	Eventos_AgregarOperacion(nuevaConexion->eventos, BROKER_CONECTADO, (EventoOperacion)&ConexionColas);

	nuevaConexion->hiloTimer = CrearHiloTimer(-1, tiempoReintentoConexion, &reconexion, nuevaConexion);

	return nuevaConexion;
}

void DestruirConexionBroker(ConexionBroker* conexion)
{
	DetenerHiloTimer(conexion->hiloTimer);
	if(conexion->clienteBroker != NULL)
		DestruirCliente2(conexion->clienteBroker);
	if (conexion->datosConectado != NULL) free(conexion->datosConectado);
	free(conexion);
}
