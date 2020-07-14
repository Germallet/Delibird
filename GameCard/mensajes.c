#include "mensajes.h"
#include "gameCard.h"

void SocketEscucha(char* ip, int puerto) {

	Eventos* eventos = Eventos_Crear0();

	Eventos_AgregarOperacion(eventos, NEW_POKEMON, (EventoOperacion) &Recibir_NEW_POKEMON);
	Eventos_AgregarOperacion(eventos, CATCH_POKEMON, (EventoOperacion) &Recibir_CATCH_POKEMON);
	Eventos_AgregarOperacion(eventos, GET_POKEMON, (EventoOperacion) &Recibir_GET_POKEMON);

	servidor = CrearServidor(ip,puerto,eventos);
}

void ConexionColas(Cliente* cliente) {
	BROKER_DATOS_SUSCRIBIRSE datosNEW;
	BROKER_DATOS_SUSCRIBIRSE datosCATCH;
	BROKER_DATOS_SUSCRIBIRSE datosGET;

	datosNEW.cola = COLA_NEW_POKEMON;
	datosCATCH.cola = COLA_CATCH_POKEMON;
	datosGET.cola = COLA_GET_POKEMON;

	EnviarMensaje(cliente, BROKER_SUSCRIBIRSE, &datosNEW, (void*) &SerializarM_BROKER_SUSCRIBIRSE);
	EnviarMensaje(cliente, BROKER_SUSCRIBIRSE, &datosCATCH, (void*) &SerializarM_BROKER_SUSCRIBIRSE);
	EnviarMensaje(cliente, BROKER_SUSCRIBIRSE, &datosGET, (void*) &SerializarM_BROKER_SUSCRIBIRSE);
}

void Recibir_NEW_POKEMON(Cliente* cliente, Paquete* paqueteRecibido) {
	Stream* stream = Stream_CrearLecturaPaquete(paqueteRecibido);
	uint32_t id = Deserializar_uint32(stream);
	DATOS_NEW_POKEMON_ID* datos = malloc(sizeof(DATOS_NEW_POKEMON_ID));
	datos->datos = Deserializar_NEW_POKEMON(stream);
	datos->id = id;

	if (stream->error)
		log_error(logger,"Error al deserializar NEW_POKEMON");
	else {
		log_info(logger,"NEW_POKEMON recibido");
		EnviarID(cliente,id);
		pthread_t thread;
		pthread_create(&thread, NULL, (void*) Operacion_NEW_POKEMON,datos);
		pthread_detach(thread);
	}
	free(stream);
}

void Operacion_NEW_POKEMON(DATOS_NEW_POKEMON_ID* datos) {

	pthread_mutex_lock(&semArbol);
	NodoArbol* nodoPokemon = encontrarPokemon(datos->datos.pokemon);

	if(nodoPokemon == NULL) {
		nodoPokemon = crearPokemon(datos->datos.pokemon);
		agregarNodo(directorioFiles(),nodoPokemon);
		nuevoSemaforo(nodoPokemon->nombre);
	}
	pthread_mutex_unlock(&semArbol);

	pthread_mutex_t* semPokemon = obtenerSemaforo(nodoPokemon->nombre);

	char* path = pathPokemon(nodoPokemon->nombre);

	while (estaAbiertoSem(path, semPokemon)) sleep(configFS.tiempoReintento);

	pthread_mutex_lock(semPokemon);
	t_config* pConfig = CrearConfig(path);
	abrir(pConfig);
	pthread_mutex_unlock(semPokemon);

	int cantBloques = 0;

	t_list* numerosBloques = leerBlocks(&cantBloques, pConfig); //DEVUELVE LA LISTA DE INTS DE LOS NROS DE BLOQUE

	int tamanio = config_get_int_value(pConfig,"SIZE");

	t_list* datosBloques = convertirBloques(numerosBloques,cantBloques,tamanio); //DEVUELVE LA LISTA DE DATOSBLOQUES

	DatosBloques posYCant;

	posYCant.cantidad = datos->datos.cantidad;
	posYCant.pos.posX = datos->datos.posicion.posX;
	posYCant.pos.posY = datos->datos.posicion.posY;

	int bytes = agregarCantidadEnPosicion(datosBloques,posYCant,numerosBloques);

	sleep(configFS.tiempoRetardo);

	pthread_mutex_lock(semPokemon);

	cambiarMetadataPokemon(pConfig,numerosBloques,bytes);

	log_info(logger,"Pokemon: %s, Size: %d, Bloques: %s", nodoPokemon->nombre, bytes, config_get_string_value(pConfig,"BLOCKS"));

	pthread_mutex_unlock(semPokemon);

	Enviar_APPEARED_POKEMON(datos);

	config_destroy(pConfig);
	free(path);

	list_destroy_and_destroy_elements(numerosBloques,&free);
	list_clean(datosBloques);
	list_destroy(datosBloques);

	pthread_exit(0);
}

