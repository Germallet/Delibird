#include "eventos.h"
#include <stdlib.h>

Eventos* Eventos_Crear(Evento conectado, Evento desconectado, EventoError error)
{
	Eventos* eventos = malloc(sizeof(Eventos));
	eventos->conectado = conectado;
	eventos->desconectado = desconectado;
	eventos->error = error;
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
