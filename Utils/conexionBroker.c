#include "conexionBroker.h"
#include "../Utils/hiloTimer.h"
#include "../Utils/socket.h"
#include "../Utils/protocolo.h"

static void reconexion(void* info)
{
	ConexionBroker* conexion = (ConexionBroker*)info;
	if(conexion->clienteBroker != NULL) return;

	conexion->clienteBroker = CrearCliente(conexion->ip, conexion->puerto, conexion->eventos);
	if(conexion->clienteBroker != NULL)
	{
		conexion->clienteBroker->info = conexion;
		if(conexion->datosConectado == NULL)
			Socket_Enviar(BROKER_CONECTAR, NULL, 0, conexion->clienteBroker->socket);
		else
		{
			Stream* stream = SerializarM_BROKER_CONECTADO(conexion->datosConectado);
			Socket_Enviar(BROKER_RECONECTAR, stream->base, stream->tamanio, conexion->clienteBroker->socket);
		}
	}
}

static void ConexionColas(Cliente* cliente, Paquete* paquete) {
	ConexionBroker* nuevaConexion = (ConexionBroker*) cliente->info;
	bool primeraVez = (nuevaConexion->datosConectado == NULL);

	nuevaConexion->datosConectado = malloc(sizeof(BROKER_DATOS_CONECTADO));
	DeserializarM_BROKER_CONECTADO(paquete, nuevaConexion->datosConectado);

	if (primeraVez)
	{
		if (nuevaConexion->alConectarse != NULL) nuevaConexion->alConectarse(cliente);
	}
	else if (nuevaConexion->alReconectarse != NULL)
		nuevaConexion->alReconectarse(cliente);
}

ConexionBroker* ConectarseABroker(char* ip, int puerto, Eventos* eventos, void (*alConectarse)(Cliente*), void (*alReconectarse)(Cliente*), int tiempoReintentoConexion)
{
	ConexionBroker* nuevaConexion = malloc(sizeof(ConexionBroker));
	nuevaConexion->clienteBroker = NULL;
	nuevaConexion->datosConectado = NULL;
	nuevaConexion->ip = ip;
	nuevaConexion->puerto = puerto;
	nuevaConexion->eventos = eventos;
	nuevaConexion->alConectarse = alConectarse;
	nuevaConexion->alConectarse = alReconectarse;

	Eventos_AgregarOperacion(nuevaConexion->eventos, BROKER_CONECTADO, (EventoOperacion)&ConexionColas);

	CrearHiloTimer(-1, tiempoReintentoConexion, &reconexion, nuevaConexion);

	return nuevaConexion;
}
