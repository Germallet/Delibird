#include "conexionBroker.h"
#include "../Utils/hiloTimer.h"
#include "../Utils/socket.h"
#include "../Utils/protocolo.h"

static void reconexion(void* info)
{
	ConexionBroker* conexion = (ConexionBroker*)info;

	conexion->clienteBroker = CrearCliente(conexion->ip, conexion->puerto, conexion->eventos);
	if(conexion->clienteBroker != NULL)
	{
		if(conexion->datosConectado == NULL)
			conexion->alConectarse(conexion->clienteBroker);
		else
		{
			Stream* stream = SerializarM_BROKER_CONECTADO(conexion->datosConectado);
			Socket_Enviar(BROKER_CONECTADO, stream->base, stream->tamanio, conexion->clienteBroker->socket);
		}
	}
}

ConexionBroker* ConectarseABroker(char* ip, int puerto, Eventos* eventos, void (*alConectarse)(Cliente*), int tiempoReintentoConexion)
{
	ConexionBroker* nuevaConexion = malloc(sizeof(ConexionBroker));
	nuevaConexion->clienteBroker = NULL;
	nuevaConexion->datosConectado = NULL;
	nuevaConexion->ip = ip;
	nuevaConexion->puerto = puerto;
	nuevaConexion->eventos = eventos;
	nuevaConexion->alConectarse = alConectarse;

	void ConexionColas(Cliente* cliente, Paquete* paquete) {
		DeserializarM_BROKER_CONECTADO(paquete, nuevaConexion->datosConectado);
	}
	Eventos_AgregarOperacion(nuevaConexion->eventos, BROKER_CONECTADO, (EventoOperacion)&ConexionColas);

	CrearHiloTimer(-1, tiempoReintentoConexion, &reconexion, nuevaConexion);

	return nuevaConexion;
}
