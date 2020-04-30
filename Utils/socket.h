#pragma once
#include "paquete.h"
#include "eventos.h"
#include "net.h"

int Socket_Crear(char* ip, uint16_t puerto);
struct sockaddr_in* Socket_Conectar(int socket, char* ip, uint16_t puerto);
Servidor* Socket_Escuchar(char* ip, int socket, uint16_t puerto, Eventos* eventos);
int Socket_RecibirPaquete(int numSocket, Paquete** paquete);
int Socket_Enviar(uint32_t codigoOperacion, void* stream, int tamanio, int numSocket);
Cliente* Socket_AceptarConexion(Servidor* servidor);
void Socket_Cerrar(int socket);
void Socket_Destruir(int socket);
