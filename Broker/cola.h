#pragma once
#include "clienteBroker.h"
#include "mensaje.h"
#include <commons/collections/list.h>

typedef struct
{
	t_list* suscriptores;
	t_list* mensajes;
	pthread_mutex_t mutexSuscriptores;
	pthread_mutex_t mutexMensajes;
} Cola;

extern Cola *cola_NEW_POKEMON, *cola_APPEARED_POKEMON, *cola_CATCH_POKEMON, *cola_CAUGHT_POKEMON, *cola_GET_POKEMON, *cola_LOCALIZED_POKEMON;

void CrearColas();
void DestruirColas();
Cola* ObtenerCola(CodigoDeCola codigo);
void AgregarSuscriptor(Cola* cola, ClienteBroker* clienteBroker);
void RemoverSuscriptor(Cola* cola, ClienteBroker* clienteBroker);
void RemoverSuscriptorTotal(ClienteBroker* clienteBroker);
bool TieneSuscriptor(Cola* cola, ClienteBroker* clienteBroker);
bool CorresponderRecibirRespuesta(CodigoDeCola codigo, uint32_t idCorrelativo);
void Cola_ProcesarNuevoMensaje(CodigoDeCola codigoDeCola, Mensaje* mensaje);
void Cola_EnviarMensajesRestantesSiCorrespondeA(Cola* cola, CodigoDeCola codigoDeCola, ClienteBroker* cliente);
void Colas_EnviarMensajesRestantesSiCorrespondeA(ClienteBroker* cliente);
