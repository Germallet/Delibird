#include "team.h"
#include "pokemon.h"
#include "entrenador.h"
#include "suscripcion.h"
#include "interrupciones.h"
#include <stdlib.h>
#include "../Utils/net.c"
#include "../Utils/socket.h"
#include "../Utils/hiloTimer.h"
#include "../Utils/protocolo.c"

BROKER_DATOS_CONECTADO datos_conexion;
t_list* id_mensajes_esperados;
HiloTimer hilo_reconexion;

bool espero_mensaje(uint32_t id_mensaje);
//-----------OPERACIONES-----------//
void operacion_CAUGHT_POKEMON(Cliente* cliente, Paquete* paquete)
{
	DATOS_CAUGHT_POKEMON_ID* datos = malloc(sizeof(DATOS_CAUGHT_POKEMON_ID));
	if(!Deserializar_CAUGHT_POKEMON_ID(paquete, datos)) log_error(logger, "Error al intentar deserializar el mensaje.");

	if(espero_mensaje(datos->idCorrelativo_CATCH))
	{
		datos_interrupcion_CAUGHT_POKEMON* datos_interrupcion = malloc(sizeof(datos_interrupcion_CAUGHT_POKEMON));
		datos_interrupcion->entrenador = cliente->info;
		datos_interrupcion->recibidos = datos;
		agregar_interrupcion(CAUGHT_POKEMON, datos_interrupcion);
	}
}
void operacion_APPEARED_POKEMON(Cliente* cliente, Paquete* paquete)
{
	DATOS_APPEARED_POKEMON* datos = malloc(sizeof(DATOS_APPEARED_POKEMON));
	if(!Deserializar_APPEARED_POKEMON(paquete, datos)) log_error(logger, "Error al intentar deserializar el mensaje.");

	if(necesito_especie_pokemon(datos->pokemon))
		agregar_interrupcion(APPEARED_POKEMON, datos);
}
void operacion_LOCALIZED_POKEMON(Cliente* cliente, Paquete* paquete)
{
	DATOS_LOCALIZED_POKEMON_ID* datos = malloc(sizeof(DATOS_LOCALIZED_POKEMON_ID));
	if(!Deserializar_LOCALIZED_POKEMON_ID(paquete, datos)) log_error(logger, "Error al intentar deserializar el mensaje.");

	if(necesito_especie_pokemon(datos->pokemon) && !esta_localizada(datos->pokemon))
		agregar_interrupcion(LOCALIZED_POKEMON, datos);
}
void operacion_ASIGNACION_ID(Cliente* cliente, Paquete* paquete)
{
	DATOS_ID_MENSAJE datos;
	if(!Deserializar_ID_MENSAJE(paquete, &datos)) log_error(logger, "Error al intentar deserializar el mensaje.");

	uint32_t* id_mensaje = malloc(sizeof(uint32_t));
	*id_mensaje = datos.id;

	list_add(id_mensajes_esperados, &(datos.id));
	Entrenador* entrenador = cliente->info;
	asignar_id_mensaje_espera(entrenador, id_mensaje);
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
Cliente* crear_cliente_de_broker(Eventos* eventos) { return CrearCliente(config_get_string_value(config, "IP_BROKER"), config_get_int_value(config, "PUERTO_BROKER"),  eventos);}

void suscribirse_a_cola(Datos_Suscripcion* datos_suscripcion)
{
	id_mensajes_esperados = list_create();

	Eventos* eventos = Eventos_Crear0();
	Eventos_AgregarOperacion(eventos, datos_suscripcion->codigo_operacion, (EventoOperacion) (datos_suscripcion->operacion));

	CodigoDeCola* codigo_cola = malloc(sizeof(CodigoDeCola));
	*codigo_cola = datos_suscripcion->cola;

	datos_suscripcion->cliente->info = codigo_cola;

	Eventos_AgregarOperacion(eventos, BROKER_CONECTADO, (EventoOperacion) &operacion_CONECTADO);
	datos_suscripcion->cliente = crear_cliente_de_broker(eventos);

	if(datos_suscripcion->cliente == NULL)
	{
		//INTENTAR CONEXION NUEVAMENTE
	}

	Socket_Enviar(BROKER_CONECTAR, NULL, 0, datos_suscripcion->cliente->socket);

	free(datos_suscripcion);
}

void reconectar_si_es_necesario(void* dato)
{

}

//void inicializar_hilo_reconexion() { hilo_reconexion = CrearHiloTimer(-1, config_get_int_value(config, "TIEMPO_RECONEXION"), reconectar_si_es_necesario, NULL); }

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

//-----------GET POKEMON-----------//
void solicitar_pokemons_para_objetivo_global()
{
	for(int i=0;i<pokemons_necesarios->elements_count;i++)
	{
		char* especie_pokemon = ((Pokemon*) list_get(pokemons_necesarios, i))->especie;
		uint32_t* largo = malloc(sizeof(uint32_t));
		*largo = strlen(especie_pokemon);

		DATOS_GET_POKEMON* datos = malloc(sizeof(DATOS_GET_POKEMON));
		datos->largoPokemon = *largo;
		datos->pokemon = especie_pokemon;

		EnviarMensaje(crear_cliente_de_broker(Eventos_Crear0()), GET_POKEMON, datos, (Serializador) &Serializar_GET_POKEMON);
	}
}

//-----------OTRAS FUNCIONES-----------//
bool espero_mensaje(uint32_t id_mensaje)
{
	bool es_mismo_id(void* id) { return (uint32_t) id==id_mensaje; }
	return list_any_satisfy(id_mensajes_esperados, &es_mismo_id);
}
