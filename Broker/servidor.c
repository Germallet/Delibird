#include "broker.h"
#include "../Utils/net.h"

void ClienteConectado();
void ClienteDesconectado();
void ClienteError(ErrorDeEscucha error, Paquete* paqueteRecibido);
void Operacion_SUSCRIPTOR(Cliente* cliente, Paquete* paqueteRecibido);
void Operacion_NEW_POKEMON(Cliente* cliente, Paquete* paqueteRecibido);
void Operacion_APPEARED_POKEMON(Cliente* cliente, Paquete* paqueteRecibido);
void Operacion_CATCH_POKEMON(Cliente* cliente, Paquete* paqueteRecibido);
void Operacion_CAUGHT_POKEMON(Cliente* cliente, Paquete* paqueteRecibido);
void Operacion_GET_POKEMON(Cliente* cliente, Paquete* paqueteRecibido);
void Operacion_LOCALIZED_POKEMON(Cliente* cliente, Paquete* paqueteRecibido);

void IniciarServidorBroker(char* ip, int puerto)
{
	Eventos* eventos = Eventos_Crear(&ClienteConectado, &ClienteDesconectado, &ClienteError);
	Eventos_AgregarOperacion(eventos, SUSCRIPTOR, &Operacion_SUSCRIPTOR);
	Eventos_AgregarOperacion(eventos, NEW_POKEMON, &Operacion_NEW_POKEMON);
	Eventos_AgregarOperacion(eventos, APPEARED_POKEMON, &Operacion_APPEARED_POKEMON);
	Eventos_AgregarOperacion(eventos, CATCH_POKEMON, &Operacion_CATCH_POKEMON);
	Eventos_AgregarOperacion(eventos, CAUGHT_POKEMON, &Operacion_CAUGHT_POKEMON);
	Eventos_AgregarOperacion(eventos, GET_POKEMON, &Operacion_GET_POKEMON);
	Eventos_AgregarOperacion(eventos, LOCALIZED_POKEMON, &Operacion_LOCALIZED_POKEMON);

	Servidor* servidor = CrearServidor(ip, puerto, eventos);

	if (servidor != NULL)
		log_info(logger, "Escucha iniciada (%s:%d)", ip, puerto);
	else
		log_error(logger, "Error al iniciar escucha (%s:%d)", ip, puerto);
}

void ClienteConectado()
{
	log_info(logger, "ClienteConectado");
}
void ClienteDesconectado()
{
	log_info(logger, "ClienteDesconectado");
}
void ClienteError(ErrorDeEscucha error, Paquete* paqueteRecibido)
{
	if (error == ERROR_RECIBIR)
		log_error(logger, "Error al recibir paquete. (Cod. op.: %d)", paqueteRecibido->codigoOperacion);
	else if (error == ERROR_OPERACION_INVALIDA)
		log_error(logger, "Recibido código de operación inválido. (Cod. op.: %d)", paqueteRecibido->codigoOperacion);
	else if (error == ERROR_PROCESAR_PAQUETE)
		log_error(logger, "Error al procesar paquete. (Cod. op.: %d)", paqueteRecibido->codigoOperacion);
}

void Operacion_SUSCRIPTOR(Cliente* cliente, Paquete* paqueteRecibido)
{
	log_info(logger, "SUSCRIPTOR");
}

void Operacion_NEW_POKEMON(Cliente* cliente, Paquete* paqueteRecibido)
{
	log_info(logger, "NEW_POKEMON");
}
void Operacion_APPEARED_POKEMON(Cliente* cliente, Paquete* paqueteRecibido)
{
	log_info(logger, "APPEARED_POKEMON");
}
void Operacion_CATCH_POKEMON(Cliente* cliente, Paquete* paqueteRecibido)
{
	log_info(logger, "CATCH_POKEMON");
}
void Operacion_CAUGHT_POKEMON(Cliente* cliente, Paquete* paqueteRecibido)
{
	log_info(logger, "CAUGHT_POKEMON");
}
void Operacion_GET_POKEMON(Cliente* cliente, Paquete* paqueteRecibido)
{
	log_info(logger, "GET_POKEMON");
}
void Operacion_LOCALIZED_POKEMON(Cliente* cliente, Paquete* paqueteRecibido)
{
	log_info(logger, "LOCALIZED_POKEMON");
}
