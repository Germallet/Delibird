#include "archivos.h"

t_config* config;

void last(void** lista, void* valorRetorno) {
	int acumulador = 0;
	while (lista[acumulador] != NULL) acumulador++;
	if (acumulador == 0) valorRetorno = (void*) -1; //TODO manehar ejsotoo bien el erorro
	valorRetorno = (void*) lista[acumulador];
}

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

	config_destroy(config);

	while (listaBloques[*cantBloques] != NULL) (*cantBloques)++;

	int* listaARetornar = malloc(*cantBloques);

	for (int i = 0; i < *cantBloques; i++) listaARetornar[i] = strtol(listaBloques[i],NULL,10);

	return listaARetornar;
}

char* leerOpen(char* path) {
	t_config* pokemon = config_create(path);

	char* open = config_get_string_value(pokemon,"OPEN");

	config_destroy(config);

	return open;
}

int leerSize(char* path) {
	t_config* pokemon = config_create(path);

	int size = config_get_int_value(pokemon,"SIZE");

	config_destroy(config);

	return size;
}

char* leerDirectorio(char* path) {
	t_config* pokemon = config_create(path);

	char* directory = config_get_string_value(pokemon,"DIRECTORY");

	config_destroy(config);

	return directory;
}

bool existeDirectorio(char* path) {
	struct stat buf;
	return stat(path, &buf) != -1;
}

short existeArchivo(char *path) {
  int fd = open(path, O_RDONLY);
  if (fd < 0) return 0;
  close(fd);
  return 1;
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

void crearDirectorioBlocks(NodoArbol* padre, int blocks) {

	char* aux = padre->nombre;
	string_append(&aux,"/Blocks");

	mkdir(aux, 0700);

	agregarNodo(padre,crearNodo("/Blocks"));

	crearBloques(blocks);
}

void crearBloques(int blocks) {
	for (int i = 0; i < blocks; i++) {
		char* nroBloque = string_itoa(i);
		char* pathBloque = pathDeNodo(directorioBlocks());
		string_append(&pathBloque,"/");
		string_append(&pathBloque,nroBloque);
		string_append(&pathBloque,".bin");
		if(!existeArchivo(pathBloque)) {
			FILE* f = fopen(pathBloque,"wb+");
			fclose(f);
		}
	}
}

void crearDirectorioMetadata(NodoArbol* padre, t_bitarray* bitmap, int size, int blocks, char* magicNumber) {

	char* aux = padre->nombre;
	string_append(&aux,"/Metadata");

	mkdir(aux, 0700);

	agregarNodo(padre,crearNodo("/Metadata"));

	crearBitmap(aux,bitmap,blocks);

	string_append(&aux,"/metadata.bin");
	FILE* metadata = fopen(aux,"wb+");
	fprintf(metadata,"BLOCK_SIZE=%d\n",size);
	fprintf(metadata,"BLOCKS=%d\n",blocks);
	fprintf(metadata,"MAGIC_NUMBER=%s\n",magicNumber);
	fclose(metadata);
}

void crearBitmap(char* path, t_bitarray* bitmap, int cantBlocks) {

	if (existeArchivo(path)) {

	} else {
		bitmap = bitarray_create_with_mode(string_repeat('\0',cantBlocks), (size_t) cantBlocks, LSB_FIRST);

		string_append(&path,"/bitmap.bin");
		FILE* fBitmap = fopen(path,"wb+");
		fwrite(bitmap->bitarray,cantBlocks,1,fBitmap);
		fclose(fBitmap);
	}
}

//void agregarBloqueOcupado(t_bitarray* bitmap, int nroBlock) {
//	bitarray_set_bit(bitmap,nroBlock);
//
//	FILE* fBM = fopen("/home/utnso/desktop/tall-grass/Metadata/bitmap.bin","ab+");
//	fseek(fBM,nroBlock,0);
//	fputs("1",fBM);
//	fclose(fBM);
//}

void agregarCantidadEnPosicion(t_list* pokemon, DatosBloques posYCant, int* bloques) {

	DatosBloques* posicion = malloc(sizeof(DatosBloques));
	posicion = encontrarPosicion(pokemon,posYCant.pos);

	if(posicion == NULL) {
		list_add(pokemon,&posYCant);

		int valorRetorno = 0;
		last((void*) bloques,&valorRetorno);

		char* path = pathDeNodo(directorioBlocks());

		string_append(&path,"/");
		string_append(&path,string_itoa(valorRetorno));
		string_append(&path,".bin");

		FILE* bloque = fopen(path,"ab+");

			//todo

		fclose(bloque);
	} else {
		posicion->cantidad += posYCant.cantidad;
		escribirListaEnArchivo(pokemon);
	}
}

void escribirListaEnArchivo(t_list* pokemon) {
//	todo
}

DatosBloques* encontrarPosicion(t_list* pokemon, Posicion pos) {

	bool esLaPosicion(Posicion pos2) {
		return pos2.posX == pos.posX && pos2.posY == pos.posY;
	}

	return list_find(pokemon,(void*)&esLaPosicion);
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

t_list* convertirBloques(int* bloques, int cantBloques) {
	t_list* datos = list_create();
	char* datosArchivo = string_new();
	datosArchivo = leerArchivos(bloques,cantBloques,64); //TODO arreglar 64 hardcodeado
	datos = interpretarCadena(datosArchivo,cantBloques,64);
	return datos;
}

char* leerArchivos(int* bloques, int cantBloques, int size) {
	char* datos = string_new();

	for(int i = 0; i < cantBloques; i++) {

		char* path = pathDeNodo(directorioBlocks());

		string_append(&path,"/");
		string_append(&path,string_itoa(bloques[i]));
		string_append(&path,".bin");

		FILE* f = fopen(path,"rb+");

		while(!feof(f)) {
			char* leidos = string_new();
			fgets(leidos,size,f);
			string_append(&datos,leidos);
		}

		fclose(f);
	}

	return datos;
}

t_list* interpretarCadena(char* cadenaDatos, int cantBloques, int size) {

	t_list* datos = list_create();

	char* posX;
	char* posY;
	char* cant;

	int i = 0;

	while(cadenaDatos[i] != '\0') {
		DatosBloques* datosLeidos = malloc(sizeof(DatosBloques));

		posX = string_new();

		while(cadenaDatos[i] != '\0' && cadenaDatos[i] != '-') {
			string_append(&posX,&cadenaDatos[i]);
			i++;
		}
		datosLeidos->pos.posX = strtol(posX,NULL,10);

		if (cadenaDatos[i] != '\0') i++;

		posY = string_new();

		while(cadenaDatos[i] != '\0' && cadenaDatos[i] != '=') {
			string_append(&posY,&cadenaDatos[i]);
			i++;
		}
		datosLeidos->pos.posY = strtol(posY,NULL,10);

		if (cadenaDatos[i] != '\0') i++;

		cant = string_new();

		while(cadenaDatos[i] != '\0' && cadenaDatos[i] != '\n') {
			string_append(&cant,&cadenaDatos[i]);
			i++;
		}
		datosLeidos->cantidad = strtol(cant,NULL,10);
		list_add(datos,datosLeidos);
	}

	return datos;
}
