#pragma once
#include "../Utils/net.h"

typedef struct
{
	Cliente* cliente;
	uint32_t id;
} ClienteBroker;

ClienteBroker* CrearClienteBroker(Cliente* cliente);
ClienteBroker* ObtenerClienteBroker(uint32_t id);
