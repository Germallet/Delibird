#include "../Utils/protocolo.h"

typedef struct
{
	int id;
	int posicionEnMemoria;
	// TODO: Variables temporales hasta verdadera implementación
	CodigoDeCola tipoDeMensaje;
	void* contenido;
} Mensaje;
