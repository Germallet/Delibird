#include "mensajes.h"

//Servidor* servidor;
Eventos* eventos;

void SocketEscucha() {

}


void SuscribirseColas(Cliente* cliente) {
	eventos = Eventos_Crear0();

	Eventos_AgregarOperacion(eventos, BROKER_CONECTADO, (EventoOperacion) &ConexionColas);
	Eventos_AgregarOperacion(eventos, NEW_POKEMON, (EventoOperacion)&Operacion_NEW_POKEMON);
	Eventos_AgregarOperacion(eventos, CATCH_POKEMON, (EventoOperacion)&Operacion_CATCH_POKEMON);
	Eventos_AgregarOperacion(eventos, GET_POKEMON, (EventoOperacion)&Operacion_GET_POKEMON);

	if (Socket_Enviar(BROKER_CONECTAR, NULL, 0, cliente->socket) < 0) {
		free(cliente->info);
		TerminarProgramaConError("ERROR EN CONEXION CON EL BROKER");
	}

}

void ConexionColas(Cliente* cliente, Paquete* paquete) {
	BROKER_DATOS_SUSCRIBIRSE datosNEW;
	BROKER_DATOS_SUSCRIBIRSE datosCATCH;
	BROKER_DATOS_SUSCRIBIRSE datosGET;

	datosNEW.cola = COLA_NEW_POKEMON;
	datosCATCH.cola = COLA_CATCH_POKEMON;
	datosGET.cola = COLA_GET_POKEMON;

	EnviarMensajeSinFree(cliente, BROKER_SUSCRIBIRSE, &datosNEW, (void*) &SerializarM_BROKER_SUSCRIBIRSE);
	EnviarMensajeSinFree(cliente, BROKER_SUSCRIBIRSE, &datosCATCH, (void*) &SerializarM_BROKER_SUSCRIBIRSE);
	EnviarMensajeSinFree(cliente, BROKER_SUSCRIBIRSE, &datosGET, (void*) &SerializarM_BROKER_SUSCRIBIRSE);

	free(cliente->info);
}


static void Recibir_NEW_POKEMON(Cliente* cliente, Paquete* paqueteRecibido) {
	Stream* stream = Stream_CrearLecturaPaquete(paqueteRecibido);
	uint32_t id = Deserializar_uint32(stream);
	DATOS_NEW_POKEMON datos = Deserializar_NEW_POKEMON(stream);

	if (stream->error)
		log_error(logger, "Error al deserializar NEW_POKEMON");
	else
		EnviarID(cliente,id);

	pthread_mutex_t mutexOperacion;
	pthread_mutex_init(&mutexOperacion,&Operacion_NEW_POKEMON(datos));
}

static void Operacion_NEW_POKEMON(DATOS_NEW_POKEMON datos) {
	//TODO hacer lo que se tenga que hacer con el NEW_POKEMON
}

static void Operacion_CATCH_POKEMON(Cliente* cliente, Paquete* paqueteRecibido) {
	Stream* stream = Stream_CrearLecturaPaquete(paqueteRecibido);
	uint32_t id = Deserializar_uint32(stream);
	DATOS_CATCH_POKEMON datos = Deserializar_CATCH_POKEMON(stream);

	if (stream->error)
		log_error(logger, "Error al deserializar CATCH_POKEMON");
	else
		EnviarID(cliente,id);

	pthread_mutex_t mutexOperacion;
	pthread_mutex_init(&mutexOperacion,&Operacion_CATCH_POKEMON(datos));
}

static void Operacion_CATCH_POKEMON(DATOS_CATCH_POKEMON datos) {
	//TODO hacer lo que se tenga que hacer con el CATCH_POKEMON
}

static void Operacion_GET_POKEMON(Cliente* cliente, Paquete* paqueteRecibido) {
	Stream* stream = Stream_CrearLecturaPaquete(paqueteRecibido);
	uint32_t id = Deserializar_uint32(stream);
	DATOS_GET_POKEMON datos = Deserializar_GET_POKEMON(stream);

	if (stream->error)
		log_error(logger, "Error al deserializar GET_POKEMON");
	else
		EnviarID(cliente,id);

	pthread_mutex_t mutexOperacion;
	pthread_mutex_init(&mutexOperacion,&Operacion_GET_POKEMON(datos));

}

static void Operacion_GET_POKEMON(DATOS_GET_POKEMON datos) {
	//TODO hacer lo que se tenga que hacer con el GET_POKEMON
}

void EnviarID(Cliente* cliente, uint32_t identificador)
{
	DATOS_ID_MENSAJE* id_mensaje = malloc(sizeof(DATOS_ID_MENSAJE));

	id_mensaje->id = identificador;
	log_error(logger, "Enviando ACK %d", id_mensaje->id);
	EnviarMensaje(cliente, BROKER_ACK, id_mensaje, (void*) &SerializarM_ID_MENSAJE);

	free(id_mensaje);

}
