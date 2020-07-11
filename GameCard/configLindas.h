#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include <commons/config.h>
#include <commons/string.h>

t_config *CrearConfig(char *path);
int ActualizarConfig(t_config *self);
int GuardarConfig(t_config *self, char* path);
