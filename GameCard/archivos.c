#include "archivos.h"
#include <dirent.h>

bool sonIguales(char* a, char* b)
{
	return strcmp(a, b) == 0;
}

bool estaAbierto(char* path) {
	return sonIguales(leerOpen(path),"Y");
}

void abrir(char* path) {
	t_config* pokemon = config_create(path);
	config_set_value(pokemon,"OPEN","Y");
	config_save(pokemon);
	config_destroy(pokemon);
}

void cerrar(char* path) {
	t_config* pokemon = config_create(path);
	config_set_value(pokemon,"OPEN","N");
	config_save(pokemon);
	config_destroy(pokemon);
}

bool esDirectorio(char* path) {
	return sonIguales(leerDirectorio(path),"Y");
}

t_list* leerBlocks(char* path, int* cantBloques) {

	t_config* pokemon = config_create(path);

	char** listaBloques = config_get_array_value(pokemon,"BLOCKS");

	config_destroy(pokemon);

	while (listaBloques[*cantBloques] != NULL) (*cantBloques)++;

	t_list* listaARetornar = list_create();

	int* elem = malloc(sizeof(int));
	for (int i = 0; i < *cantBloques; i++) {
		*elem = strtol(listaBloques[i],NULL,10);
		list_add(listaARetornar,elem);
	}
//	free(elem);

	return listaARetornar;
}

char* leerOpen(char* path) {
	t_config* pokemon = config_create(path);

	char* open = config_get_string_value(pokemon,"OPEN");

	config_destroy(pokemon);

	return open;
}

int leerSize(char* path) {
	t_config* pokemon = config_create(path);

	int size = config_get_int_value(pokemon,"SIZE");

	config_destroy(pokemon);

	return size;
}

