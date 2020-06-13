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

Cliente* crear_cliente_de_broker(Eventos* eventos);
void conectarse_y_suscribirse_a_colas();
void solicitar_pokemons_para_objetivo_global();

void operacion_APPEARED_POKEMON(Cliente* cliente, Paquete* paquete);
