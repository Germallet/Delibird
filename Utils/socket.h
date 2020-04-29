#pragma once
#include "paquete.h"
#include "eventos.h"
#include "net.h"

int Socket_Crear(int domain, int type, int protocol);
struct sockaddr_in* Socket_Conectar(int socket, char* ip, char* puerto);
Servidor* Socket_Escuchar(char* ip, int socket, uint16_t puerto, Eventos* eventos);
int Socket_RecibirPaquete(int numSocket, Paquete** paquete);
int Socket_Enviar(uint32_t codigoOperacion, void* stream, int tamanio, int numSocket);
Cliente* Socket_AceptarConexion(Servidor* servidor);
void Socket_Cerrar(int socket);
void Socket_Destruir(int socket);
