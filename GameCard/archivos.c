#include "archivos.h"

t_config* config;

void last(void** lista, void* valorRetorno) {
	int acumulador = 0;
	while (lista[acumulador] != NULL) acumulador++;
	if (acumulador == 0) valorRetorno = (void*) -1;
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

t_list* leerBlocks(char* path, int* cantBloques) {

	t_config* pokemon = config_create(path);

	char** listaBloques = config_get_array_value(pokemon,"BLOCKS");

	config_destroy(config);

	while (listaBloques[*cantBloques] != NULL) (*cantBloques)++;

	t_list* listaARetornar = list_create();

	for (int i = 0; i < *cantBloques; i++) {
		int elem = strtol(listaBloques[i],NULL,10);
		list_add(listaARetornar,&elem);
	}

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

char* pathMetadataBinDe(char* path, char* nombreArchivo) {
	string_append(&path,"/");
	string_append(&path,nombreArchivo);
	string_append(&path,".bin");
	return path;
}

void crearDirectorioFiles(NodoArbol* arbol) {

	char* aux = string_new();
	string_append(&aux,arbol->nombre);
	string_append(&aux,"/Files");

	mkdir(aux, 0700);

	agregarNodo(arbol,crearNodo("Files"));

	string_append(&aux,"/metadata.bin");
	FILE* metadata = fopen(aux,"wb+");
	fputs("DIRECTORY=Y\n",metadata);
	fclose(metadata);
	free(aux);
}

void crearDirectorioBlocks(NodoArbol* padre, int blocks) {

	char* aux = string_new();
	string_append(&aux,padre->nombre);
	string_append(&aux,"/Blocks");

	mkdir(aux, 0700);

	agregarNodo(padre,crearNodo("Blocks"));

	crearBloques(blocks);

	free(aux);
}

void crearBloques(int blocks) {
	for (int i = 0; i < blocks; i++) {
		char* nroBloque = string_itoa(i);
		char* pathBloque = string_new();
		string_append(&pathBloque,"/home/utnso/Escritorio/tall-grass/Blocks"); //TODO
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

	char* aux = string_new();
	string_append(&aux,padre->nombre);
	string_append(&aux,"/Metadata");

	mkdir(aux, 0700);

	agregarNodo(padre,crearNodo("Metadata"));

	char* aux2 = aux;

	crearBitmap(aux2,bitmap,blocks);

	string_append(&aux,"/metadata.bin");
	FILE* metadata = fopen(aux,"wb+");
	fprintf(metadata,"BLOCK_SIZE=%d\n",size);
	fprintf(metadata,"BLOCKS=%d\n",blocks);
	fprintf(metadata,"MAGIC_NUMBER=%s\n",magicNumber);
	fclose(metadata);

	free(aux);
	free(aux2);
}

void crearBitmap(char* path, t_bitarray* bitmap, int cantBlocks) {

//	if (existeArchivo(path)) {
//
//	} else {
		bitmap = bitarray_create_with_mode(string_repeat('\0',cantBlocks), (size_t) cantBlocks, LSB_FIRST);

		string_append(&path,"/bitmap.bin");
		FILE* fBitmap = fopen(path,"wb+");
		fwrite(bitmap->bitarray,bitmap->size,1,fBitmap);
		fclose(fBitmap);
//	}
}

void agregarCantidadEnPosicion(t_list* pokemon, DatosBloques posYCant, t_list* numerosBloques, int size) {

	DatosBloques* posicion = malloc(sizeof(DatosBloques));
	posicion = encontrarPosicion(pokemon,posYCant.pos);

	if(posicion == NULL) {
		list_add(pokemon,&posYCant);

		int* valorRetorno = list_get(numerosBloques,list_size(numerosBloques));
//		last((void*) bloques,&valorRetorno); //TODO VER ACA

		char* path = pathMetadataBinDe("/home/utnso/Escritorio/tall-grass/Blocks",string_itoa(*valorRetorno));

		free(valorRetorno);

		FILE* bloque = fopen(path,"ab+");

//		char* cadena = posicionAString(&posYCant);

		fclose(bloque);
	} else {
		posicion->cantidad += posYCant.cantidad;
		escribirListaEnArchivo(pokemon,size,numerosBloques);
	}
}

char* posicionAString(DatosBloques* d) {
	char* cadena = string_new();

	string_append(&cadena,string_itoa(d->pos.posX));
	string_append(&cadena,"-");
	string_append(&cadena,string_itoa(d->pos.posY));
	string_append(&cadena,"=");
	string_append(&cadena,string_itoa(d->cantidad));
	string_append(&cadena,"\n");
	return cadena;
}

int escribirListaEnArchivo(t_list* pokemon, int size, t_list* numerosBloques) {
	char* cadenaGrande = string_new();

	int cantPokemon = list_size(pokemon);

	for (int i = 0; i < cantPokemon; i++) {
		DatosBloques* pok = list_get(pokemon,i);
		char* cadena = posicionAString(pok);
		string_append(&cadenaGrande,cadena);
	}

	cantPokemon = strlen(cadenaGrande);

	int i = 0;
	while((void*) list_get(numerosBloques,i) != NULL) {
		int* a = list_get(numerosBloques,i);
		FILE* f = fopen(pathMetadataBinDe("/home/utnso/Escritorio/tall-grass/Blocks",string_itoa(*a)),"wb+");
		fwrite(cadenaGrande,size,1,f);
		cadenaGrande = string_substring_from(cadenaGrande, size);
	}

	while(strlen(cadenaGrande) > 0) {
		int nuevoBloque = pedirBloque();
		list_add(numerosBloques,&nuevoBloque);
		FILE* f = fopen(pathMetadataBinDe("/home/utnso/Escritorio/tall-grass/Blocks",string_itoa(nuevoBloque)),"wb+");
		fwrite(cadenaGrande,size,1,f);
		cadenaGrande = string_substring_from(cadenaGrande, size);
	}

	free(cadenaGrande);

	return cantPokemon;
}

DatosBloques* encontrarPosicion(t_list* pokemon, Posicion pos) {

	bool esLaPosicion(Posicion pos2) {
		return pos2.posX == pos.posX && pos2.posY == pos.posY;
	}

	return list_find(pokemon,(void*)&esLaPosicion);
}

NodoArbol* crearPokemon(char* nombre) {

	NodoArbol* nodo = crearNodo(nombre);

	char* pathNodo = string_new();
	string_append(&pathNodo,"/home/utnso/Escritorio/tall-grass/Blocks");
	string_append(&pathNodo,nodo->nombre);

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

	// TODO asignar bloque y escribirlo con lo que tenga que venir


	fclose(metadata);
}

t_list* convertirBloques(t_list* bloques, int cantBloques) {
	t_list* datos = list_create();
	char* datosArchivo = string_new();
	datosArchivo = leerArchivos(bloques,cantBloques,64); //TODO arreglar 64 hardcodeado
	datos = interpretarCadena(datosArchivo,cantBloques,64);
	return datos;
}

char* leerArchivos(t_list* bloques, int cantBloques, int size) {
	char* datos = string_new();

	for(int i = 0; i < cantBloques; i++) {

		int* bloque = list_get(bloques,i);

		char* path = pathMetadataBinDe("/home/utnso/Escritorio/tall-grass/Blocks",string_itoa(*bloque)); //TODO SACAR

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

	free(cant);
	free(posY);
	free(posX);
	return datos;
}
