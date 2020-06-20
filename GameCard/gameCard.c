#include "gameCard.h"
#include "mensajes.h"
#include <unistd.h>
#include "archivos.h"
#include <signal.h>

Eventos* eventos;

/*
 * TODO MANEJAR MEJOR LOS ERRORES DE FALTA DE MEMORIA
 * TODO SINCRONIZAR BITMAP
 * TODO CATCH DEL CTRL C PARA TERMINAR DE ESCUCHAR
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

	mensajesNoEnviadosAPPEARED = list_create();
	mensajesNoEnviadosCAUGHT = list_create();
	mensajesNoEnviadosLOCALIZED = list_create();

//	pthread_mutex_init(&semBitmap,NULL);

	SocketEscucha(miIp, miPuerto);

	clienteBroker = NULL;

	CrearHiloTimer(-1,tiempoReintentoConexion,&reconexion,clienteBroker);

//	conectarse();

	log_info(logger,"Creando tall-grass");

	tallGrass_init(puntoMontaje);

	log_info(logger,"Mandame mensaje pa, dale");

	signal(SIGINT,EscuchaSignal);

	EsperarHilos();

	TerminarPrograma();

	return 0;
}

//ESTO ES PARA ESCUCHAR HILOS
void EscuchaSignal(int signo) {
    if (signo == SIGINT) pthread_mutex_unlock(&mx_main);
}

void tallGrass_init(char* puntoMontaje) {

	raiz = crearNodo(puntoMontaje);

	if (!existeDirectorio(puntoMontaje)) mkdir(puntoMontaje, 0700);

	crearDirectorioFiles();

	crearDirectorioMetadata();

	crearDirectorioBlocks();
}

NodoArbol* encontrarPokemon(char* nombre) {

	NodoArbol* files = directorioFiles();

	if(list_is_empty(files->hijos)) return NULL;
	for (int i = 0; i < list_size(files->hijos); i++) {
		NodoArbol* pok = list_get(files->hijos,i);
		if (sonIguales(pok->nombre,nombre)) return pok;
	}

	return NULL;
}

char* pathPtoMnt() {
	return raiz->nombre;
}

char* pathPokemon(char* nombre) {
	char* path = string_new();
	string_append(&path,pathPtoMnt());
	string_append(&path,"/Files/");
	string_append(&path,nombre);
	string_append(&path,"/metadata.bin");
	return path;
}

char* pathBloque(char* nombre) {
	char* path = string_new();
	string_append(&path,pathPtoMnt());
	string_append(&path,"/Blocks/");
	string_append(&path,nombre);
	string_append(&path,".bin");
	return path;
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


int* pedirBloque() {

	int* block = malloc(sizeof(int));
	*block = buscarPosicionLibre();

	return block;
}

int buscarPosicionLibre() {

	int i;

	for (i = 0;bitarray_test_bit(bitmap,i) && i < bitmap->size;i++) {}

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

void reconexion(void* info) { //TODO ARREGLAR LO DE LOS MENSAJES
	if (clienteBroker == NULL) {
		conectarse();

		if (clienteBroker != NULL) {

			log_info(logger, "Me conecte al broker");

			for (int i = 0; i < list_size(mensajesNoEnviadosAPPEARED); i++) {
				EnviarMensaje(clienteBroker, APPEARED_POKEMON, list_get(mensajesNoEnviadosAPPEARED,i), (void*) &SerializarM_APPEARED_POKEMON_ID);
			}

			list_clean(mensajesNoEnviadosAPPEARED);

			for (int i = 0; i < list_size(mensajesNoEnviadosCAUGHT); i++) {
				EnviarMensaje(clienteBroker, CAUGHT_POKEMON, list_get(mensajesNoEnviadosCAUGHT,i), (void*) &SerializarM_CAUGHT_POKEMON_ID);
			}

			list_clean(mensajesNoEnviadosCAUGHT);

			for (int i = 0; i < list_size(mensajesNoEnviadosLOCALIZED); i++) {
				EnviarMensaje(clienteBroker, LOCALIZED_POKEMON, list_get(mensajesNoEnviadosLOCALIZED,i), (void*) &SerializarM_LOCALIZED_POKEMON);
			}

			list_clean(mensajesNoEnviadosLOCALIZED);

		} else log_info(logger, "No me pude conectar al broker");

	}
}

void EsperarHilos()
{
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
	log_info(logger,"Terminando programa");
	log_destroy(logger);
	config_destroy(config);
	bitarray_destroy(bitmap);
	if (clienteBroker != NULL) DestruirCliente(clienteBroker);
	DestruirServidor(servidor);
	list_clean(mensajesNoEnviadosAPPEARED);
	list_destroy(mensajesNoEnviadosAPPEARED);
	list_clean(mensajesNoEnviadosCAUGHT);
	list_destroy(mensajesNoEnviadosCAUGHT);
	list_clean(mensajesNoEnviadosLOCALIZED);
	list_destroy(mensajesNoEnviadosLOCALIZED);
	NodoArbol* files = directorioFiles();
	list_clean(files->hijos);
	list_destroy(files->hijos);
	free(files);
	list_clean(raiz->hijos);
	list_destroy(raiz->hijos);
	free(raiz);
}
