#include "archivos.h"
#include <dirent.h>

bool sonIguales(char* a, char* b)
{
	return string_equals_ignore_case(a, b);
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

	log_info(logger,"Leyendo el metadata para ver que bloques usa el pokemon");

	t_config* pokemon = config_create(path);

	char** listaBloques = config_get_array_value(pokemon,"BLOCKS");

	config_destroy(pokemon);

	while (listaBloques[*cantBloques] != NULL) (*cantBloques)++;

	t_list* listaARetornar = list_create();


	for (int i = 0; i < *cantBloques; i++) {
		int* elem = malloc(sizeof(int));
		*elem = strtol(listaBloques[i],NULL,10);
		list_add(listaARetornar,elem);
	}

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

void crearDirectorioFiles() {

	log_info(logger,"Creando directorio Files");

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
		//TODO LEER LOS DIRECTORIOS POKEMON QUE TIENE ADENTRO E INCORPORAR LOS NODOS AL ARBOL. READDIR
	}

	free(aux);
}

void crearDirectorioBlocks(int blocks) {

	log_info(logger,"Creando directorio Blocks");

	char* aux = string_new();
	string_append(&aux,pathPtoMnt());
	string_append(&aux,"/Blocks");

	if(!existeDirectorio(aux)) mkdir(aux, 0700);

	agregarNodo(raiz,crearNodo("/Blocks"));

	crearBloques(blocks);

	free(aux);
}

void crearBloques(int blocks) {

	log_info(logger,"Creando los 4096 blocks");

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

	log_info(logger,"Creando directorio Metadata");

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

	log_info(logger,"Creando bitmap");

	bitmap = bitarray_create_with_mode(string_repeat('\0',cantBlocks), cantBlocks, LSB_FIRST);

	FILE* fBitmap = fopen(path,"wb+");
	fwrite(bitmap->bitarray,bitmap->size,1,fBitmap);
	fclose(fBitmap);
}

bool atraparPokemon(t_list* datosBloques, Posicion pos, t_list* numerosBloques, int size, int* bytes) {
	DatosBloques* posYCant = malloc(sizeof(DatosBloques));
	posYCant = encontrarPosicion(datosBloques,pos);

	bool caught;

	bool esPosicion(DatosBloques* dat) {
		return dat->pos.posX == pos.posX && dat->pos.posY == pos.posY;
	}

	if (posYCant == NULL) {
		log_error(logger, "No existe pokemon en esa posicion");
		caught = false;
	} else {
		posYCant->cantidad-=1;
		if (posYCant->cantidad == 0) list_remove_and_destroy_by_condition(datosBloques,(void*)&esPosicion,&free);
		log_info(logger,"Atrapando pokemon...");
		escribirListaEnArchivo(datosBloques,size,numerosBloques);
		caught = true;
	}

	*bytes = size*(list_size(numerosBloques) - 1) + tamanioBloque((int*) list_get(numerosBloques,list_size(numerosBloques) - 1));

	return caught;
}

int agregarCantidadEnPosicion(t_list* pokemon, DatosBloques posYCant, t_list* numerosBloques, int size) {

	DatosBloques* posicion = malloc(sizeof(DatosBloques));
	posicion = encontrarPosicion(pokemon,posYCant.pos);

	if(posicion == NULL) {

		log_info(logger,"Agregando la posicion");

		list_add(pokemon,&posYCant); //ACA NO SE PORQUE LE SUMA OTRO SI NO HAY NADA

		int* valorRetorno = list_get(numerosBloques,list_size(numerosBloques)-1);

		char* path = pathBloque(string_itoa(*valorRetorno));

		FILE* bloque = fopen(path,"ab+");

		if (bloque != NULL) {
			char* cadena = posicionAString(&posYCant);

			fseek(bloque,0,SEEK_END);
			int tam = ftell(bloque);

			int bytesDesocupados = 64 - tam;

			if (strlen(cadena) <= bytesDesocupados) {

				fwrite(cadena,strlen(cadena),1,bloque);

			} else {
				fwrite(cadena,bytesDesocupados,1,bloque);

				cadena = string_substring_from(cadena,bytesDesocupados);
				int* nuevoBloque = pedirBloque();
				list_add(numerosBloques,nuevoBloque);

				char* nB = string_itoa(*nuevoBloque);
				char* pathB = pathBloque(nB);
				free(nB);
				FILE* f = fopen(pathB,"wb+");
				fwrite(cadena,strlen(cadena),1,f);
				fclose(f);
				free(pathB);
			}

			free(cadena);
			free(path);

			fclose(bloque);
		} else {
			log_error(logger,"No se pudo abrir el archivo del bloque");
		}
	} else {
		log_info(logger,"Sumando la cantidad en la posicion");
		posicion->cantidad += posYCant.cantidad;
		escribirListaEnArchivo(pokemon,size,numerosBloques);
	}

	free(posicion);

	return size*(list_size(numerosBloques) - 1) + tamanioBloque((int*) list_get(numerosBloques,list_size(numerosBloques) - 1));
}

