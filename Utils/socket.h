#include <netdb.h>
#include <pthread.h>
#include <commons/collections/dictionary.h>

typedef struct
{
	uint32_t codigoOperacion;
	uint32_t tamanio;
	void* stream;
} Paquete;

typedef enum
{
	MENSAJE = 1,
	NEW_POKEMON = 2,
	APPEARED_POKEMON = 3,
	CATCH_POKEMON = 4,
	CAUGHT_POKEMON = 5,
	GET_POKEMON = 6,
	LOCALLIZED_POKEMON = 7,
	SUSCRIPTOR = 8,
} CodigoDeOperacion;

typedef enum
{
	ERROR_RECIBIR = 1,
	ERROR_OPERACION_INVALIDA = 2,
	ERROR_PROCESAR_PAQUETE = 3
} ErrorDeEscucha;

typedef void (*evento)(void);
typedef void (*eventoError)(ErrorDeEscucha, Paquete*);

typedef struct
{
	evento conectado;
	evento desconectado;
	eventoError error;
	t_dictionary* operaciones;
} Eventos;

typedef struct
{
	int socket;
	struct sockaddr_in* direccion;
	Eventos eventos;
	pthread_t thread;
} DatosConexion;

extern int Socket_Crear(char *ip, char* puerto);
extern int Socket_Enviar(uint32_t codigoOperacion, void* stream, int tamanio, int numSocket);
extern int Socket_Recibir(int numSocket, Paquete** paquete);
extern int Socket_ProcesarPaquete(int numSocket, Paquete* paquete);
extern void Socket_LiberarPaquete(Paquete* paquete);
extern void Socket_LiberarConexion(int numSocket);
extern int Socket_IniciarEscucha(uint16_t puerto, void(*eventoConectado)(), void(*eventoDesconectado)(), void(*eventoError)(), t_dictionary* operaciones);
