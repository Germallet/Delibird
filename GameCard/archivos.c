#include "archivos.h"


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

void crearDirectorioFiles(NodoArbol* arbol) {

	char* aux = arbol->nombre;
	string_append(&aux,"/Files");

	mkdir(aux, 0700);

	agregarHijo(arbol,crearNodo("/Files"));

	string_append(&aux,"/metadata.bin");
	FILE* metadata = fopen(aux,"wb+");
	fputs("DIRECTORY=Y\n",metadata);
	fclose(metadata);
}

void crearDirectorioBlocks(NodoArbol* padre) {

	char* aux = padre->nombre;
	string_append(&aux,"/Blocks");

	mkdir(aux, 0700);

	agregarHijo(padre,crearNodo("/Blocks"));
}

void crearDirectorioMetadata(NodoArbol* padre, t_bitarray* bitmap) {

	char* aux = padre->nombre;
	string_append(&aux,"/Metadata");

	mkdir(aux, 0700);

	agregarHijo(padre,crearNodo("/Metadata"));

	crearBitmap(aux,bitmap,5192);

	string_append(&aux,"/metadata.bin");
	FILE* metadata = fopen(aux,"wb+");
	fputs("BLOCK_SIZE=64\n",metadata);
	fputs("BLOCKS=5192\n",metadata);
	fputs("MAGIC_NUMBER=TALLGRASS\n",metadata);
	fclose(metadata);

	 //COMO REPRESENTAR EL BITMAP EN EL BITMAP.BIN

}

void crearBitmap(char* path, t_bitarray* bitmap, uint32_t cantBlocks) {

	char* bitNumeritos = calloc(cantBlocks/8,1);

	bitmap = bitarray_create(bitNumeritos, (size_t) cantBlocks);

	string_append(&path,"/bitmap.bin");
	FILE* fBitmap = fopen(path,"wb+");
	//ESCRIBIR TODOS EN 0?
	fclose(fBitmap);
}

void agregarBloqueOcupado(t_bitarray* bitmap, int nroBlock) {
	bitarray_set_bit(bitmap,nroBlock);

//	FILE* fBM = fopen("/home/utnso/desktop/tall-grass/Metadata/bitmap.bin","ab+");
//	fseek(fBM,nroBlock,0);
//	fputs("1",fBM);
//	fclose(fBM);
}

bool existeDirectorio(char* path) {
	struct stat buf;
	return stat(path, &buf) != -1;
}

char* crearPokemon(char* nombre) {

	crearNodo(nombre);

	char* pathF = pathFiles();

	string_append(&pathF, nombre);

	crearMetadataPokemon(pathF);

	return pathF;
}

void crearMetadataPokemon(char* path,int size) {
	string_append(&path,"/metadata.bin");

	FILE* metadata = fopen(path,"wb+");

	fputs("DIRECTORY=N\n",metadata);
	fputs("SIZE=256\n",metadata);
	fputs("BLOCKS=[]\n",metadata);
	fputs("OPEN=N\n",metadata);

	fclose(metadata);
}

char* encontrarPokemon(char* nombre) {
	char* path = pathFiles();

	bool esIgual(char* nombre2) {
		return sonIguales(nombre,nombre2);
	}

//	NodoArbol* pokemon = list_find(dirPokemon->hijos,(void*)&esIgual);

	string_append(&path,"/");
	string_append(&path,nombre);
	string_append(&path,"/metadata.bin");

	return path;
}

//void bloquesPokemon(FILE* pokemon) {
//	config = config_create(pokemon);
//	char* directory = config_get_string_value(config,"DIRECTORY");
//
//}

//char leerDirectory(FILE* file) {
//	char* directory;
//	fgetc(directory,10,file);
//
//	return directory[10];
//}
//
//uint32_t leerSize(FILE* file) {
//	char* size;
//
//	fseek(file,11,0);
//
//	fgets(size,);
//}

char* leerBlocks(FILE* file) {

	char* blocks;
	char a = fgetc(file);
	int acum = 0;
	int acum2 = 0;

	while(a != '['){
		a = fgetc(file);
		acum++;
	}

	while(a != ']'){
		a = fgetc(file);
		acum2++;
	}

	fseek(file,acum,0);

	char* bloques = malloc(acum2-acum);

	bloques = fgets(blocks,acum2-acum,file);

	string_split(fgets(blocks,acum2-acum,file),",");

	return bloques;
}

char* leerBlocksPorConfig(char* path) {

	t_config* pokemon = config_create(path);

	return config_get_string_value(pokemon,"BLOCKS");
}

void agregarCantidadEnPosicion(DatosBloques* pokemon,Posicion pos, uint32_t cantidad) {

}

void crearBloque(char* path) {

}


char* last(char** lista) {
	int acumulador = 0;
	while (lista[acumulador] != NULL) acumulador++;
	if (acumulador == 0) return "error"; //TODO manehar ejsotoo bien el erorro
	return lista[acumulador];
}
