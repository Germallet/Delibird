#include "mensajes.h"
#include "gameCard.h"

Servidor* servidor;

void SuscribirseColas(Cliente* cliente) {

	Eventos_AgregarOperacion(cliente->eventos, BROKER_CONECTADO, (EventoOperacion) &ConexionColas);
	Eventos_AgregarOperacion(cliente->eventos, NEW_POKEMON, (EventoOperacion)&Recibir_NEW_POKEMON);
	Eventos_AgregarOperacion(cliente->eventos, CATCH_POKEMON, (EventoOperacion)&Recibir_CATCH_POKEMON);
	Eventos_AgregarOperacion(cliente->eventos, GET_POKEMON, (EventoOperacion)&Recibir_GET_POKEMON);

	if (Socket_Enviar(BROKER_CONECTAR, NULL, 0, cliente->socket) < 0) {
		free(cliente->info);
		TerminarProgramaConError("ERROR EN CONEXION CON EL BROKER");
	}
}

void SocketEscucha(char* ip, int puerto) {

	Eventos* eventos = Eventos_Crear0();

	Eventos_AgregarOperacion(eventos, NEW_POKEMON, (EventoOperacion) &Recibir_NEW_POKEMON);
	Eventos_AgregarOperacion(eventos, CATCH_POKEMON, (EventoOperacion) &Recibir_CATCH_POKEMON);
	Eventos_AgregarOperacion(eventos, GET_POKEMON, (EventoOperacion) &Recibir_GET_POKEMON);

	servidor = CrearServidor(ip,puerto,eventos);
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

void Recibir_NEW_POKEMON(Cliente* cliente, Paquete* paqueteRecibido) {
	Stream* stream = Stream_CrearLecturaPaquete(paqueteRecibido);
	uint32_t id = Deserializar_uint32(stream);
	DATOS_NEW_POKEMON_ID datos;
	datos.datos = Deserializar_NEW_POKEMON(stream);
	datos.id = id;

	if (stream->error)
		log_error(logger,"Error al deserializar NEW_POKEMON");
	else {
		log_info(logger,"Tu mensaje llego correctamente");
		EnviarID(cliente,id);
		pthread_t thread;
		pthread_create(&thread, NULL, (void*) Operacion_NEW_POKEMON,&datos);
		pthread_detach(thread);
	}
}

/*
 * ver si el directorio del tipo de pokemon ya esta en nuestro fs
 * si esta => fijarse si ya hay uno de esos pokemones en esas coords
 * 			si hay => sumarle uno a lo de despues del =
 * 			si no hay => fijarse si hay espacio en el ultimo bloque que queda para agregar una linea mas
 * 						si hay => agregar la linea (posX - posY) = 1
 * 						si no hay => pedir otro bloque y agregar la linea
 * si no esta => armar el directorio de ese tipo de pokemon y adentro poner los datos (metadata, bloques, coords, etc.)
 */

void Operacion_NEW_POKEMON(DATOS_NEW_POKEMON_ID* datos) {

	NodoArbol* nodoPokemon = encontrarPokemon(datos->datos.pokemon);

	if(nodoPokemon == NULL) {
		nodoPokemon = crearPokemon(datos->datos.pokemon);
		agregarNodo(directorioFiles(),nodoPokemon);
	}

	FILE* filePokemon = fopen(strcat(pathDeNodo(nodoPokemon),"/metadata.bin"),"ab+");

	//TODO VER QUE NADIE ESTE ABRIENDO EL ARCHIVO

	int cantBloques = 0;

	t_list* numerosBloques = leerBlocks(pathDeNodo(nodoPokemon), &cantBloques);

	t_list* bloquesConvertidos = convertirBloques(numerosBloques,cantBloques);

	DatosBloques posYCant;

	posYCant.cantidad = datos->datos.cantidad;
	posYCant.pos.posX = datos->datos.posicion.posX;
	posYCant.pos.posY = datos->datos.posicion.posY;

	agregarCantidadEnPosicion(bloquesConvertidos,posYCant,numerosBloques,64);

	Enviar_APPEARED_POKEMON(datos);

	fclose(filePokemon);

	list_destroy_and_destroy_elements(numerosBloques,&free);
	list_destroy_and_destroy_elements(bloquesConvertidos,&free);
}

void Recibir_CATCH_POKEMON(Cliente* cliente, Paquete* paqueteRecibido) {
	Stream* stream = Stream_CrearLecturaPaquete(paqueteRecibido);
	uint32_t id = Deserializar_uint32(stream);
	DATOS_CATCH_POKEMON datos = Deserializar_CATCH_POKEMON(stream);

	if (stream->error)
		log_error(logger, "Error al deserializar CATCH_POKEMON");
	else
		EnviarID(cliente,id);

	pthread_t thread;
	pthread_create(&thread, NULL, (void*) Operacion_CATCH_POKEMON,&datos);
	pthread_detach(thread);
}

void Operacion_CATCH_POKEMON(DATOS_CATCH_POKEMON* datos) {
	log_info(logger,"llego");
	//TODO hacer lo que se tenga que hacer con el CATCH_POKEMON
	// HAY QUE MANDARLE AL BROKER UN MENSAJE DE CAUGHT
	Enviar_CAUGHT_POKEMON(/*PARAMETROS*/);
}

void Recibir_GET_POKEMON(Cliente* cliente, Paquete* paqueteRecibido) {
	Stream* stream = Stream_CrearLecturaPaquete(paqueteRecibido);
	uint32_t id = Deserializar_uint32(stream);
	DATOS_GET_POKEMON datos = Deserializar_GET_POKEMON(stream);

	if (stream->error)
		log_error(logger, "Error al deserializar GET_POKEMON");
	else
		EnviarID(cliente,id);

	pthread_t thread;
	pthread_create(&thread, NULL, (void*) Operacion_GET_POKEMON,&datos);
	pthread_detach(thread);
}

void Operacion_GET_POKEMON(DATOS_GET_POKEMON* datos) {

	log_info(logger,"llego");
	//TODO hacer lo que se tenga que hacer con el GET_POKEMON
	// HAY QUE MANDARLE AL BROKER UN MENSAJE DE LOCALIZED
	Enviar_LOCALIZED_POKEMON(/*PARAMETROS*/);
}

void EnviarID(Cliente* cliente, uint32_t identificador)
{
	DATOS_ID_MENSAJE* id_mensaje = malloc(sizeof(DATOS_ID_MENSAJE));

	id_mensaje->id = identificador;
	EnviarMensaje(cliente, BROKER_ACK, id_mensaje, (void*) &SerializarM_ID_MENSAJE);

	free(id_mensaje);
}


void Enviar_APPEARED_POKEMON(DATOS_NEW_POKEMON_ID* datos) {
	DATOS_APPEARED_POKEMON* datosEnviar = malloc(sizeof(DATOS_APPEARED_POKEMON));

	datosEnviar->idCorrelativa = datos->id;
	datosEnviar->pokemon = datos->datos.pokemon;
	datosEnviar->posicion = datos->datos.posicion;

	EnviarMensaje(clienteBroker, APPEARED_POKEMON, datosEnviar, (void*) &SerializarM_APPEARED_POKEMON);
}


void Enviar_CAUGHT_POKEMON(DATOS_CATCH_POKEMON* datos) {

}


void Enviar_LOCALIZED_POKEMON(DATOS_GET_POKEMON* datos,Posicion* posiciones) {

}