void Recibir_CATCH_POKEMON(Cliente* cliente, Paquete* paqueteRecibido) {
	Stream* stream = Stream_CrearLecturaPaquete(paqueteRecibido);
	uint32_t id = Deserializar_uint32(stream);
	DATOS_CATCH_POKEMON_ID* datos = malloc(sizeof(DATOS_CATCH_POKEMON_ID));
	datos->datos = Deserializar_CATCH_POKEMON(stream);
	datos->id = id;

	if (stream->error)
		log_error(logger,"Error al deserializar CATCH_POKEMON");
	else {
		log_info(logger,"CATCH_POKEMON recibido");
		EnviarID(cliente,id);
		pthread_t thread;
		pthread_create(&thread, NULL, (void*) Operacion_CATCH_POKEMON,datos);
		pthread_detach(thread);
	}
	free(stream);
}

void Operacion_CATCH_POKEMON(DATOS_CATCH_POKEMON_ID* datos) {

	pthread_mutex_lock(&semArbol);
	NodoArbol* nodoPokemon = encontrarPokemon(datos->datos.pokemon);
	pthread_mutex_unlock(&semArbol);

	if(nodoPokemon == NULL) {
		log_error(logger, "No existe el pokemon");
		Enviar_CAUGHT_POKEMON(datos,false);
	} else {

		pthread_mutex_t* semPokemon = obtenerSemaforo(nodoPokemon->nombre);

		char* path = pathPokemon(nodoPokemon->nombre);

		while (estaAbiertoSem(path, semPokemon)) sleep(configFS.tiempoReintento);

		pthread_mutex_lock(semPokemon);

		t_config* pConfig = CrearConfig(path);

		abrir(pConfig);

		pthread_mutex_unlock(semPokemon);

		int cantBloques =  0;

		t_list* numerosBloques = leerBlocks(&cantBloques,pConfig); //DEVUELVE LA LISTA DE INTS DE LOS NROS DE BLOQUE

		int tamanio = config_get_int_value(pConfig,"SIZE");

		t_list* datosBloques = convertirBloques(numerosBloques,cantBloques,tamanio); //DEVUELVE LA LISTA DE DATOSBLOQUES

		Posicion posicion;

		posicion.posX = datos->datos.posicion.posX;
		posicion.posY = datos->datos.posicion.posY;

		int bytes = 0;

		bool caught = atraparPokemon(datosBloques,posicion,numerosBloques,&bytes,nodoPokemon->nombre);

		sleep(configFS.tiempoRetardo);

		pthread_mutex_lock(semPokemon);
		cambiarMetadataPokemon(pConfig,numerosBloques,bytes);
		log_info(logger,"Pokemon: %s, Size: %d, Bloques: %s", nodoPokemon->nombre, bytes, config_get_string_value(pConfig,"BLOCKS"));
		pthread_mutex_unlock(semPokemon);

		Enviar_CAUGHT_POKEMON(datos,caught);

		config_destroy(pConfig);

		free(path);

		list_destroy_and_destroy_elements(numerosBloques,&free);
		list_clean(datosBloques);
		list_destroy(datosBloques);
	}

	pthread_exit(0);
}

void Recibir_GET_POKEMON(Cliente* cliente, Paquete* paqueteRecibido) {
	Stream* stream = Stream_CrearLecturaPaquete(paqueteRecibido);
	uint32_t id = Deserializar_uint32(stream);
	DATOS_GET_POKEMON_ID* datos = malloc(sizeof(DATOS_GET_POKEMON_ID));
	datos->datos = Deserializar_GET_POKEMON(stream);
	datos->id = id;

	if (stream->error)
		log_error(logger,"Error al deserializar GET_POKEMON");
	else {
		log_info(logger,"GET_POKEMON recibido");
		EnviarID(cliente,id);
		pthread_t thread;
		pthread_create(&thread, NULL, (void*) Operacion_GET_POKEMON,datos);
		pthread_detach(thread);
	}
	free(stream);
}

void Operacion_GET_POKEMON(DATOS_GET_POKEMON_ID* datos) {

	pthread_mutex_lock(&semArbol);
	NodoArbol* nodoPokemon = encontrarPokemon(datos->datos.pokemon);
	pthread_mutex_unlock(&semArbol);

	if(nodoPokemon == NULL) Enviar_LOCALIZED_POKEMON(datos,NULL);
	else {
		char* path = pathPokemon(nodoPokemon->nombre);

		pthread_mutex_t* sem = obtenerSemaforo(nodoPokemon->nombre);

		while (estaAbiertoSem(path,sem)) sleep(configFS.tiempoReintento);

		pthread_mutex_lock(sem);
		t_config* pConfig = CrearConfig(path);
		abrir(pConfig);
		pthread_mutex_unlock(sem);

		int cantBloques =  0;

		t_list* numerosBloques = leerBlocks(&cantBloques,pConfig); //DEVUELVE LA LISTA DE INTS DE LOS NROS DE BLOQUE

		int tamanio = config_get_int_value(pConfig,"SIZE");

		t_list* datosBloques = convertirBloques(numerosBloques,cantBloques,tamanio); //DEVUELVE LA LISTA DE DATOSBLOQUES

		sleep(configFS.tiempoRetardo);

		Enviar_LOCALIZED_POKEMON(datos,datosBloques);

		pthread_mutex_lock(sem);
		cerrar(pConfig);
		pthread_mutex_unlock(sem);

		config_destroy(pConfig);

		free(path);

		list_destroy_and_destroy_elements(numerosBloques,&free);

		for (int i = 0; i < list_size(datosBloques); i++) {
			DatosBloques* pok = list_get(datosBloques,i);
			free(pok);
		}

		list_clean(datosBloques);
		list_destroy(datosBloques);
	}

	pthread_exit(0);
}

