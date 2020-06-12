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
 * TODO VERIFICAR SI SE PUEDE ABRIR
 * TODO ELIMINAR CANTIDADES Y POSICIONES DE ARCHIVO
 * TODO SINCRONIZAR TOODO
 * TODO ESCRIBIR EN LOS ARCHIVOS
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

	log_info(logger,"%d",tiempoReintentoOperacion);
	log_info(logger,"%d",tiempoRetardoOperacion);

	tallGrass_init(puntoMontaje);

	CrearHiloTimer(-1,tiempoReintentoConexion,&reconexion,NULL);

	SocketEscucha(miIp, miPuerto);

	EsperarHilos();

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

		int size = config_get_int_value(config,"BLOCK_SIZE");
		int blocks = config_get_int_value(config,"BLOCKS");
		char* magicNumber = config_get_string_value(config,"MAGIC_NUMBER");

		crearDirectorioBlocks(raiz,blocks);
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

//char* pathDeNodo(NodoArbol* nodo) {
//	NodoArbol* padre = nodo->padre;
//	NodoArbol* abuelo = padre->padre;
//
//	char* str = string_new();
//
//	if(abuelo == NULL) {
//		string_append(&str,padre->nombre);
//		string_append(&str,nodo->nombre);
//		return str;
//	}
//
//	if(abuelo->padre == NULL) {
//		string_append(&str,abuelo->nombre);
//		string_append(&str,padre->nombre);
//		string_append(&str,nodo->nombre);
//		return str;
//	}
//	return str;
//}

char* pathDeNodo(NodoArbol* nodo) {
	char* pal = string_new();

	if(nodo->padre == NULL)
		return nodo->nombre;

	pal = nodo->nombre;

	char* pal2 = string_new();
	pal2 = pathDeNodo(nodo->padre);

	string_append(&pal2,pal);

	return pal2;
}

//TODO Recorrer mejor esto

NodoArbol* directorio(char* str) {

	int i = 0;

	NodoArbol* node = (NodoArbol*) list_get(raiz->hijos,i);
	char* nombre = node->nombre;

	while (!sonIguales(str,nombre)){
		i++;
		node = list_get(raiz->hijos,i);
		nombre = node->nombre;
	}

	return list_get(raiz->hijos,i);
}

NodoArbol* directorioFiles() {
	return directorio("/Files");
}

NodoArbol* directorioBlocks() {
	return directorio("/Blocks");
}

NodoArbol* directorioMetadata() {
	return directorio("/Metadata");
}


int pedirBloque() {

	int block = buscarPosicionLibre();

	if(block == -1) return -1;

	return block;
}

int buscarPosicionLibre() {

	int i = 0;

	while (bitarray_test_bit(bitmap,i))
		i++;

	bitarray_set_bit(bitmap,i);

	if (i == bitmap->size)
		return -1;

	FILE* file = fopen(pathMetadataBinDe(pathDeNodo(directorioMetadata()), "/bitmap.bin"),"wb+"); //TODO VER COMO ESCRIBIR UN SOLO BIT
	fwrite(bitmap->bitarray,bitmap->size,1,file);
	fclose(file);

	return i;
}

void conectarse() {

	char* ipBroker = config_get_string_value(config,"IP_BROKER");
	int puertoBroker = config_get_int_value(config,"PUERTO_BROKER");

	eventos = Eventos_Crear0();
	clienteBroker = CrearCliente(ipBroker,puertoBroker,eventos);

	if(clienteBroker != NULL) SuscribirseColas(clienteBroker);
}

void reconexion(void* hello) {
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