char* leerDirectorio(char* path) {
	t_config* pokemon = config_create(path);

	char* directory = config_get_string_value(pokemon,"DIRECTORY");

	config_destroy(pokemon);

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

//char* pathMetadataBinDe(char* path, char* nombreArchivo) {
//	string_append(&path,"/");
//	string_append(&path,nombreArchivo);
//	string_append(&path,".bin");
//	return path;
//}

void crearDirectorioFiles() {

	char* aux = string_new();
	string_append(&aux,pathPtoMnt());
	string_append(&aux,"/Files");
	agregarNodo(raiz,crearNodo("/Files"));

	if (!existeDirectorio(aux)) {
		mkdir(aux, 0700);

		string_append(&aux,"/metadata.bin");
		FILE* metadata = fopen(aux,"wb+");
		fputs("DIRECTORY=Y\n",metadata);
		fclose(metadata);
	} else {
		//TODO LEER LOS DIRECTORIOS POKEMON QUE TIENE ADENTRO E INCORPORAR LOS NODOS AL ARBOL
	}

	free(aux);
}

void crearDirectorioBlocks(int blocks) {

	char* aux = string_new();
	string_append(&aux,pathPtoMnt());
	string_append(&aux,"/Blocks");

	if(!existeDirectorio(aux)) mkdir(aux, 0700);

	agregarNodo(raiz,crearNodo("/Blocks"));

	crearBloques(blocks);

	free(aux);
}

void crearBloques(int blocks) {

	for (int i = 0; i < blocks; i++) {
		char* b = string_itoa(i);

		char* bloque = pathBloque(b);

		if(!existeArchivo(bloque)) {
			FILE* f = fopen(bloque,"wb+");
			fclose(f);
		}

		free(b);
		free(bloque);
	}
}

void crearDirectorioMetadata() {

	char* aux = string_new();
	string_append(&aux,raiz->nombre);
	string_append(&aux,"/Metadata");

	if(!existeDirectorio(aux)) {
		mkdir(aux, 0700);
	}

	agregarNodo(raiz,crearNodo("/Metadata"));

	char* aux2 = string_new();
	string_append(&aux2,aux);
	string_append(&aux2,"/bitmap.bin");

	string_append(&aux,"/metadata.bin");

	if(!existeArchivo(aux)) {
		FILE* metadata = fopen(aux,"wb+");
		fprintf(metadata,"BLOCK_SIZE=%d\n",config_get_int_value(config,"BLOCK_SIZE"));
		fprintf(metadata,"BLOCKS=%d\n",config_get_int_value(config,"BLOCKS"));
		fprintf(metadata,"MAGIC_NUMBER=%s\n",config_get_string_value(config,"BLOCK_SIZE"));
		fclose(metadata);
	}

	if(!existeArchivo(aux2)) {
		crearBitmap(aux2,config_get_int_value(config,"BLOCKS"));
	} else {
		bitmap = bitarray_create_with_mode(string_repeat('\0',config_get_int_value(config,"BLOCKS")), config_get_int_value(config,"BLOCKS"), LSB_FIRST);
		//TODO ESTO LO TENDRIA QUE LEER ARCHIVO
	}

	free(aux2);
	free(aux);
}

void crearBitmap(char* path, int cantBlocks) {

	bitmap = bitarray_create_with_mode(string_repeat('\0',cantBlocks), cantBlocks, LSB_FIRST);

	FILE* fBitmap = fopen(path,"wb+");
	fwrite(bitmap->bitarray,bitmap->size,1,fBitmap);
	fclose(fBitmap);
}

int agregarCantidadEnPosicion(t_list* pokemon, DatosBloques posYCant, t_list* numerosBloques, int size) {

	DatosBloques* posicion = malloc(sizeof(DatosBloques));
	posicion = encontrarPosicion(pokemon,posYCant.pos);

	if(posicion == NULL) {
		list_add(pokemon,&posYCant); //ACA NO SE PORQUE LE SUMA OTRO SI NO HAY NADA

		int* valorRetorno = list_get(numerosBloques,list_size(numerosBloques)-1);

		char* path = pathBloque(string_itoa(*valorRetorno));

		FILE* bloque = fopen(path,"ab+");

		char* cadena = posicionAString(&posYCant);

		fseek(bloque,0,SEEK_END);
		int tam = ftell(bloque);

		int bytesDesocupados = 64 - tam;

		if (strlen(cadena) <= bytesDesocupados) {

			fwrite(cadena,strlen(cadena),1,bloque);

		} else {
			fwrite(cadena,bytesDesocupados,1,bloque);

			cadena = string_substring_from(cadena,bytesDesocupados);
			int nuevoBloque = pedirBloque();
			list_add(numerosBloques,&nuevoBloque);

			FILE* f = fopen(pathBloque(string_itoa(nuevoBloque)),"wb+");
			fwrite(cadena,strlen(cadena),1,f);
			fclose(f);
		}

		free(cadena);
		free(path);

		fclose(bloque);

	} else {
		posicion->cantidad += posYCant.cantidad;
		escribirListaEnArchivo(pokemon,size,numerosBloques);
	}

	free(posicion);

	return size*(list_size(numerosBloques) - 1) + tamanioBloque((int*) list_get(numerosBloques,list_size(numerosBloques) - 1));
}

//int bytesUtilizados(t_list* numerosBloques) {
//	int a = 0;
//	for(int i = 0; i < list_size(numerosBloques); i++) {
//		a += tamArchivo(list_get(numerosBloques,i));
//	}
//
//	return a;
//}

void escribirListaEnArchivo(t_list* pokemon, int size, t_list* numerosBloques) {
	char* cadenaGrande = string_new();

	int cantPokemon = list_size(pokemon);

	for (int i = 0; i < cantPokemon; i++) {
		DatosBloques* pok = list_get(pokemon,i);
		char* cadena = posicionAString(pok);
		string_append(&cadenaGrande,cadena);
	}

	cantPokemon = strlen(cadenaGrande);

	int i = 0;

	while(list_get(numerosBloques,i) != NULL) {
		int* a = list_get(numerosBloques,i);
		FILE* f = fopen(pathBloque(string_itoa(*a)),"wb+");
		fwrite(cadenaGrande,size,1,f);
		fclose(f);
		if(strlen(cadenaGrande) < size) {
			free(cadenaGrande);
			break;
		}
		cadenaGrande = string_substring_from(cadenaGrande, size);
	}

	while(strlen(cadenaGrande) > 0) {
		int nuevoBloque = pedirBloque();
		list_add(numerosBloques,&nuevoBloque);
		FILE* f = fopen(pathBloque(string_itoa(nuevoBloque)),"wb+");
		fwrite(cadenaGrande,size,1,f);
		fclose(f);
		if(strlen(cadenaGrande) < size) {

			break;
		}
		cadenaGrande = string_substring_from(cadenaGrande, size);
	}

//	free(cadenaGrande);
//	free(aux);
}

DatosBloques* encontrarPosicion(t_list* pokemon, Posicion pos) {

	for (int i = 0; i < list_size(pokemon); i++) {
		DatosBloques* dat = (DatosBloques*) list_get(pokemon,i);
		Posicion pos2 = dat->pos;
		if (pos2.posX == pos.posX && pos2.posY == pos.posY) {
			return dat;
		}
		free(dat);
	}
	return NULL;
}

NodoArbol* crearPokemon(char* nombre) {

	NodoArbol* nodo = crearNodo(nombre);

	char* pathNodo = string_new();
	string_append(&pathNodo,pathPtoMnt());
	string_append(&pathNodo,"/Files/");
	string_append(&pathNodo,nodo->nombre);

	mkdir(pathNodo,0700);
	crearMetadataPokemon(pathNodo);

	return nodo;
}

void crearMetadataPokemon(char* path) {

	string_append(&path,"/metadata.bin");

	FILE* metadata = fopen(path,"wb+");

	fprintf(metadata,"DIRECTORY=N\n");
	fprintf(metadata,"SIZE=0\n");

	int nuevoBloque = pedirBloque();
	char* bloques = string_new();
	string_append(&bloques,"BLOCKS=[");
	string_append(&bloques,string_itoa(nuevoBloque));
	string_append(&bloques,"]\n");
	fputs(bloques,metadata);
	fputs("OPEN=N\n",metadata);

	free(bloques);
	fclose(metadata);
}

t_list* convertirBloques(t_list* bloques, int cantBloques) {
	int tamBloque = config_get_int_value(config,"BLOCK_SIZE"); //TODO HAY QUE VER QUE PUEDE NO VENIR DEL CONFIG

	char* datosArchivo = string_new();
	datosArchivo = leerArchivos(bloques,cantBloques,tamBloque);

	t_list* datos = interpretarCadena(datosArchivo,cantBloques,tamBloque);

	free(datosArchivo);
	return datos;
}

char* leerArchivos(t_list* bloques, int cantBloques, int size) {
	char* datos = string_new();

	int* bloque = malloc(sizeof(int));

	for(int i = 0; i < cantBloques; i++) {

		bloque = list_get(bloques,i);

		char* pathBlocks = pathBloque(string_itoa(*bloque));

		FILE* f = fopen(pathBlocks,"rb+");

		if(!feof(f)) {
			char* leidos = malloc(size);
			fread(leidos,size,1,f);
			string_append(&datos,leidos);
			free(leidos);
		}

		fclose(f);
		free(pathBlocks);
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

		free(posX);

		if (cadenaDatos[i] != '\0') i++;

		posY = string_new();

		while(cadenaDatos[i] != '\0' && cadenaDatos[i] != '=') {
			string_append(&posY,&cadenaDatos[i]);
			i++;
		}
		datosLeidos->pos.posY = strtol(posY,NULL,10);

		free(posY);

		if (cadenaDatos[i] != '\0') i++;

		cant = string_new();

		while(cadenaDatos[i] != '\0' && cadenaDatos[i] != '\n') {
			string_append(&cant,&cadenaDatos[i]);
			i++;
		}
		datosLeidos->cantidad = strtol(cant,NULL,10);

		free(cant);

		if (cadenaDatos[i] != '\0') i++;

		list_add(datos,datosLeidos);
	}
	return datos;
}

int tamanioBloque(int* nroBloque) {
	char* path = pathBloque(string_itoa(*nroBloque));

	FILE* bloque = fopen(path,"rb");

	fseek(bloque,0,SEEK_END);
	int a = ftell(bloque);

	fclose(bloque);

	free(path);

	return a;
}

void cambiarMetadataPokemon(char* pathPokemon, t_list* numerosBloques, int bytes) {
	t_config* c = config_create(pathPokemon);
	config_set_value(c,"SIZE",string_itoa(bytes));

	char* bloques = string_new();
	string_append(&bloques,"[");
	for(int i = 0; i < list_size(numerosBloques); i++) {
		int* a = list_get(numerosBloques,i);
		string_append(&bloques,string_itoa(*a));
		string_append(&bloques,",");
	}
	bloques[string_length(bloques)-1] = ']';
	config_set_value(c,"BLOCKS",bloques);

	config_set_value(c,"OPEN","N");

	free(bloques);
	config_save(c);
	config_destroy(c);
}

char* posicionAString(DatosBloques* d) {
	char* cadena = string_new();

	char* posx = string_itoa(d->pos.posX);
	char* posy = string_itoa(d->pos.posY);
	char* cant = string_itoa(d->cantidad);

	string_append(&cadena,posx);
	string_append(&cadena,"-");
	string_append(&cadena,posy);
	string_append(&cadena,"=");
	string_append(&cadena,cant);
	string_append(&cadena,"\n");

	free(posx);
	free(posy);
	free(cant);
	return cadena;
}
