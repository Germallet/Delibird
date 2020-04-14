#include <netdb.h>

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

int Socket_Crear(char *ip, char* puerto);
int Socket_Enviar(uint32_t codigoOperacion, void* stream, int tamanio, int numSocket);
int Socket_Recibir(int numSocket, Paquete** paquete);
int Socket_ProcesarPaquete(int numSocket, Paquete* paquete);
void Socket_LiberarPaquete(Paquete* paquete);
void Socket_LiberarConexion(int numSocket);