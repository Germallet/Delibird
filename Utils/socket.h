#include "eventos.h"
#include <netdb.h>
#include <pthread.h>

extern int Socket_CrearCliente(char *ip, char* puerto, Eventos* eventos);
extern int Socket_Enviar(uint32_t codigoOperacion, void* stream, int tamanio, int numSocket);
extern int Socket_IniciarEscucha(uint16_t puerto, Eventos* eventos);
extern void Socket_LiberarConexion(int numSocket);
