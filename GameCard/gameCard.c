#include "gameCard.h"
#include "mensajes.h"
#include <unistd.h>
#include "archivos.h"

Eventos* eventos;

/*
 * TODO VERIFICAR SI SE PUEDE ABRIR
 * TODO ELIMINAR CANTIDADES Y POSICIONES DE ARCHIVO
 * TODO SINCRONIZAR TOODO
 */

int main()
{
	raiz = malloc(sizeof(NodoArbol));

	logger = log_create("gameCard.log", "GameCard", true, LOG_LEVEL_INFO);
	config = config_create("gameCard.config");

	log_info(logger, "Bienvenido a la GameCard!");

	char* miIp = config_get_string_value(config,"IP_GAMECARD");
	int miPuerto = config_get_int_value(config,"PUERTO_GAMECARD");

	char* puntoMontaje = config_get_string_value(config,"PUNTO_MONTAJE_TALLGRASS");
	int tiempoReintentoConexion = config_get_int_value(config,"TIEMPO_DE_REINTENTO_CONEXION");

	SocketEscucha(miIp, miPuerto);

	CrearHiloTimer(-1,tiempoReintentoConexion,&reconexion,NULL);

	tallGrass_init(puntoMontaje);

	EsperarHilos();

	TerminarPrograma(logger,config);
	return 0;
}

void tallGrass_init(char* puntoMontaje) {

	raiz = crearNodo(puntoMontaje);

	if (!existeDirectorio(puntoMontaje)) mkdir(puntoMontaje, 0700);

	crearDirectorioFiles();
	crearDirectorioBlocks(config_get_int_value(config,"BLOCKS"));
	crearDirectorioMetadata(config_get_int_value(config,"BLOCK_SIZE"),config_get_int_value(config,"BLOCKS"),config_get_string_value(config,"MAGIC_NUMBER"));
}

NodoArbol* encontrarPokemon(char* nombre) {

	NodoArbol* files = directorioFiles();

	if(list_is_empty(files->hijos)) return NULL;
	for (int i = 0; i < list_size(files->hijos); i++) {
		NodoArbol* pok = list_get(files->hijos,i);
		if (sonIguales(pok->nombre,nombre)) return pok;
		free(pok);
	}

	return NULL;
}

char* pathPtoMnt() {
	return "/home/utnso/Escritorio/tall-grass";//config_get_string_value(config,"PUNTO_MONTAJE_TALLGRASS");
}

char* pathPokemon(char* nombre) {
	char* path = string_new();
	string_append(&path,pathPtoMnt());
	string_append(&path,"/Files/");
	string_append(&path,nombre);
	string_append(&path,"/metadata.bin");
	return path;
}

NodoArbol* directorio(char* nombre) {

	if(list_is_empty(raiz->hijos))

	for (int i = 0; i < list_size(raiz->hijos); i++) {
		NodoArbol* hijo = list_get(raiz->hijos,i);
		if (sonIguales(hijo->nombre,nombre)) return hijo;
	}
	return NULL;
}

NodoArbol* directorioFiles() {
	return (NodoArbol*) list_get(raiz->hijos,0);
}

NodoArbol* directorioBlocks() {
	return (NodoArbol*) list_get(raiz->hijos,1);
}

NodoArbol* directorioMetadata() {
	return (NodoArbol*) list_get(raiz->hijos,2);
}


int pedirBloque() {

	int block = buscarPosicionLibre();

	if(block == -1) return -1;

	return block;
}

int buscarPosicionLibre() {

	int i;

	for (i = 0;bitarray_test_bit(bitmap,i) && i < bitmap->size;i++) {}
//	while (bitarray_test_bit(bitmap,i))
//		i++;

	if (i >= bitmap->size)
		return -1;

	bitarray_set_bit(bitmap,i);

	char* pathMetadata = string_new();
	string_append(&pathMetadata,pathPtoMnt());
	string_append(&pathMetadata,"/Metadata/bitmap.bin");

	FILE* file = fopen(pathMetadata,"wb+"); //TODO VER COMO ESCRIBIR UN SOLO BIT
	fwrite(bitmap->bitarray,bitmap->size,1,file);
	fclose(file);
	free(pathMetadata);

	return i;
}

void conectarse() {

	char* ipBroker = config_get_string_value(config,"IP_BROKER");
	int puertoBroker = config_get_int_value(config,"PUERTO_BROKER");

	eventos = Eventos_Crear0();
	clienteBroker = CrearCliente(ipBroker,puertoBroker,eventos);

	if(clienteBroker != NULL) SuscribirseColas(clienteBroker);
}

void reconexion(void* a) {
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

void TerminarPrograma()
{
	log_destroy(logger);
	config_destroy(config);
	free(raiz);
}
