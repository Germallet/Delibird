#include "codigoDeCola.h"
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

char* CodigoDeColaAString(CodigoDeCola codigoDeCola)
{
	switch(codigoDeCola)
	{
		case COLA_NEW_POKEMON:
			return "NEW_POKEMON";
		case COLA_APPEARED_POKEMON:
			return "APPEARED_POKEMON";
		case COLA_CATCH_POKEMON:
			return "CATCH_POKEMON";
		case COLA_CAUGHT_POKEMON:
			return "CAUGHT_POKEMON";
		case COLA_GET_POKEMON:
			return "GET_POKEMON";
		case COLA_LOCALIZED_POKEMON:
			return "LOCALIZED_POKEMON";
	}
	return "COLA_INVALIDA";
}

CodigoDeCola* StringACodigoDeCola(char* stringCola)
{
	bool sonIguales(char* a, char* b) { return strcmp(a, b) == 0; }

	CodigoDeCola* codigoDeCola = malloc(sizeof(CodigoDeCola));

	if (sonIguales(stringCola, "NEW_POKEMON"))
		*codigoDeCola = COLA_NEW_POKEMON;
	else if (sonIguales(stringCola, "APPEARED_POKEMON"))
		*codigoDeCola = COLA_APPEARED_POKEMON;
	else if (sonIguales(stringCola, "CATCH_POKEMON"))
		*codigoDeCola = COLA_CATCH_POKEMON;
	else if (sonIguales(stringCola, "CAUGHT_POKEMON"))
		*codigoDeCola = COLA_CAUGHT_POKEMON;
	else if (sonIguales(stringCola, "GET_POKEMON"))
		*codigoDeCola = COLA_GET_POKEMON;
	else if (sonIguales(stringCola, "LOCALIZED_POKEMON"))
		*codigoDeCola = COLA_LOCALIZED_POKEMON;
	else {
		free(codigoDeCola);
		return NULL;
	}

	return codigoDeCola;
}
