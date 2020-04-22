#include "eventos.h"

t_dictionary* Operaciones_CrearDiccionario()
{
	return dictionary_create();
}
void Operaciones_AgregarOperacion(t_dictionary* diccionario, CodigoDeOperacion codigoDeOperacion, eventoOperacion evento)
{
	char stringCodigo[] = {codigoDeOperacion, '\0'};
	dictionary_put(diccionario, stringCodigo, evento);
}
bool Operaciones_TieneEvento(t_dictionary* diccionario, CodigoDeOperacion codigoDeOperacion)
{
	char stringCodigo[] = {codigoDeOperacion, '\0'};
	return (eventoOperacion)dictionary_has_key(diccionario, stringCodigo);
}
eventoOperacion Operaciones_ObtenerEvento(t_dictionary* diccionario, CodigoDeOperacion codigoDeOperacion)
{
	char stringCodigo[] = {codigoDeOperacion, '\0'};
	return (eventoOperacion)dictionary_get(diccionario, stringCodigo);
}
