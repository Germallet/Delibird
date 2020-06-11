#include "gameCard.h"
#include "mensajes.h"
#include <unistd.h>
#include "archivos.h"

t_config* config;

Eventos* eventos;

Cliente* clienteBroker;

t_bitarray* bitmap;

NodoArbol* raiz;

/*
 * TODO BITMAP
 * TODO HACER FUNCIONES PARA LEER, ESCRIBIR, BUSCAR EL ARCHIVO, VERIFICAR SI SE PUEDE ABRIR, OBTENER POSICIONES Y CANTIDADES DE ARCHIVO
 * TODO ELIMINAR CANTIDADES Y POSICIONES DE ARCHIVO
 *
 *
 */

int main()
{
	logger = log_create("gameCard.log", "GameCard", true, LOG_LEVEL_INFO);
	config = config_create("gameCard.config");

	log_info(logger, "Bienvenido a la GameCard!");

	char* miIp = config_get_string_value(config,"IP_GAMECARD");
	int miPuerto = config_get_int_value(config,"PUERTO_GAMECARD");

	char* puntoMontaje = config_get_string_value(config,"PUNTO_MONTAJE_TALLGRASS");
	int tiempoReintentoConexion = config_get_int_value(config,"TIEMPO_DE_REINTENTO_CONEXION");
	int tiempoReintentoOperacion = config_get_int_value(config,"TIEMPO_DE_REINTENTO_OPERACION");
	int tiempoRetardoOperacion = config_get_int_value(config,"TIEMPO_RETARDO_OPERACION");

	CrearHiloTimer(-1,tiempoReintentoConexion,&reconexion,NULL);

	SocketEscucha(miIp, miPuerto);

	EsperarHilos();

	tallGrass_init(puntoMontaje);

	TerminarPrograma(logger,config);
	return 0;
}

void tallGrass_init(char* puntoMontaje) {

	raiz = crearNodo(puntoMontaje);

	if (existeDirectorio(puntoMontaje)) {
//		incorparFileSystem(puntoMontaje);

	} else {
		mkdir(puntoMontaje, 0700);

		crearDirectorioFiles(raiz);

		crearDirectorioBlocks(raiz);

		int size = config_get_int_value(config,"BLOCK_SIZE");
		int blocks = config_get_int_value(config,"BLOCKS");
		char* magicNumber = config_get_string_value(config,"MAGIC_NUMBER");
		crearDirectorioMetadata(raiz,bitmap,size,blocks,magicNumber);
	}
}

bool existePokemon(char* nombre) {

	NodoArbol* dirPokemon = directorioFiles();

	bool esIgual(char* nombre2) {
		return sonIguales(nombre,nombre2);
	}

	return list_any_satisfy(dirPokemon->hijos,(void*)&esIgual);
}

NodoArbol* encontrarPokemon(char* nombre) {

	NodoArbol* nodoFiles = directorioFiles();

	bool esIgual(char* nombre2) {
		return sonIguales(nombre,nombre2);
	}

	return list_find(nodoFiles->hijos,(void*)&esIgual);
}

char* pathDeNodo(NodoArbol* nodo) {
	if (nodo->padre == NULL) return nodo->nombre;
	return strcat(pathDeNodo(nodo),nodo->nombre);
}

NodoArbol* directorioFiles() {
	return (NodoArbol*) list_get(raiz->hijos,0);
}

NodoArbol* directorioBlocks() {
	return (NodoArbol*) list_get(raiz->hijos,1);
}

void conectarse() {

	char* ipBroker = config_get_string_value(config,"IP_BROKER");
	int puertoBroker = config_get_int_value(config,"PUERTO_BROKER");

	eventos = Eventos_Crear0();
	clienteBroker = CrearCliente(ipBroker,puertoBroker,eventos);

	SuscribirseColas(clienteBroker);
}

void reconexion() {
	if (clienteBroker == NULL) conectarse();
}

void EsperarHilos()
{
	pthread_mutex_t mx_main;
	pthread_mutex_init(&mx_main, NULL);
	pthread_mutex_lock(&mx_main);
	pthread_mutex_lock(&mx_main);
}

void TerminarProgramaConError(char* error)
{
	log_error(logger, error);
	TerminarPrograma(logger, config);
	exit(-1);
}

void TerminarPrograma(t_log* logger, t_config* config)
{
	log_destroy(logger);
	config_destroy(config);
}
