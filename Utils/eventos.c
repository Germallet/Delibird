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
	eventos->operaciones = dictionary_create();
	return eventos;
}

void Eventos_AgregarOperacion(Eventos* eventos, CodigoDeOperacion codigoDeOperacion, EventoOperacion evento)
{
	char stringCodigo[] = {codigoDeOperacion, '\0'};
	dictionary_put(eventos->operaciones, stringCodigo, evento);
}
bool Eventos_TieneOperacion(Eventos* eventos, CodigoDeOperacion codigoDeOperacion)
{
	char stringCodigo[] = {codigoDeOperacion, '\0'};
	return (EventoOperacion)dictionary_has_key(eventos->operaciones, stringCodigo);
}
EventoOperacion Eventos_ObtenerOperacion(Eventos* eventos, CodigoDeOperacion codigoDeOperacion)
{
	char stringCodigo[] = {codigoDeOperacion, '\0'};
	return (EventoOperacion)dictionary_get(eventos->operaciones, stringCodigo);
}
void Eventos_Destruir(Eventos* eventos)
{
	dictionary_destroy(eventos->operaciones);
	free(eventos);
}
