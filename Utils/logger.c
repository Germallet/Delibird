#include "logger.h"

t_log* logger;

void Log_Inicializar(char* nombreArchivo, char* nombrePrograma, bool mostrarEnConsola, t_log_level nivel)
{
	logger = log_create(nombreArchivo, nombrePrograma, mostrarEnConsola, nivel);
}
void Log_Finalizar()
{
	log_destroy(logger);
}

void Log(char* mensaje)
{
	log_info(logger, mensaje);
}
void Log_Warning(char* mensaje)
{
	log_warning(logger, mensaje);
}
void Log_Error(char* mensaje)
{
	log_error(logger, mensaje);
}
