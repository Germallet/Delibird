#include "clienteBroker.h"
#include "memoria.h"
#include "../Utils/dictionaryInt.h"
#include <commons/config.h>
#include <stdlib.h>

t_dictionaryInt* clientes;
pthread_mutex_t mutexClientes;

uint32_t siguienteIDCliente = 0;
pthread_mutex_t mutexIDCliente;

void Inicializar()
{
	clientes = dictionaryInt_create();
	pthread_mutex_init(&mutexClientes, NULL);
	pthread_mutex_init(&mutexIDCliente, NULL);
}

ClienteBroker* CrearClienteBroker(Cliente* cliente)
{
	ClienteBroker* clienteBroker = malloc(sizeof(ClienteBroker));
	clienteBroker->cliente = cliente;

	pthread_mutex_lock(&mutexIDCliente);
	clienteBroker->id = siguienteIDCliente;
	siguienteIDCliente++;
	pthread_mutex_unlock(&mutexIDCliente);

	return clienteBroker;
}

void AgregarClienteBroker(ClienteBroker* cliente)
{
	pthread_mutex_lock(&mutexClientes);
	dictionaryInt_put(clientes, cliente->id, cliente);
	pthread_mutex_unlock(&mutexClientes);
}

ClienteBroker* ObtenerClienteBroker(uint32_t id)
{
	pthread_mutex_lock(&mutexClientes);
	ClienteBroker* clienteBroker = dictionaryInt_get(clientes, id);
	pthread_mutex_unlock(&mutexClientes);
	return clienteBroker;
}

void RemoverClienteBroker(uint32_t id)
{
	pthread_mutex_lock(&mutexClientes);
	dictionaryInt_remove(clientes, id);
	pthread_mutex_unlock(&mutexClientes);
}
