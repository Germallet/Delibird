#include "servidorTeam.h"
#include "team.h"
#include "entrenador.h"
#include "interrupciones.h"
#include "../Utils/serializacion.h"
#include "../Utils/socket.h"

Servidor* servidor;

void operacionTEAM_APPEARED_POKEMON(Cliente* cliente, Paquete* paquete)
{
	Stream* stream_lectura = Stream_CrearLecturaPaquete(paquete);
	//Deserializar_uint32(stream_lectura);

	DATOS_APPEARED_POKEMON* datos = malloc(sizeof(DATOS_APPEARED_POKEMON));
	*datos = Deserializar_APPEARED_POKEMON(stream_lectura);
	agregar_interrupcion(I_APPEARED_POKEMON, datos);

	//Stream_Destruir(stream_lectura);
}

void IniciarServidorTeam(char* ip, int puerto)
{
	Eventos* eventos = Eventos_Crear0();
	Eventos_AgregarOperacion(eventos, APPEARED_POKEMON, (EventoOperacion)&operacionTEAM_APPEARED_POKEMON);

	servidor = CrearServidor(ip, puerto, eventos);
	if (servidor == NULL) log_error(logger, "Error al iniciar escucha (%s:%d)", ip, puerto);
}

