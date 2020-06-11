#include "archivos.h"

t_config* config;

bool sonIguales(char* a, char* b)
{
	return strcmp(a, b) == 0;
}

bool estaAbierto(char* path) {
	return sonIguales(leerOpen(path),"Y");
}

bool esDirectorio(char* path) {
	return sonIguales(leerDirectorio(path),"Y");
}

int* leerBlocks(char* path, int* cantBloques) {

	t_config* pokemon = config_create(path);

	char** listaBloques = config_get_array_value(pokemon,"BLOCKS");

	while (listaBloques[*cantBloques] != NULL) (*cantBloques)++;

	int listaARetornar[*cantBloques];

	for (int i = 0; i < *cantBloques; i++) listaARetornar[i] = strtol(listaBloques[i],NULL,10);

	config_destroy(config);

	return listaARetornar;
}

char* leerOpen(char* path) {
	t_config* pokemon = config_create(path);

	config_destroy(config);

	return config_get_string_value(pokemon,"OPEN");
}

int leerSize(char* path) {
	t_config* pokemon = config_create(path);

	config_destroy(config);

	return config_get_int_value(pokemon,"SIZE");
}

char* leerDirectory(char* path) {
	t_config* pokemon = config_create(path);

	config_destroy(config);

	return config_get_string_value(pokemon,"DIRECTORY");
}

bool existeDirectorio(char* path) {
	struct stat buf;
	return stat(path, &buf) != -1;
}

void crearDirectorioFiles(NodoArbol* arbol) {

	char* aux = arbol->nombre;
	string_append(&aux,"/Files");

	mkdir(aux, 0700);

	agregarNodo(arbol,crearNodo("/Files"));

	string_append(&aux,"/metadata.bin");
	FILE* metadata = fopen(aux,"wb+");
	fputs("DIRECTORY=Y\n",metadata);
	fclose(metadata);
}

void crearDirectorioBlocks(NodoArbol* padre) {

	char* aux = padre->nombre;
	string_append(&aux,"/Blocks");

	mkdir(aux, 0700);

	agregarNodo(padre,crearNodo("/Blocks"));
}

void crearDirectorioMetadata(NodoArbol* padre, t_bitarray* bitmap, int size, int blocks, char* magicNumber) {

	char* aux = padre->nombre;
	string_append(&aux,"/Metadata");

	mkdir(aux, 0700);

	agregarNodo(padre,crearNodo("/Metadata"));

	crearBitmap(aux,bitmap,4096);

	string_append(&aux,"/metadata.bin");
	FILE* metadata = fopen(aux,"wb+");
	fprintf(metadata,"BLOCK_SIZE=%d\n",size);
	fprintf(metadata,"BLOCKS=%d\n",blocks);
	fprintf(metadata,"MAGIC_NUMBER=%s\n",magicNumber);
	fclose(metadata);

	//COMO REPRESENTAR EL BITMAP EN EL BITMAP.BIN

}

//void crearBitmap(char* path, t_bitarray* bitmap, uint32_t cantBlocks) {
//
//	char* bitNumeritos = calloc(cantBlocks/8,1);
//
//	bitmap = bitarray_create(bitNumeritos, (size_t) cantBlocks);
//
//	string_append(&path,"/bitmap.bin");
//	FILE* fBitmap = fopen(path,"wb+");
//	//ESCRIBIR TODOS EN 0?
//	fclose(fBitmap);
//}
//
//void agregarBloqueOcupado(t_bitarray* bitmap, int nroBlock) {
//	bitarray_set_bit(bitmap,nroBlock);
//
//	FILE* fBM = fopen("/home/utnso/desktop/tall-grass/Metadata/bitmap.bin","ab+");
//	fseek(fBM,nroBlock,0);
//	fputs("1",fBM);
//	fclose(fBM);
//}
//
//void agregarCantidadEnPosicion(DatosBloques* pokemon,Posicion pos, uint32_t cantidad) {
//
//}

FILE* crearBloque(char* nroBloque) {
	char* pathBloque = pathDeNodo(directorioBlocks());
	string_append(pathBloque,"/");
	string_append(pathBloque,nroBloque);
	string_append(pathBloque,".bin");

	return fopen(pathBloque,"wb+");
}

NodoArbol* crearPokemon(char* nombre) {

	NodoArbol* nodo = crearNodo(nombre);

	char* pathNodo = pathDeNodo(nodo);

	crearMetadataPokemon(pathNodo);

	return nodo;
}

void crearMetadataPokemon(char* path) {
	string_append(&path,"/metadata.bin");

	FILE* metadata = fopen(path,"wb+");

	fputs("DIRECTORY=N\n",metadata);
	fputs("SIZE=0\n",metadata);
	fputs("BLOCKS=[]\n",metadata);
	fputs("OPEN=N\n",metadata);

	fclose(metadata);
}

char* last(char** lista) {
	int acumulador = 0;
	while (lista[acumulador] != NULL) acumulador++;
	if (acumulador == 0) return "-1"; //TODO manehar ejsotoo bien el erorro
	return lista[acumulador];
}
