#pragma once

typedef enum
{
	COLA_NEW_POKEMON,		//0
	COLA_APPEARED_POKEMON,	//1
	COLA_CATCH_POKEMON,		//2
	COLA_CAUGHT_POKEMON,	//3
	COLA_GET_POKEMON,		//4
	COLA_LOCALIZED_POKEMON	//5
} CodigoDeCola;

extern char* CodigoDeColaAString(CodigoDeCola codigoDeCola);
extern CodigoDeCola* StringACodigoDeCola(char* stringCola);
