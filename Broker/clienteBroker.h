#pragma once
#include "mensaje.h"
#include "../Utils/net.h"
#include <commons/collections/list.h>

typedef struct
{
	Cliente* cliente;
	uint32_t id;
} ClienteBroker;

ClienteBroker* CrearClienteBroker(Cliente* cliente);
ClienteBroker* ObtenerClienteBroker(uint32_t id);
