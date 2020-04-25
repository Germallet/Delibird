typedef struct
{
	char* nombre;
	int posX;
	int posY;
	int ID_MENSAJE;
} DATOS_APPEARED_POKEMON;

extern DATOS_APPEARED_POKEMON* Deserializar_APPEARED_POKEMON();
extern void* Serializar_APPEARED_POKEMON(DATOS_APPEARED_POKEMON* datos, int* tamanioBuffer);
