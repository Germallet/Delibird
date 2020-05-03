#include "eventos.h"
#include <stdlib.h>
#include <commons/log.h>

static void ClienteErrorDefault(ErrorDeEscucha error, Paquete* paqueteRecibido)
{
	uint32_t codOp = paqueteRecibido == NULL ? -1 : paqueteRecibido->codigoOperacion;
	t_log* logger = log_create("Utils.log", "Utils", true, LOG_LEVEL_INFO);
	if (error == ERROR_RECIBIR)
		log_error(logger, "Error al recibir paquete. (Cod. op.: %d)", codOp);
	else if (error == ERROR_OPERACION_INVALIDA)
		log_error(logger, "Recibido código de operación inválido. (Cod. op.: %d)", codOp);
	else if (error == ERROR_PROCESAR_PAQUETE)
		log_error(logger, "Error al procesar paquete. (Cod. op.: %d)", codOp);
	log_destroy(logger);
}

Eventos* Eventos_Crear0()
{
	return Eventos_Crear3(NULL, NULL, NULL);
}
Eventos* Eventos_Crear2(Evento conectado, Evento desconectado)
{
	return Eventos_Crear3(conectado, desconectado, NULL);
}
Eventos* Eventos_Crear3(Evento conectado, Evento desconectado, EventoError error)
{
	Eventos* eventos = malloc(sizeof(Eventos));
	eventos->conectado = conectado;
	eventos->desconectado = desconectado;
	eventos->error = error != NULL ? error : ClienteErrorDefault;
	eventos->operaciones = dictionaryInt_create();
	return eventos;
}

void Eventos_AgregarOperacion(Eventos* eventos, uint32_t codigoDeOperacion, EventoOperacion evento)
{
	dictionaryInt_put(eventos->operaciones, codigoDeOperacion, evento);
}
bool Eventos_TieneOperacion(Eventos* eventos, uint32_t codigoDeOperacion)
{
	return (EventoOperacion)dictionaryInt_has_key(eventos->operaciones, codigoDeOperacion);
}
EventoOperacion Eventos_ObtenerOperacion(Eventos* eventos, uint32_t codigoDeOperacion)
{
	return (EventoOperacion)dictionaryInt_get(eventos->operaciones, codigoDeOperacion);
}
void Eventos_Destruir(Eventos* eventos)
{
	dictionaryInt_destroy(eventos->operaciones);
	free(eventos);
}
