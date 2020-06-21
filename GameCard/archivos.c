#include "archivos.h"
#include <dirent.h>

bool sonIguales(char* a, char* b) {
	return string_equals_ignore_case(a, b);
}

bool estaAbierto(t_config* pConfig) {
	return sonIguales(config_get_string_value(pConfig,"OPEN"),"Y");
}

bool estaAbiertoPath(char* path) {
	pthread_mutex_lock(&semDeMierda);
	t_config* c = config_create(path);
	bool open = sonIguales(config_get_string_value(c,"OPEN"),"Y");
	config_destroy(c);
	return open;
}

void abrir(t_config* conf) {
	config_set_value(conf,"OPEN","Y");
	config_save(conf);
}

void cerrar(t_config* conf) {
	config_set_value(conf,"OPEN","N");
	config_save(conf);
}

t_list* leerBlocks(int* cantBloques, t_config* conf) {

	log_info(logger,"Leyendo el metadata para ver que bloques usa el pokemon");

	char** listaBloques = config_get_array_value(conf,"BLOCKS");

	while (listaBloques[*cantBloques] != NULL) (*cantBloques)++;

	t_list* listaARetornar = list_create();

	for (int i = 0; i < *cantBloques; i++) {
		int* elem = malloc(sizeof(int));
		*elem = strtol(listaBloques[i],NULL,10);
		list_add(listaARetornar,elem);
	}

	for (int i = 0; i < list_size(listaARetornar); i++) {
		free(listaBloques[i]);
	}

	return listaARetornar;
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
		log_info(logger,"Levantando directorio files");
		DIR* files = opendir(aux);
		struct dirent* entry;

		if (files == NULL) {
			log_error(logger,"No se pudo abrir correctamente el directorio files"); //TODO TERMINAR PROGRAMA ??
		} else {
			while((entry = readdir(files))) {
				if (!sonIguales(entry->d_name,"metadata.bin") && !sonIguales(entry->d_name,".") && !sonIguales(entry->d_name,"..")) {
					log_info(logger,"Leyendo archivo %s",entry->d_name);
//					char* nombre = string_new();
//					strcpy(nombre,entry->d_name);
					agregarNodo(directorioFiles(),crearNodo(entry->d_name));
				}

			}
		}
//		closedir(files); TODO VER ESTO
	}
	free(aux);
}

void crearDirectorioBlocks() {

	log_info(logger,"Creando directorio Blocks");

	char* aux = string_new();
	string_append(&aux,pathPtoMnt());
	string_append(&aux,"/Blocks");

	if(!existeDirectorio(aux)) mkdir(aux, 0700);

	agregarNodo(raiz,crearNodo("/Blocks"));

	crearBloques();

	free(aux);
}

void crearBloques() {

	log_info(logger,"Creando los %d blocks", configFS.cantidadBlocks);

	for (int i = 0; i < configFS.cantidadBlocks; i++) {
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
		fprintf(metadata,"MAGIC_NUMBER=%s\n",config_get_string_value(config,"MAGIC_NUMBER"));
		fclose(metadata);
	}

	t_config* conf = config_create(aux);

	configFS.cantidadBlocks = config_get_int_value(conf,"BLOCKS");
	configFS.tamanioBlocks = config_get_int_value(conf,"BLOCK_SIZE");
	configFS.magicNumber = config_get_string_value(conf,"MAGIC_NUMBER");

	config_destroy(conf);

	if (!existeArchivo(aux2)) crearBitmap(aux2);
	else {
		log_info(logger,"Levantando bitmap");
		FILE* bm = fopen(aux2,"rb+");
		char* bitarray = calloc(1,configFS.cantidadBlocks);
		fread(bitarray,configFS.cantidadBlocks,1,bm);
		bitmap = bitarray_create_with_mode(bitarray, configFS.cantidadBlocks, LSB_FIRST);
	}

	free(aux2);
	free(aux);
}

void crearBitmap(char* path) {

	log_info(logger,"Creando bitmap");
	bitmap = bitarray_create_with_mode(string_repeat('\0',configFS.cantidadBlocks), configFS.cantidadBlocks, LSB_FIRST);

	FILE* fBitmap = fopen(path,"wb+");
	fwrite(bitmap->bitarray,bitmap->size,1,fBitmap);
	fclose(fBitmap);
}