//TODO VER VALGRIND ACA PORQUE HAY BASTANTES COSAS MAL

void escribirListaEnArchivo(t_list* pokemon, int size, t_list* numerosBloques) {

	log_info(logger, "Actualizando los bloques");

	char* cadenaGrande = string_new();

	for (int i = 0; i < list_size(pokemon); i++) {
		DatosBloques* pok = list_get(pokemon,i);
		char* cadena = posicionAString(pok);
		string_append(&cadenaGrande,cadena);
	}

	if (strlen(cadenaGrande) > size*list_size(numerosBloques)) {
		log_info(logger, "Pidiendo bloque");
		int* nuevoBloque = pedirBloque();
		list_add(numerosBloques,nuevoBloque);
	}
	for (int i = 0; i < list_size(numerosBloques); i++) {
		int* a = list_get(numerosBloques,i);
		log_info(logger, "Escribiendo bloque n%d", *a);
		FILE* f = fopen(pathBloque(string_itoa(*a)),"wb+");
		if (strlen(cadenaGrande) < size) size = strlen(cadenaGrande);
		fwrite(cadenaGrande,size,1,f);
		fclose(f);
		if(strlen(cadenaGrande) > size) cadenaGrande = string_substring_from(cadenaGrande, size);
	}
	free(cadenaGrande);
}

DatosBloques* encontrarPosicion(t_list* pokemon, Posicion pos) {

	DatosBloques* dat = malloc(sizeof(DatosBloques));
	for (int i = 0; i < list_size(pokemon); i++) {
		dat = list_get(pokemon,i);
		Posicion pos2 = dat->pos;
		if (pos2.posX == pos.posX && pos2.posY == pos.posY) return dat;
	}
	return NULL;
}

NodoArbol* crearPokemon(char* nombre) {

	log_info(logger,"Creando directorio para el pokemon");

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

	log_info(logger,"Creando metadata para el pokemon");

	string_append(&path,"/metadata.bin");

	FILE* metadata = fopen(path,"wb+");

	fprintf(metadata,"DIRECTORY=N\n");
	fprintf(metadata,"SIZE=0\n");

	int* nuevoBloque = pedirBloque();
	char* bloques = string_new();
	string_append(&bloques,"BLOCKS=[");
	string_append(&bloques,string_itoa(*nuevoBloque));
	string_append(&bloques,"]\n");
	fputs(bloques,metadata);
	fputs("OPEN=N\n",metadata);

	free(bloques);
	free(nuevoBloque);
	fclose(metadata);
}

t_list* convertirBloques(t_list* bloques, int cantBloques) {
	int tamBloque = config_get_int_value(config,"BLOCK_SIZE"); //TODO HAY QUE VER QUE PUEDE NO VENIR DEL CONFIG

	char* datosArchivo = leerArchivos(bloques,cantBloques,tamBloque);

	t_list* datos = interpretarCadena(datosArchivo,cantBloques,tamBloque);

	log_info(logger,"Bloques convertidos");

	free(datosArchivo);
	return datos;
}

char* leerArchivos(t_list* bloques, int cantBloques, int size) {

	log_info(logger,"Leyendo los bloques");

	char* datos = string_new();

	int* bloque = malloc(sizeof(int));

	for(int i = 0; i < cantBloques; i++) {

		bloque = list_get(bloques,i);

		char* pathBlocks = pathBloque(string_itoa(*bloque));

		FILE* f = fopen(pathBlocks,"rb+");

		if(f != NULL && !feof(f)) {
			char* leidos = calloc(1,size);
			fread(leidos,size,1,f);
			string_append(&datos,leidos);
			free(leidos);
		} else log_error(logger,"no se pudo abrir el archivo o algo paso aca ni idea");

		fclose(f);
		free(pathBlocks);
	}
	return datos;
}

t_list* interpretarCadena(char* cadenaDatos, int cantBloques, int size) {

	log_info(logger,"Interpretando la cadena de datos");

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
