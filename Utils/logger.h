#include <commons/log.h>

void Log_Inicializar(char* nombreArchivo, char* nombrePrograma, bool mostrarEnConsola, t_log_level nivel);
void Log_Finalizar();
void Log(char* mensaje);
void Log_Warning(char* mensaje);
void Log_Error(char* mensaje);
