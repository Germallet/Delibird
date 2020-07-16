#include "team.h"
#include "pokemon.h"
#include "entrenador.h"
#include "suscripcion.h"
#include "interrupciones.h"
#include <stdlib.h>
#include "../Utils/serializacion.h"
#include "../Utils/socket.h"

ConexionBroker* conexionBroker;
t_list* id_mensajes_esperados;
pthread_mutex_t entrenadoresEsperandoCaught;
Entrenador* espero_mensaje(uint32_t id_mensaje);

void EnviarID(Cliente* cliente, uint32_t identificador)
{
	DATOS_ID_MENSAJE* id_mensaje = malloc(sizeof(DATOS_ID_MENSAJE));
	id_mensaje->id = identificador;
	EnviarMensaje(cliente, BROKER_ACK, id_mensaje, (void*) &SerializarM_ID_MENSAJE);
	free(id_mensaje);
}

//-----------OPERACIONES-----------//
void operacion_CAUGHT_POKEMON(Cliente* cliente, Paquete* paquete)
{
	Stream* stream_lectura = Stream_CrearLecturaPaquete(paquete);
	uint32_t identificador = Deserializar_uint32(stream_lectura);
	uint32_t id_catch = Deserializar_uint32(stream_lectura);
	DATOS_CAUGHT_POKEMON* datos = malloc(sizeof(DATOS_CAUGHT_POKEMON));
	*datos = Deserializar_CAUGHT_POKEMON(stream_lectura);

	Entrenador* e = espero_mensaje(id_catch);
	if (e != NULL)
	{
		datos_interrupcion_CAUGHT_POKEMON* datos_interrupcion = malloc(sizeof(datos_interrupcion_CAUGHT_POKEMON));
		datos_interrupcion->entrenador = e;
		datos_interrupcion->recibidos = datos;
		agregar_interrupcion(I_CAUGHT_POKEMON, datos_interrupcion);
	}
	else
		free(datos);

	Stream_Destruir(stream_lectura);
	EnviarID(cliente, identificador);
}
void operacion_APPEARED_POKEMON(Cliente* cliente, Paquete* paquete)
{
	Stream* stream_lectura = Stream_CrearLecturaPaquete(paquete);
	uint32_t identificador = Deserializar_uint32(stream_lectura);
	Deserializar_uint32(stream_lectura);

	DATOS_APPEARED_POKEMON* datos = malloc(sizeof(DATOS_APPEARED_POKEMON));
	*datos = Deserializar_APPEARED_POKEMON(stream_lectura);
	agregar_interrupcion(I_APPEARED_POKEMON, datos);

	Stream_Destruir(stream_lectura);
	EnviarID(cliente, identificador);
}
void operacion_LOCALIZED_POKEMON(Cliente* cliente, Paquete* paquete)
{
	Stream* stream_lectura = Stream_CrearLecturaPaquete(paquete);
	uint32_t identificador = Deserializar_uint32(stream_lectura);
	Deserializar_uint32(stream_lectura);
	DATOS_LOCALIZED_POKEMON* datos = malloc(sizeof(DATOS_LOCALIZED_POKEMON));
	*datos = Deserializar_LOCALIZED_POKEMON(stream_lectura);

	char* pokemon = datos->pokemon;

	if(necesito_especie_pokemon(pokemon) && !esta_localizada(pokemon))
		agregar_interrupcion(I_LOCALIZED_POKEMON, datos);
	else
	{
		free(datos->pokemon);
		if (datos->cantidad != 0)
			free(datos->posiciones);
		free(datos);
	}

	Stream_Destruir(stream_lectura);
	EnviarID(cliente, identificador);
}
void operacion_ID(Cliente* cliente, Paquete* paquete)
{
}

//-----------SUSCRIPCION-----------//
Cliente* crear_cliente_de_broker() { return CrearCliente(config_get_string_value(config, "IP_BROKER"), config_get_int_value(config, "PUERTO_BROKER"), Eventos_Crear0()); }

static void operacion_CONECTADO(Cliente* cliente, CodigoDeCola cola)
{
	BROKER_DATOS_SUSCRIBIRSE datos_broker;
	datos_broker.cola = cola;
	Stream* stream = SerializarM_BROKER_SUSCRIBIRSE(&datos_broker);

	Socket_Enviar(BROKER_SUSCRIBIRSE, stream->base, stream->tamanio, cliente->socket);
	Stream_DestruirConBuffer(stream);
}

