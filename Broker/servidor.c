#include "broker.h"
#include "clienteBroker.h"
#include "cola.h"
#include "../Utils/socket.h"

void ClienteConectado(Cliente* cliente);
void ClienteDesconectado(Cliente* cliente);

void Operacion_CONECTAR(Cliente* cliente, Paquete* paqueteRecibido);
void Operacion_RECONECTAR(Cliente* cliente, Paquete* paqueteRecibido);
void Operacion_SUSCRIBIRSE(Cliente* cliente, Paquete* paqueteRecibido);

void Operacion_NEW_POKEMON(Cliente* cliente, Paquete* paqueteRecibido);
void Operacion_APPEARED_POKEMON(Cliente* cliente, Paquete* paqueteRecibido);
void Operacion_CATCH_POKEMON(Cliente* cliente, Paquete* paqueteRecibido);
void Operacion_CAUGHT_POKEMON(Cliente* cliente, Paquete* paqueteRecibido);
void Operacion_GET_POKEMON(Cliente* cliente, Paquete* paqueteRecibido);
void Operacion_LOCALIZED_POKEMON(Cliente* cliente, Paquete* paqueteRecibido);

void IniciarServidorBroker(char* ip, int puerto)
{
	Eventos* eventos = Eventos_Crear2((EventoGeneral)&ClienteConectado, (EventoGeneral)&ClienteDesconectado);
	Eventos_AgregarOperacion(eventos, BROKER_CONECTAR, (EventoOperacion)&Operacion_CONECTAR);
	Eventos_AgregarOperacion(eventos, BROKER_RECONECTAR, (EventoOperacion)&Operacion_RECONECTAR);
	Eventos_AgregarOperacion(eventos, BROKER_SUSCRIBIRSE, (EventoOperacion)&Operacion_SUSCRIBIRSE);

	Eventos_AgregarOperacion(eventos, NEW_POKEMON, (EventoOperacion)&Operacion_NEW_POKEMON);
	Eventos_AgregarOperacion(eventos, APPEARED_POKEMON, (EventoOperacion)&Operacion_APPEARED_POKEMON);
	Eventos_AgregarOperacion(eventos, CATCH_POKEMON, (EventoOperacion)&Operacion_CATCH_POKEMON);
	Eventos_AgregarOperacion(eventos, CAUGHT_POKEMON, (EventoOperacion)&Operacion_CAUGHT_POKEMON);
	Eventos_AgregarOperacion(eventos, GET_POKEMON, (EventoOperacion)&Operacion_GET_POKEMON);
	Eventos_AgregarOperacion(eventos, LOCALIZED_POKEMON, (EventoOperacion)&Operacion_LOCALIZED_POKEMON);

	Servidor* servidor = CrearServidor(ip, puerto, eventos);

	if (servidor != NULL)
		log_info(logger, "Escucha iniciada (%s:%d)", ip, puerto);
	else
		log_error(logger, "Error al iniciar escucha (%s:%d)", ip, puerto);
}

void ClienteConectado(Cliente* cliente)
{
	log_info(logger, "Se conectó un cliente");
}
void ClienteDesconectado(Cliente* cliente)
{
	log_info(logger, "Se desconectó un cliente");
}

void Operacion_CONECTAR(Cliente* cliente, Paquete* paqueteRecibido)
{
	if (cliente->info != NULL)
	{
		// TODO
		log_error(logger, "El cliente ya está conectado");
		return;
	}
	if (!Paquete_StreamLeido(paqueteRecibido))
	{
		// TODO
		log_error(logger, "Mensaje inválido");
		return;
	}

	ClienteBroker* clienteBroker = CrearClienteBroker(cliente);
	cliente->info = clienteBroker;

	int tamanioBuffer;
	Broker_DATOS_RECONECTAR datos;
	datos.id = clienteBroker->id;
	void* buffer = Serializar_BROKER_CONECTADO(&(datos), &tamanioBuffer);
	int r = Socket_Enviar(BROKER_CONECTADO, buffer, tamanioBuffer, cliente->socket);

	if (r > 0)
		log_info(logger, "Se conectó un cliente correctamente (id: %d)", clienteBroker->id);
	else
		log_error(logger, "Error al conectar cliente");
}

void Operacion_RECONECTAR(Cliente* cliente, Paquete* paqueteRecibido)
{
	if (cliente->info != NULL)
	{
		// TODO
		log_error(logger, "El cliente ya está conectado");
		return;
	}

	Broker_DATOS_RECONECTAR datos;
	if (!Deserializar_BROKER_RECONECTAR(paqueteRecibido, &datos) || !Paquete_StreamLeido(paqueteRecibido))
		log_error(logger, "Error Deserializar_BROKER_RECONECTAR");
	else
	{
		ClienteBroker* clienteBroker = ObtenerClienteBroker(datos.id);
		if (clienteBroker == NULL)
		{
			// TODO
			log_error(logger, "El cliente no puede reconectar");
			return;
		}
		else
		{
			int tamanioBuffer;
			Broker_DATOS_RECONECTAR datos;
			datos.id = clienteBroker->id;
			void* buffer = Serializar_BROKER_CONECTADO(&(datos), &tamanioBuffer);
			int r = Socket_Enviar(BROKER_CONECTADO, buffer, tamanioBuffer, cliente->socket);

			if (r > 0)
				log_info(logger, "Se conectó un cliente correctamente (id: %d)", clienteBroker->id);
			else
				log_error(logger, "Error al conectar cliente");
		}
	}
}

void Operacion_SUSCRIBIRSE(Cliente* cliente, Paquete* paqueteRecibido)
{
	if (cliente->info == NULL)
	{
		// TODO
		log_error(logger, "El cliente no está conectado!");
		return;
	}

	BROKER_DATOS_SUSCRIBIRSE datosSuscripcion;
	Deserializar_BROKER_SUSCRIBIRSE(paqueteRecibido, &datosSuscripcion);
	Cola* cola = ObtenerCola(datosSuscripcion.cola);
	AgregarSuscriptor(cola, (ClienteBroker*)cliente->info);
	log_info(logger, "Nueva suscripción: %d", datosSuscripcion.cola);
}

void Operacion_NEW_POKEMON(Cliente* cliente, Paquete* paqueteRecibido) {}
void Operacion_APPEARED_POKEMON(Cliente* cliente, Paquete* paqueteRecibido) {}
void Operacion_CATCH_POKEMON(Cliente* cliente, Paquete* paqueteRecibido) {}
void Operacion_CAUGHT_POKEMON(Cliente* cliente, Paquete* paqueteRecibido) {}
void Operacion_GET_POKEMON(Cliente* cliente, Paquete* paqueteRecibido)
{
	DATOS_GET_POKEMON datos;
	if (!Deserializar_GET_POKEMON(paqueteRecibido, &datos))
		log_error(logger, "Error al deserializar GET_POKEMON");
	else
		log_info(logger, "GET_POKEMON: %s", datos.pokemon);
}
void Operacion_LOCALIZED_POKEMON(Cliente* cliente, Paquete* paqueteRecibido) {}
