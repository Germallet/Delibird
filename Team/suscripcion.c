#include "team.h"
#include "pokemon.h"
#include "entrenador.h"
#include "suscripcion.h"
#include "interrupciones.h"
#include <stdlib.h>
#include "../Utils/conexionBroker.h"
#include "../Utils/serializacion.h"
#include "../Utils/socket.h"

t_list* id_mensajes_esperados;

bool espero_mensaje(uint32_t id_mensaje);
//-----------OPERACIONES-----------//
void operacion_CAUGHT_POKEMON(Cliente* cliente, Paquete* paquete)
{
	Stream* stream_lectura = Stream_CrearLecturaPaquete(paquete);
	Deserializar_uint32(stream_lectura);
	uint32_t id_catch = Deserializar_uint32(stream_lectura);
	DATOS_CAUGHT_POKEMON* datos = malloc(sizeof(DATOS_CAUGHT_POKEMON));
	*datos = Deserializar_CAUGHT_POKEMON(stream_lectura);

	if(espero_mensaje(id_catch))
	{
		datos_interrupcion_CAUGHT_POKEMON* datos_interrupcion = malloc(sizeof(datos_interrupcion_CAUGHT_POKEMON));
		datos_interrupcion->entrenador = cliente->info;
		datos_interrupcion->recibidos = datos;
		agregar_interrupcion(CAUGHT_POKEMON, datos_interrupcion);
	}

	Stream_DestruirConBuffer(stream_lectura);
}
void operacion_APPEARED_POKEMON(Cliente* cliente, Paquete* paquete)
{
	Stream* stream_lectura = Stream_CrearLecturaPaquete(paquete);
	Deserializar_uint32(stream_lectura);

	DATOS_APPEARED_POKEMON* datos = malloc(sizeof(DATOS_APPEARED_POKEMON));
	*datos = Deserializar_APPEARED_POKEMON(stream_lectura);
	agregar_interrupcion(APPEARED_POKEMON, datos);

	Stream_DestruirConBuffer(stream_lectura);
}
void operacion_LOCALIZED_POKEMON(Cliente* cliente, Paquete* paquete)
{
	Stream* stream_lectura = Stream_CrearLecturaPaquete(paquete);
	Deserializar_uint32(stream_lectura);
	DATOS_LOCALIZED_POKEMON* datos = malloc(sizeof(DATOS_LOCALIZED_POKEMON));
	*datos = Deserializar_LOCALIZED_POKEMON(stream_lectura);

	char* pokemon = datos->pokemon;

	if(necesito_especie_pokemon(pokemon) && !esta_localizada(pokemon))
		agregar_interrupcion(LOCALIZED_POKEMON, datos);

	Stream_DestruirConBuffer(stream_lectura);
}
void operacion_ASIGNACION_ID(Cliente* cliente, Paquete* paquete)
{
	DATOS_ID_MENSAJE datos;
	if(!DeserializarM_ID_MENSAJE(paquete, &datos)) log_error(logger, "Error al intentar deserializar el mensaje.");

	uint32_t* id_mensaje = malloc(sizeof(uint32_t));
	*id_mensaje = datos.id;

	list_add(id_mensajes_esperados, &(datos.id));
	Entrenador* entrenador = cliente->info;
	asignar_id_mensaje_espera(entrenador, id_mensaje);
}

//-----------SUSCRIPCION-----------//
Cliente* crear_cliente_de_broker()
{
	Eventos* eventos = Eventos_Crear0();
	Eventos_AgregarOperacion(eventos, BROKER_ID_MENSAJE, (EventoOperacion) &operacion_ASIGNACION_ID);
	return CrearCliente(config_get_string_value(config, "IP_BROKER"), config_get_int_value(config, "PUERTO_BROKER"),  eventos);
}

static void conectarse_y_suscribirse_a_cola(CodigoDeOperacion cod_op, EventoOperacion operacion, CodigoDeCola cola)
{
	Eventos* eventos = Eventos_Crear0();
	Eventos_AgregarOperacion(eventos, cod_op, operacion);

	void operacion_CONECTADO(Cliente* cliente)
	{
		BROKER_DATOS_SUSCRIBIRSE datos_broker;
		datos_broker.cola = cola;
		Stream* stream = SerializarM_BROKER_SUSCRIBIRSE(&datos_broker);

		Socket_Enviar(BROKER_SUSCRIBIRSE, stream->base, stream->tamanio, cliente->socket);
	}

	ConectarseABroker(config_get_string_value(config, "IP_BROKER"), config_get_int_value(config, "PUERTO_BROKER"), eventos, &operacion_CONECTADO, config_get_int_value(config, "TIEMPO_RECONEXION"));
}

void conectarse_y_suscribirse_a_colas()
{
	conectarse_y_suscribirse_a_cola(CAUGHT_POKEMON, (EventoOperacion) &operacion_CAUGHT_POKEMON, COLA_CAUGHT_POKEMON);
	conectarse_y_suscribirse_a_cola(APPEARED_POKEMON, (EventoOperacion) &operacion_APPEARED_POKEMON, COLA_APPEARED_POKEMON);
	conectarse_y_suscribirse_a_cola(LOCALIZED_POKEMON, (EventoOperacion) &operacion_LOCALIZED_POKEMON, COLA_LOCALIZED_POKEMON);
}

//-----------GET POKEMON-----------//
void solicitar_pokemons_para_objetivo_global()
{
	for(int i=0;i<pokemons_necesarios->elements_count;i++)
	{
		char* especie_pokemon = ((Pokemon*) list_get(pokemons_necesarios, i))->especie;

		DATOS_GET_POKEMON* datos = malloc(sizeof(DATOS_GET_POKEMON));
		datos->pokemon = especie_pokemon;

		Cliente* cliente = crear_cliente_de_broker();

		if(cliente != NULL)
			EnviarMensaje(cliente, GET_POKEMON, datos, (Serializador) &SerializarM_GET_POKEMON);
		else
			;//TODO
	}
}

//-----------OTRAS FUNCIONES-----------//
bool espero_mensaje(uint32_t id_mensaje)
{
	bool es_mismo_id(void* id) { return (uint32_t) id==id_mensaje; }
	return list_any_satisfy(id_mensajes_esperados, &es_mismo_id);
}
