#include "gameCard.h"
#include "mensajes.h"
#include <unistd.h>
#include "archivos.h"
#include <signal.h>
#include <dirent.h>

Eventos* eventos;

int main()
{
	logger = log_create("gameCard.log", "GameCard", true, LOG_LEVEL_INFO);
	config = config_create("gameCard.config");

	log_info(logger, "Bienvenido a la GameCard!");

	char* miIp = config_get_string_value(config,"IP_GAMECARD");
	int miPuerto = config_get_int_value(config,"PUERTO_GAMECARD");

	char* puntoMontaje = config_get_string_value(config,"PUNTO_MONTAJE_TALLGRASS");

	configFS.tiempoReintento = config_get_int_value(config,"TIEMPO_DE_REINTENTO_OPERACION");
	configFS.tiempoRetardo = config_get_int_value(config,"TIEMPO_RETARDO_OPERACION");

	mensajesNoEnviadosAPPEARED = list_create();
	mensajesNoEnviadosCAUGHT = list_create();
	mensajesNoEnviadosLOCALIZED = list_create();

	pthread_mutex_init(&semBitmap,NULL);
	pthread_mutex_init(&semArbol,NULL);
	pthread_mutex_init(&semDiccionario,NULL);

	semaforos = dictionary_create();

	SocketEscucha(miIp, miPuerto);

	conectarse();

	log_info(logger,"Creando tall-grass");

	tallGrass_init(puntoMontaje);

	log_info(logger,"Mandame mensaje pa, dale");

	signal(SIGINT,EscuchaSignal);

//	EsperarHilos();

	TerminarPrograma();

	return 0;
}

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

