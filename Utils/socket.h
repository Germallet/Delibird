#include <netdb.h>
#include <pthread.h>

typedef struct
{
	uint32_t codigoOperacion;
	uint32_t tamanio;
	void* stream;
} Paquete;

typedef enum
{
	MENSAJE = 1,
} CodigoDeOperacion;

extern int Socket_Crear(char *ip, char* puerto);
extern int Socket_Enviar(uint32_t codigoOperacion, void* stream, int tamanio, int numSocket);
extern int Socket_Recibir(int numSocket, Paquete** paquete);
extern int Socket_ProcesarPaquete(int numSocket, Paquete* paquete);
extern void Socket_LiberarPaquete(Paquete* paquete);
extern void Socket_LiberarConexion(int numSocket);
extern int Socket_IniciarEscucha(uint16_t puerto, void(*EventoNuevoCliente)());
