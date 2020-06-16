#include <commons/log.h>
#include <commons/config.h>
#include <commons/bitarray.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "arbol.h"
#include "archivos.h"
#include "../Utils/socket.h"
#include "../Utils/hiloTimer.h"
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

//typedef struct {
//	uint32_t tamanioBlocks;
//	uint32_t cantidadBlocks;
////	NodoArbol* arbolDirectorios; NO SABRIA SI ESTO ESTARIA BUENO ACA
//
//} ConfigFS;
//
//ConfigFS* configFS;

t_log* logger;

Cliente* clienteBroker;

t_config* config;

NodoArbol* raiz;

t_bitarray* bitmap;

Servidor* servidor;

t_list* mensajesNoEnviados;

void EscuchaSignal(int signo);
char* pathBloque(char* nombre);
void conectarse();
void reconexion();
void tallGrass_init(char* puntoMontaje);
void crearDirectorio(char* path);
void TerminarProgramaConError(char* error);
void TerminarPrograma();
void EsperarHilos();
bool existePokemon(char* nombre);
NodoArbol* encontrarPokemon(char* nombre);
NodoArbol* directorioFiles();
NodoArbol* directorioBlocks();
char* pathDeNodo(NodoArbol* nodo);
char* pathPtoMnt();
char* pathPokemon(char* nombre);
int pedirBloque();
NodoArbol* directorioMetadata();
int buscarPosicionLibre();
NodoArbol* directorio(char* str);
