#include "configLindas.h"

t_config *CrearConfig(char *path) {
	FILE* file = fopen(path, "ab+");
//	flockfile(file);

	if (file == NULL)  return NULL;

	struct stat stat_file;
	stat(path, &stat_file);

	t_config *config = malloc(sizeof(t_config));

	config->path = strdup(path);
	config->properties = dictionary_create();

	char* buffer = calloc(1, stat_file.st_size + 1);
	fread(buffer, stat_file.st_size, 1, file);

	if (strstr(buffer, "\r\n")) {
		printf("\n\nconfig_create - WARNING: the file %s contains a \\r\\n sequence "
		 "- the Windows new line sequence. The \\r characters will remain as part "
		 "of the value, as Unix newlines consist of a single \\n. You can install "
		 "and use `dos2unix` program to convert your files to Unix style.\n\n", path);
	}

	char** lines = string_split(buffer, "\n");

	void add_cofiguration(char *line) {
		if (!string_starts_with(line, "#")) {
			char** keyAndValue = string_n_split(line, 2, "=");
			dictionary_put(config->properties, keyAndValue[0], keyAndValue[1]);
			free(keyAndValue[0]);
			free(keyAndValue);
		}
	}
	string_iterate_lines(lines, add_cofiguration);
	string_iterate_lines(lines, (void*) free);

	free(lines);
	free(buffer);

//	funlockfile(file);

	fclose(file);

	return config;
}

int GuardarConfig(t_config *self, char* path) {
	FILE* file = fopen(path, "wb+");
	flockfile(file);

	if (file == NULL) {
		return -1;
	}

	char* lines = string_new();
	void add_line(char* key, void* value) {
		string_append_with_format(&lines, "%s=%s\n", key, value);
	}

	dictionary_iterator(self->properties, add_line);
	int result = fwrite(lines, strlen(lines), 1, file);

	funlockfile(file);
	fclose(file);
	free(lines);
	return result;
}

int ActualizarConfig(t_config *self) {
	return GuardarConfig(self, self->path);
}

