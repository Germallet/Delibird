#include <commons/collections/dictionary.h>
#include "paquete.h"

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

typedef void (*eventoOperacion)(DatosConexion*, Paquete*);

extern t_dictionary* Operaciones_CrearDiccionario();
extern void Operaciones_AgregarOperacion(t_dictionary* diccionario, CodigoDeOperacion codigoDeOperacion, eventoOperacion evento);
extern bool Operaciones_TieneEvento(t_dictionary* diccionario, CodigoDeOperacion codigoDeOperacion);
extern eventoOperacion Operaciones_ObtenerEvento(t_dictionary* diccionario, CodigoDeOperacion codigoDeOperacion);
