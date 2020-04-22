#include "eventos.h"
#include <netdb.h>
#include <pthread.h>

extern int Socket_Crear(char *ip, char* puerto);
extern int Socket_Enviar(uint32_t codigoOperacion, void* stream, int tamanio, int numSocket);
extern int Socket_Recibir(int numSocket, Paquete** paquete);
extern int Socket_ProcesarPaquete(int numSocket, Paquete* paquete);
extern void Socket_LiberarPaquete(Paquete* paquete);
extern void Socket_LiberarConexion(int numSocket);
extern int Socket_IniciarEscucha(uint16_t puerto, evento eventoConectado, evento eventoDesconectado, eventoError eventoError, t_dictionary* operaciones);
