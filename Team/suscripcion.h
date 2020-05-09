#include "../Utils/net.h"
#include "../Utils/eventos.h"
#include "../Utils/protocolo.h"

typedef struct
{
	Cliente* cliente;
	void* operacion;
	CodigoDeOperacion codigo_operacion;
	CodigoDeCola cola;
}Datos_Suscripcion;