void EnviarID(Cliente* cliente, uint32_t identificador)
{
	DATOS_ID_MENSAJE* id_mensaje = malloc(sizeof(DATOS_ID_MENSAJE));

	id_mensaje->id = identificador;
	EnviarMensaje(cliente, BROKER_ACK, id_mensaje, (void*) &SerializarM_ID_MENSAJE);

	free(id_mensaje);
}

void Enviar_APPEARED_POKEMON(DATOS_NEW_POKEMON_ID* datos) {

	DATOS_APPEARED_POKEMON_ID* datosEnviar = malloc(sizeof(DATOS_APPEARED_POKEMON_ID));

	datosEnviar->id = datos->id;
	datosEnviar->datos.pokemon = datos->datos.pokemon;
	datosEnviar->datos.posicion = datos->datos.posicion;

	if(clienteBroker->clienteBroker != NULL) {
		EnviarMensaje(clienteBroker->clienteBroker, APPEARED_POKEMON, datosEnviar, (void*) &SerializarM_APPEARED_POKEMON_ID);
		log_info(logger,"APPEARED_POKEMON enviado.");
	} else {
		log_info(logger, "Sin conexion con el Broker. APPEARED_POKEMON Guardado.");
		list_add(mensajesNoEnviadosAPPEARED,datosEnviar);
	}

//	free(datos->datos.pokemon);
	free(datos);
}

void Enviar_CAUGHT_POKEMON(DATOS_CATCH_POKEMON_ID* datos, bool caught) {

	DATOS_CAUGHT_POKEMON_ID* datosEnviar = malloc(sizeof(DATOS_CAUGHT_POKEMON_ID));

	datosEnviar->idCorrelativa = datos->id;
	datosEnviar->datos.capturado = caught;

	if(clienteBroker->clienteBroker != NULL) {
		EnviarMensaje(clienteBroker->clienteBroker, CAUGHT_POKEMON, datosEnviar, (void*) &SerializarM_CAUGHT_POKEMON_ID);
		log_info(logger,"CAUGHT_POKEMON enviado.");
	} else {
		log_info(logger, "Sin conexion con el Broker. CAUGHT_POKEMON Guardado.");
		list_add(mensajesNoEnviadosCAUGHT,datosEnviar);
	}

//	free(datos->datos.pokemon);
	free(datos);
}

void Enviar_LOCALIZED_POKEMON(DATOS_GET_POKEMON_ID* datos,t_list* datosArchivo) {
	DATOS_LOCALIZED_POKEMON_ID* datosAEnviar = malloc(sizeof(DATOS_LOCALIZED_POKEMON_ID));

	if (datosArchivo == NULL) {
		datosAEnviar->datos.cantidad = 0;
		datosAEnviar->datos.posiciones = NULL;
	} else {
		Posicion* posiciones = malloc(sizeof(Posicion)*list_size(datosArchivo));
		for (int i = 0; i < list_size(datosArchivo); i++) {
			DatosBloques* a = list_get(datosArchivo,i);

			posiciones[i].posX = a->pos.posX;
			posiciones[i].posY = a->pos.posY;
		}
		datosAEnviar->datos.cantidad = list_size(datosArchivo);
		datosAEnviar->datos.posiciones = posiciones;
	}

	datosAEnviar->id = datos->id;
	datosAEnviar->datos.pokemon = datos->datos.pokemon;

	if(clienteBroker->clienteBroker != NULL) {
		EnviarMensaje(clienteBroker->clienteBroker, LOCALIZED_POKEMON, datosAEnviar, (void*) &SerializarM_LOCALIZED_POKEMON_ID);
		log_info(logger,"LOCALIZED_POKEMON enviado.");
	} else {
		log_info(logger, "Sin conexion con el Broker. LOCALIZED_POKEMON guardado.");
		list_add(mensajesNoEnviadosLOCALIZED,datosAEnviar);
	}

//	free(datos->datos.pokemon);
	free(datos);
}

void Recibir_ID(Cliente* cliente, Paquete* paqueteRecibido) {
	Stream* stream = Stream_CrearLecturaPaquete(paqueteRecibido);
	Deserializar_uint32(stream);
}
