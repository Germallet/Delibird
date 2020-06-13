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
	config_destroy(pokemon);
}

void cerrar(char* path) {
	t_config* pokemon = config_create(path);
	config_set_value(pokemon,"OPEN","N");
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
//		DIR* directorio = opendir(aux);
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
		char* nroBloque = string_itoa(i);
		char* pathBloque = string_new();

		string_append(&pathBloque,pathPtoMnt());
		string_append(&pathBloque,"/Blocks");
		string_append(&pathBloque,"/");
		string_append(&pathBloque,nroBloque);
		string_append(&pathBloque,".bin");

		if(!existeArchivo(pathBloque)) {
			FILE* f = fopen(pathBloque,"wb+");
			fclose(f);
		}
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
		int cantBloques = 0;
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
		list_add(pokemon,&posYCant);

		int* valorRetorno = list_get(numerosBloques,list_size(numerosBloques)-1);

		char* pathBlocks = string_new();
		string_append(&pathBlocks,pathPtoMnt());
		string_append(&pathBlocks,"/Blocks");

		char* path = pathMetadataBinDe(pathBlocks,string_itoa(*valorRetorno));

		free(valorRetorno);

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
			FILE* f = fopen(pathMetadataBinDe(pathBlocks,string_itoa(nuevoBloque)),"wb+");
			fwrite(cadena,strlen(cadena),1,f);
			fclose(f);
		}

		fclose(bloque);
	} else {
		posicion->cantidad += posYCant.cantidad;
		escribirListaEnArchivo(pokemon,size,numerosBloques);
	}
	return size*(list_size(numerosBloques) - 1) + tamanioBloque((int) list_get(numerosBloques,list_size(numerosBloques)));
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

void escribirListaEnArchivo(t_list* pokemon, int size, t_list* numerosBloques) {
	char* cadenaGrande = string_new();

	int cantPokemon = list_size(pokemon);

	for (int i = 0; i < cantPokemon; i++) {
		DatosBloques* pok = list_get(pokemon,i);
		char* cadena = posicionAString(pok);
		string_append(&cadenaGrande,cadena);
	}

	cantPokemon = strlen(cadenaGrande);

	char* aux = string_new();
	string_append(&aux,pathPtoMnt());
	string_append(&aux,"/Blocks");

	int i = 0;
	while((void*) list_get(numerosBloques,i) != NULL) {
		int* a = list_get(numerosBloques,i);
		FILE* f = fopen(pathMetadataBinDe(aux,string_itoa(*a)),"wb+");
		fwrite(cadenaGrande,size,1,f);
		cadenaGrande = string_substring_from(cadenaGrande, size);
	}

	while(strlen(cadenaGrande) > 0) {
		int nuevoBloque = pedirBloque();
		list_add(numerosBloques,&nuevoBloque);
		FILE* f = fopen(pathMetadataBinDe(aux,string_itoa(nuevoBloque)),"wb+");
		fwrite(cadenaGrande,size,1,f);
		cadenaGrande = string_substring_from(cadenaGrande, size);
	}

	free(cadenaGrande);
	free(aux);
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

	t_list* datos = list_create();
	char* datosArchivo = string_new();

//	int tamBloque = config_get_int_value(config,"BLOCK_SIZE"); TODO NO SABEMOS PQ NO FUNCIONA
	datosArchivo = leerArchivos(bloques,cantBloques,62);
	datos = interpretarCadena(datosArchivo,cantBloques,62);
	return datos;
}

//TODO HASTA ACA FUNCIONA ESTO

char* leerArchivos(t_list* bloques, int cantBloques, int size) {
	char* datos = string_new();

	for(int i = 0; i < cantBloques; i++) {

		int* bloque = list_get(bloques,i);

		char* pathBlocks = string_new();
		string_append(&pathBlocks,pathPtoMnt()); //TODO VER PORQUE SE BORRA EL NOMBRE DE LA RAIZ
		string_append(&pathBlocks,"/Blocks");

		char* path = pathMetadataBinDe(pathBlocks,string_itoa(*bloque));

		FILE* f = fopen(path,"rb+");

		while(!feof(f)) {
			char* leidos = string_new();
			fgets(leidos,size,f);
			string_append(&datos,leidos);
		}

		fclose(f);
		free(bloque);
		free(path);
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
//
//	free(cant);
//	free(posY);
//	free(posX);
	return datos;
}

int tamanioBloque(int nroBloque) {
	char* pathBloque = string_new();
	string_append(&pathBloque,pathPtoMnt());
	string_append(&pathBloque,"/Blocks/");
	string_append(&pathBloque,string_itoa(nroBloque));
	string_append(&pathBloque,".bin");

	FILE* bloque = fopen(pathBloque,"rb+");
	fseek(bloque,0,SEEK_END);
	int a = ftell(bloque);
	fclose(bloque);

	free(pathBloque);

	return a;
}

void cambiarMetadataPokemon(char* pathPokemon, t_list* numerosBloques, int bytes) {
	t_config* c = config_create(pathPokemon);
	config_set_value(c,"SIZE",string_itoa(bytes));

	char* bloques = string_new();
	string_append(&bloques,"[");
	for(int i = 0; i < list_size(numerosBloques); i++) {
		string_append(&bloques,string_itoa((int)list_get(numerosBloques,i)));
		string_append(&bloques,",");
	}
	bloques[string_length(bloques)-1] = ']';
	config_set_value(c,"BLOCKS",bloques);

	config_set_value(c,"OPEN","N");

	free(bloques);
	config_destroy(c);
}









