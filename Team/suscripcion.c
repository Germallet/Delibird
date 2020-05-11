#include "team.h"
#include "pokemon.h"
#include "entrenador.h"
#include "suscripcion.h"
#include "interrupciones.h"
#include <stdlib.h>
#include "../Utils/socket.h"
#include "../Utils/protocolo.h"

BROKER_DATOS_CONECTADO datos_conexion;
t_list* id_mensajes_esperados;

bool espero_mensaje(uint32_t id_mensaje);
//-----------OPERACIONES-----------//
void operacion_CAUGHT_POKEMON(Cliente* cliente, Paquete* paquete)
{
	DATOS_CAUGHT_POKEMON datos;
	if(!Deserializar_CAUGHT_POKEMON(paquete, &datos)) log_error(logger, "Error al intentar deserializar el mensaje.");

	if(espero_mensaje(datos.idCatch))
	{
		Entrenador* entrenador = ((Entrenador*) (cliente->info));
		if(datos.capturado)
			capturo_pokemon(entrenador);
		else
			fallo_captura_pokemon(entrenador);
	}

	//TODO Agregar activacion interrupcion planificar
}
void operacion_APPEARED_POKEMON(Cliente* cliente, Paquete* paquete)
{
	DATOS_APPEARED_POKEMON datos;
	if(!Deserializar_APPEARED_POKEMON(paquete, &datos)) log_error(logger, "Error al intentar deserializar el mensaje.");

	if(necesito_especie_pokemon(datos.pokemon))
		agregar_pokemon_a_mapa(datos.pokemon, &datos.posicion);
	//TODO Agregar activacion interrupcion planificar
}
void operacion_LOCALIZED_POKEMON(Cliente* cliente, Paquete* paquete)
{
	DATOS_LOCALIZED_POKEMON datos;
	if(!Deserializar_LOCALIZED_POKEMON(paquete, &datos)) log_error(logger, "Error al intentar deserializar el mensaje.");

	if(necesito_especie_pokemon(datos.pokemon) && !esta_localizada(datos.pokemon))
	{
		for(int i=0;i<datos.cantidad;i++)
			agregar_pokemon_a_mapa(datos.pokemon, &datos.posiciones[i]);
		se_localizo(datos.pokemon);
		//TODO Agregar activacion interrupcion planificar
	}
}
void operacion_ASIGNACION_ID(Cliente* cliente, Paquete* paquete)
{
	DATOS_ID_MENSAJE datos;
	if(!Deserializar_ID_MENSAJE(paquete, &datos)) log_error(logger, "Error al intentar deserializar el mensaje.");

	list_add(id_mensajes_esperados, &datos.id); //TODO check this
	((Entrenador*) cliente->info)->id_mensaje_espera = datos.id ;
}
void operacion_CONECTADO(Cliente* cliente, Paquete* paquete)
{
	Deserializar_BROKER_CONECTADO(paquete, &datos_conexion);

	BROKER_DATOS_SUSCRIBIRSE datos_broker;
	datos_broker.cola = (CodigoDeCola) (cliente->info);
	int tamanio_datos_broker = 0;
	void* buffer = Serializar_BROKER_SUSCRIBIRSE(&datos_broker, &tamanio_datos_broker);
	Socket_Enviar(BROKER_SUSCRIBIRSE, buffer, tamanio_datos_broker, cliente->socket);

	free(cliente->info);
}

//-----------SUSCRIPCION-----------//
void suscribirse_a_cola(Datos_Suscripcion* datos_suscripcion)
{
	id_mensajes_esperados = list_create();

	Eventos* evento = Eventos_Crear0();
	Eventos_AgregarOperacion(evento, datos_suscripcion->codigo_operacion, (EventoOperacion) (datos_suscripcion->operacion));

	CodigoDeCola* codigo_cola = malloc(sizeof(CodigoDeCola));
	*codigo_cola = datos_suscripcion->cola; // TODO

	datos_suscripcion->cliente->info = codigo_cola;

	Eventos_AgregarOperacion(evento, BROKER_CONECTADO, (EventoOperacion) &operacion_CONECTADO);
	datos_suscripcion->cliente = CrearCliente(config_get_string_value(config, "IP_BROKER"), config_get_int_value(config, "PUERTO_BROKER"),  evento);

	if(datos_suscripcion->cliente == NULL)
		;//Reconectar

	Socket_Enviar(BROKER_CONECTAR, NULL, 0, datos_suscripcion->cliente->socket);

	free(datos_suscripcion);
}

void conectarse_y_suscribirse_a_colas()
{
	Datos_Suscripcion* datos_suscripcion_CAUGHT_POKEMON = malloc(sizeof(Datos_Suscripcion));
	datos_suscripcion_CAUGHT_POKEMON->codigo_operacion = CAUGHT_POKEMON;
	datos_suscripcion_CAUGHT_POKEMON->cola = COLA_CAUGHT_POKEMON;
	datos_suscripcion_CAUGHT_POKEMON->operacion = &operacion_CAUGHT_POKEMON;

	Datos_Suscripcion* datos_suscripcion_APPEARED_POKEMON = malloc(sizeof(Datos_Suscripcion));
	datos_suscripcion_APPEARED_POKEMON->codigo_operacion = APPEARED_POKEMON;
	datos_suscripcion_APPEARED_POKEMON->cola = COLA_APPEARED_POKEMON;
	datos_suscripcion_APPEARED_POKEMON->operacion = &operacion_APPEARED_POKEMON;

	Datos_Suscripcion* datos_suscripcion_LOCALIZED_POKEMON = malloc(sizeof(Datos_Suscripcion));
	datos_suscripcion_LOCALIZED_POKEMON->codigo_operacion = LOCALIZED_POKEMON;
	datos_suscripcion_LOCALIZED_POKEMON->cola = COLA_LOCALIZED_POKEMON;
	datos_suscripcion_LOCALIZED_POKEMON->operacion = &operacion_LOCALIZED_POKEMON;

	suscribirse_a_cola(datos_suscripcion_CAUGHT_POKEMON);
	suscribirse_a_cola(datos_suscripcion_APPEARED_POKEMON);
	suscribirse_a_cola(datos_suscripcion_LOCALIZED_POKEMON);
}
//-----------OTRAS FUNCIONES-----------//
bool espero_mensaje(uint32_t id_mensaje)
{
	bool es_mismo_id(void* id) { return (uint32_t) id==id_mensaje; }
	return list_any_satisfy(id_mensajes_esperados, &es_mismo_id);
}
