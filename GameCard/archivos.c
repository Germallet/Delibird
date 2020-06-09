#include "archivos.h"
#include <sys/types.h>
#include <sys/stat.h>

t_config* config;

bool sonIguales(char* a, char* b)
{
	return strcmp(a, b) == 0;
}

bool estaAbierto(char* path) {
	config = config_create(path);
	char* open = config_get_string_value(config,"OPEN");
	return sonIguales(open,"Y");
}

bool esDirectorio(char* path) {
	config = config_create(path);
	char* directory = config_get_string_value(config,"DIRECTORY");
	return sonIguales(directory,"Y");
}

void crearDirectorio(char* path, NodoArbol* padre) {

	mkdir(path, 0700);
	agregarHijo(padre,crearNodo(last(string_split(path,"/"))));
	string_append(&path,"/metadata.bin");
	FILE* metadata = fopen(path,"wb+");
	fputs("DIRECTORY=Y\n",metadata);
}

// TODO hacer bien toodo esto que esta mal
void crearFS(char* puntoMontaje, NodoArbol* raiz) {
	mkdir(puntoMontaje, 0700);
	string_append(&puntoMontaje,"/metadata.bin");
	FILE* metadata = fopen(puntoMontaje, "wb+");
	crearDirectorio("/home/utnso/desktop/tall-grass/Files",raiz);
	crearDirectorio("/home/utnso/desktop/tall-grass/Blocks",raiz);
	crearDirectorio("/home/utnso/desktop/tall-grass/Metadata",raiz);
}

void crearArchivo(char* path) {

}

char* last(char** lista) {
	int acumulador = 0;
	while (lista[acumulador] != NULL) acumulador++;
	if (acumulador == 0) return "error"; //TODO manehar ejsotoo bien el erorro
	return lista[acumulador];
}