bool atraparPokemon(t_list* datosBloques, Posicion pos, t_list* numerosBloques, int* bytes) {
	DatosBloques* posYCant = encontrarPosicion(datosBloques,pos);

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

		escribirListaEnArchivo(datosBloques,numerosBloques);
		caught = true;
	}

	*bytes = configFS.tamanioBlocks*(list_size(numerosBloques) - 1) + tamanioBloque(list_get(numerosBloques,list_size(numerosBloques) - 1));

	return caught;
}

int agregarCantidadEnPosicion(t_list* pokemon, DatosBloques posYCant, t_list* numerosBloques) {

	DatosBloques* posicion = encontrarPosicion(pokemon,posYCant.pos);

	if(posicion == NULL) {

		log_info(logger,"Agregando la posicion");

		list_add(pokemon,&posYCant);

		int* valorRetorno = list_get(numerosBloques,list_size(numerosBloques)-1);

		char* val = string_itoa(*valorRetorno);
		char* path = pathBloque(val);
		free(val);

		FILE* bloque = fopen(path,"ab+");

		if (bloque != NULL) {
			char* cadena = posicionAString(&posYCant);

			fseek(bloque,0,SEEK_END);
			int tam = ftell(bloque);

			int bytesDesocupados = configFS.tamanioBlocks - tam;

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

			fclose(bloque);
		} else {
			log_error(logger,"No se pudo abrir el archivo del bloque");
		}
		free(path);
	} else {
		log_info(logger,"Sumando la cantidad en la posicion");
		posicion->cantidad += posYCant.cantidad;
		escribirListaEnArchivo(pokemon,numerosBloques);
	}

	free(posicion);

	return configFS.tamanioBlocks*(list_size(numerosBloques) - 1) + tamanioBloque(list_get(numerosBloques,list_size(numerosBloques) - 1));
}

void escribirListaEnArchivo(t_list* datosBloques, t_list* numerosBloques) {

//	log_info(logger, "Actualizando los bloques");

	char* cadenaGrande = string_new();

	for (int i = 0; i < list_size(datosBloques); i++) {
		DatosBloques* pok = list_get(datosBloques,i);
		char* cadena = posicionAString(pok);
		string_append(&cadenaGrande,cadena);
		free(cadena);
	}

//	pthread_mutex_lock(&semBitmap);
	if (strlen(cadenaGrande) > configFS.tamanioBlocks*list_size(numerosBloques)) {
		log_info(logger, "Pidiendo bloque");
		int* nuevoBloque = pedirBloque();
		list_add(numerosBloques,nuevoBloque);
	}
//	pthread_mutex_unlock(&semBitmap);
// 	TODO VAN ESTOS DOS??
//	pthread_mutex_lock(&semBitmap);
	if (strlen(cadenaGrande) <= configFS.tamanioBlocks*(list_size(numerosBloques) - 1)) {
		log_info(logger, "Borrando bloque");

		int* index = list_get(numerosBloques,list_size(numerosBloques) - 1);

		bitarray_clean_bit(bitmap,*index);

		char* ix = string_itoa(*index);
		char* p = pathBloque(ix);

		FILE* bloque = fopen(p,"wb+");
		fclose(bloque);

		free(ix);
		free(p);

		list_remove_and_destroy_element(numerosBloques,list_size(numerosBloques) - 1, &free);
	}
//	pthread_mutex_unlock(&semBitmap);

	int tamanioBloque = configFS.tamanioBlocks;

	for (int i = 0; i < list_size(numerosBloques); i++) {
		int* a = list_get(numerosBloques,i);
		log_info(logger, "Escribiendo bloque n%d", *a);

		char* bl = string_itoa(*a);
		char* p = pathBloque(bl);
		FILE* f = fopen(p,"wb+");
		free(bl);

		if (strlen(cadenaGrande) < configFS.tamanioBlocks) tamanioBloque = strlen(cadenaGrande);
		fwrite(cadenaGrande,tamanioBloque,1,f);
		fclose(f);

		free(p);
		if(strlen(cadenaGrande) > configFS.tamanioBlocks) cadenaGrande = string_substring_from(cadenaGrande, configFS.tamanioBlocks);
	}
	free(cadenaGrande);
}