/*static void operacion_CONECTADO_CAUGHT_POKEMON(Cliente* cliente) { operacion_CONECTADO(cliente, COLA_CAUGHT_POKEMON); }
static void operacion_CONECTADO_APPEARED_POKEMON(Cliente* cliente) { operacion_CONECTADO(cliente, COLA_APPEARED_POKEMON); }
static void operacion_CONECTADO_LOCALIZED_POKEMON(Cliente* cliente) { operacion_CONECTADO(cliente, COLA_LOCALIZED_POKEMON); }

static void conectarse_y_suscribirse_a_cola(CodigoDeOperacion cod_op, EventoOperacion operacion, void (*alConectarse)(Cliente*))
{
	Eventos* eventos = Eventos_Crear0();
	Eventos_AgregarOperacion(eventos, cod_op, operacion);
	ConectarseABroker(config_get_string_value(config, "IP_BROKER"), config_get_int_value(config, "PUERTO_BROKER"), eventos, alConectarse, NULL, config_get_int_value(config, "TIEMPO_RECONEXION"));
}

void conectarse_y_suscribirse_a_colas()
{
	conectarse_y_suscribirse_a_cola(CAUGHT_POKEMON, (EventoOperacion) &operacion_CAUGHT_POKEMON, &operacion_CONECTADO_CAUGHT_POKEMON);
	conectarse_y_suscribirse_a_cola(APPEARED_POKEMON, (EventoOperacion) &operacion_APPEARED_POKEMON, &operacion_CONECTADO_APPEARED_POKEMON);
	conectarse_y_suscribirse_a_cola(LOCALIZED_POKEMON, (EventoOperacion) &operacion_LOCALIZED_POKEMON, &operacion_CONECTADO_LOCALIZED_POKEMON);
}*/


static void Reconectado(Cliente* cliente)
{
	log_info(logger, "Conectado con el Broker!");
}
static void Suscribirse(Cliente* cliente)
{
	log_info(logger, "Conectado con el Broker!");
	operacion_CONECTADO(cliente, COLA_APPEARED_POKEMON);
	operacion_CONECTADO(cliente, COLA_CAUGHT_POKEMON);
	operacion_CONECTADO(cliente, COLA_LOCALIZED_POKEMON);
	pthread_mutex_unlock(&mutex_conectado);
}
static void ConexFallida()
{
	pthread_mutex_unlock(&mutex_conectado);
}

void conectarse()
{
	Eventos* eventos = Eventos_Crear0();
	Eventos_AgregarOperacion(eventos, CAUGHT_POKEMON, (EventoOperacion) &operacion_CAUGHT_POKEMON);
	Eventos_AgregarOperacion(eventos, APPEARED_POKEMON, (EventoOperacion) &operacion_APPEARED_POKEMON);
	Eventos_AgregarOperacion(eventos, LOCALIZED_POKEMON, (EventoOperacion) &operacion_LOCALIZED_POKEMON);

	conexionBroker = ConectarseABroker(config_get_string_value(config, "IP_BROKER"), config_get_int_value(config, "PUERTO_BROKER"), eventos, &Suscribirse, &Reconectado, &ConexFallida, config_get_int_value(config, "TIEMPO_RECONEXION"));
}

//-----------GET POKEMON-----------//
void solicitar_pokemons_para_objetivo_global()
{
	for(int i=0;i<pokemons_necesarios->elements_count;i++)
	{
		char* especie_pokemon = ((Pokemon*) list_get(pokemons_necesarios, i))->especie;
		Eventos* eventos = Eventos_Crear0();
		Eventos_AgregarOperacion(eventos, BROKER_ID_MENSAJE, (EventoOperacion) &operacion_ID);
		Cliente* cliente = CrearCliente(config_get_string_value(config, "IP_BROKER"), config_get_int_value(config, "PUERTO_BROKER"), eventos);

		if(cliente != NULL)
		{
			DATOS_GET_POKEMON* datos = malloc(sizeof(DATOS_GET_POKEMON));
			datos->pokemon = especie_pokemon;
			EnviarMensaje(cliente, GET_POKEMON, datos, (Serializador) &SerializarM_GET_POKEMON);
			free(datos);
			DestruirCliente(cliente);
		}
	}
}

//-----------OTRAS FUNCIONES-----------//
Entrenador* espero_mensaje(uint32_t id_mensaje)
{
	pthread_mutex_lock(&entrenadoresEsperandoCaught);
	for(int i = 0; i < id_mensajes_esperados->elements_count; i++)
	{
		Entrenador* e = list_get(id_mensajes_esperados, i);
		if (e->id_mensaje_espera == id_mensaje)
		{
			list_remove(id_mensajes_esperados, i);
			e->id_mensaje_espera = -1;
			pthread_mutex_unlock(&entrenadoresEsperandoCaught);
			return e;
		}
	}

	pthread_mutex_unlock(&entrenadoresEsperandoCaught);
	return NULL;
}