void conectarse() {

	char* ipBroker = config_get_string_value(config,"IP_BROKER");
	int puertoBroker = config_get_int_value(config,"PUERTO_BROKER");
	int tiempoReintentoConexion = config_get_int_value(config,"TIEMPO_DE_REINTENTO_CONEXION");

	eventos = Eventos_Crear0();
	Eventos_AgregarOperacion(eventos, NEW_POKEMON, (EventoOperacion)&Recibir_NEW_POKEMON);
	Eventos_AgregarOperacion(eventos, CATCH_POKEMON, (EventoOperacion)&Recibir_CATCH_POKEMON);
	Eventos_AgregarOperacion(eventos, GET_POKEMON, (EventoOperacion)&Recibir_GET_POKEMON);
	Eventos_AgregarOperacion(eventos, BROKER_ID_MENSAJE, (EventoOperacion)&Recibir_ID);

	clienteBroker = ConectarseABroker(ipBroker,puertoBroker,eventos,&ConexionColas,&EnviarMensajesGuardados,tiempoReintentoConexion);

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

int eliminarPokemon(char* nombre) {

	NodoArbol* files = directorioFiles();

	if(list_is_empty(files->hijos)) return -1;
	for (int i = 0; i < list_size(files->hijos); i++) {
		NodoArbol* pok = list_get(files->hijos,i);
		if (sonIguales(pok->nombre,nombre)) {
			list_remove_and_destroy_element(files->hijos,i,(void* )&DestruirNodo);
			return i;
		}
	}

	return -1;
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

t_list* directorioPokemon() {
	return directorioFiles()->hijos;
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


void EsperarHilos()
{
	pthread_mutex_init(&mx_main, NULL);
	pthread_mutex_lock(&mx_main);
	pthread_mutex_lock(&mx_main);
}

void TerminarPrograma()
{
	log_info(logger,"Terminando programa");
	eliminarPokemonsNoExistentes();
	log_destroy(logger);
	config_destroy(config);
	free(bitmap->bitarray);
	bitarray_destroy(bitmap);
	DestruirServidor(servidor);
	list_destroy_and_destroy_elements(mensajesNoEnviadosAPPEARED,(void*) &BorrarMensajesAppeared);
	list_destroy_and_destroy_elements(mensajesNoEnviadosCAUGHT,(void*) &free);
	list_destroy_and_destroy_elements(mensajesNoEnviadosLOCALIZED,(void*) &BorrarMensajesLocalized);
	freeArbol();
}

void EnviarMensajesGuardados(Cliente* cliente) {
	log_info(logger, "Me conecte al broker");

	for (int i = 0; i < list_size(mensajesNoEnviadosAPPEARED); i++) {
		DATOS_APPEARED_POKEMON_ID* dat = list_get(mensajesNoEnviadosAPPEARED,i);
		EnviarMensaje(cliente, APPEARED_POKEMON, dat, (void*) &SerializarM_APPEARED_POKEMON_ID);
		log_info(logger,"APPEARED_POKEMON enviado");
		free(dat);
	}

	list_clean(mensajesNoEnviadosAPPEARED);

	for (int i = 0; i < list_size(mensajesNoEnviadosCAUGHT); i++)  {
		DATOS_CAUGHT_POKEMON_ID* dat = list_get(mensajesNoEnviadosCAUGHT,i);
		EnviarMensaje(cliente, CAUGHT_POKEMON, dat, (void*) &SerializarM_CAUGHT_POKEMON_ID);
		log_info(logger,"CAUGHT_POKEMON enviado");
		free(dat);
	}

	list_clean(mensajesNoEnviadosCAUGHT);

	for (int i = 0; i < list_size(mensajesNoEnviadosLOCALIZED); i++) {
		DATOS_LOCALIZED_POKEMON_ID* dat = list_get(mensajesNoEnviadosLOCALIZED,i);
		EnviarMensaje(cliente, LOCALIZED_POKEMON, list_get(mensajesNoEnviadosLOCALIZED,i), (void*) &SerializarM_LOCALIZED_POKEMON_ID);
		log_info(logger,"LOCALIZED_POKEMON enviado");
		free(dat);
	}

	list_clean(mensajesNoEnviadosLOCALIZED);
}


void freeArbol() {
	list_clean_and_destroy_elements(directorioPokemon(),(void*) &DestruirNodo);
	list_clean_and_destroy_elements(raiz->hijos,(void*) &DestruirNodo);
	DestruirNodo(raiz);
}

void DestruirNodo(NodoArbol* pok) {
	free(pok->nombre);
	list_destroy(pok->hijos);
	free(pok);
}

void BorrarMensajesLocalized(DATOS_LOCALIZED_POKEMON_ID* dat) {
	free(dat->datos.posiciones);
	free(dat->datos.pokemon);
	free(dat);
}

void BorrarMensajesAppeared(DATOS_APPEARED_POKEMON_ID* dat) {
	free(dat->datos.pokemon);
	free(dat);
}

void eliminarPokemonsNoExistentes() {
	char* pathFiles = string_new();
	string_append(&pathFiles,pathPtoMnt());
	string_append(&pathFiles,directorioFiles()->nombre);
	DIR* dir = opendir(pathFiles);
	struct dirent* entry;

	while((entry = readdir(dir))) {
		if (!sonIguales(entry->d_name,"metadata.bin") && !sonIguales(entry->d_name,".") && !sonIguales(entry->d_name,"..")) {
			char* nombre = string_duplicate(entry->d_name);
			char* path = pathPokemon(nombre);

			t_config* c = CrearConfig(path);

			if (config_get_int_value(c,"SIZE") == 0) eliminarArchivosPokemon(nombre);

			config_destroy(c);
			free(nombre);
			free(path);
		}
	}
	free(pathFiles);

	closedir(dir);
}

int* pedirBloque() {

	int* block = malloc(sizeof(int));
	*block = buscarPosicionLibre();

	return block;
}

void liberarBloque(int bloque) {
	bitarray_clean_bit(bitmap,bloque);

	char* pathMetadata = string_new();
	string_append(&pathMetadata,pathPtoMnt());
	string_append(&pathMetadata,"/Metadata/bitmap.bin");

	FILE* file = fopen(pathMetadata,"wb+");
	fwrite(bitmap->bitarray,bitmap->size,1,file);
	fclose(file);
	free(pathMetadata);
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

	FILE* file = fopen(pathMetadata,"wb+");
	fwrite(bitmap->bitarray,bitmap->size,1,file);
	fclose(file);
	free(pathMetadata);

	return i;
}

void nuevoSemaforo(char* key) {
	pthread_mutex_t* sem = malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(sem, NULL);
	pthread_mutex_lock(&semDiccionario);
	dictionary_put(semaforos, key, sem);
	pthread_mutex_unlock(&semDiccionario);
}

pthread_mutex_t* obtenerSemaforo(char* key) {
	pthread_mutex_lock(&semDiccionario);
	pthread_mutex_t* sem = dictionary_get(semaforos, key);
	pthread_mutex_unlock(&semDiccionario);
	return sem;
}

void destruirDiccionario() {
	dictionary_destroy_and_destroy_elements(semaforos, (void*)&pthread_mutex_destroy);
}