DatosBloques* encontrarPosicion(t_list* pokemon, Posicion pos) {

	DatosBloques* dat;
	for (int i = 0; i < list_size(pokemon); i++) {
		dat = list_get(pokemon,i);
		Posicion pos2 = dat->pos;
		if (pos2.posX == pos.posX && pos2.posY == pos.posY) return dat;
	}
	return NULL;
}

NodoArbol* crearPokemon(char* nombre) {

//	log_info(logger,"Creando directorio para el pokemon");

	NodoArbol* nodo = crearNodo(nombre);

	char* pathNodo = string_new();
	string_append(&pathNodo,pathPtoMnt());
	string_append(&pathNodo,"/Files/");
	string_append(&pathNodo,nombre);

	mkdir(pathNodo,0700);

	crearMetadataPokemon(nombre);

	return nodo;
}

void crearMetadataPokemon(char* path) {

//	log_info(logger,"Creando metadata para el pokemon");
	char* pk = pathPokemon(path);

	FILE* metadata = fopen(pk,"wb+");

	fprintf(metadata,"DIRECTORY=N\n");
	fprintf(metadata,"SIZE=0\n");

	int* nuevoBloque = pedirBloque();
	char* bloques = string_new();
	string_append(&bloques,"BLOCKS=[");
	char* nuevoB = string_itoa(*nuevoBloque);
	string_append(&bloques,nuevoB);
	string_append(&bloques,"]\n");
	fputs(bloques,metadata);
	fputs("OPEN=N\n",metadata);

	free(nuevoB);
	free(bloques);
	free(nuevoBloque);
	fclose(metadata);
	free(pk);
}

t_list* convertirBloques(t_list* bloques, int cantBloques) {

	char* datosArchivo = leerArchivos(bloques,cantBloques);

	t_list* datos = interpretarCadena(datosArchivo,cantBloques);

	log_info(logger,"Bloques convertidos");

	free(datosArchivo);
	return datos;
}

char* leerArchivos(t_list* bloques, int cantBloques) {

	log_info(logger,"Leyendo los bloques");

	char* datos = string_new();

	for(int i = 0; i < cantBloques; i++) {

		int* bloque = list_get(bloques,i);

		char* bl = string_itoa(*bloque);

		char* pathBlocks = pathBloque(bl);

		free(bl);

		FILE* f = fopen(pathBlocks,"rb+");

		if(f != NULL || !feof(f) || tamanioArchivo(f) != 0) {
			char* leidos = calloc(1,configFS.tamanioBlocks);
			fread(leidos,configFS.tamanioBlocks,1,f);
			if(string_is_empty(leidos)) log_error(logger,"Bloque Mal: %d",*bloque);
			string_append(&datos,leidos);
			free(leidos);
		} else log_error(logger,"no se pudo abrir el archivo o algo paso aca ni idea");

		fclose(f);
		free(pathBlocks);
	}
	return datos;
}

t_list* interpretarCadena(char* cadenaDatos, int cantBloques) {

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
	char* bl = string_itoa(*nroBloque);
	char* path = pathBloque(bl);
	free(bl);

	FILE* bloque = fopen(path,"rb");

	fseek(bloque,0,SEEK_END);
	int a = ftell(bloque);

	fclose(bloque);

	free(path);

	return a;
}

int tamanioArchivo(FILE* arch) {
	fseek(arch,0,SEEK_END);
	return ftell(arch);
}

void cambiarMetadataPokemon(t_config* c, t_list* numerosBloques, int bytes) {

	char* size = string_itoa(bytes);
	config_set_value(c,"SIZE",size);
	free(size);

	char* bloques = string_new();
	string_append(&bloques,"[");
	for(int i = 0; i < list_size(numerosBloques); i++) {
		int* a = list_get(numerosBloques,i);
		char* bl = string_itoa(*a);
		string_append(&bloques,bl);
		free(bl);
		string_append(&bloques,",");
	}

	bloques[string_length(bloques)-1] = ']';
	config_set_value(c,"BLOCKS",bloques);

	config_set_value(c,"OPEN","N");

	free(bloques);
	config_save(c);
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
