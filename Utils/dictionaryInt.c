#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "dictionaryInt.h"

t_dictionaryInt* dictionaryInt_create()
{
	t_dictionaryInt* nuevoDiccionarioInt = malloc(sizeof(t_dictionaryInt));
	nuevoDiccionarioInt->dictionary = dictionary_create();
	return nuevoDiccionarioInt;
}

void dictionaryInt_put(t_dictionaryInt* diccionario, uint32_t key, void *data)
{
	char keyArray[10];
	sprintf(keyArray, "%d", key);
	dictionary_put(diccionario->dictionary, keyArray, data);
}

void *dictionaryInt_get(t_dictionaryInt* diccionario, uint32_t key)
{
	char keyArray[10];
	sprintf(keyArray, "%d", key);
	return dictionary_get(diccionario->dictionary, keyArray);
}

void *dictionaryInt_remove(t_dictionaryInt* diccionario, uint32_t key)
{
	char keyArray[10];
		sprintf(keyArray, "%d", key);
	return dictionary_remove(diccionario->dictionary, keyArray);
}

void dictionaryInt_remove_and_destroy(t_dictionaryInt* diccionario, uint32_t key, void(*data_destroyer)(void*))
{
	char keyArray[10];
	sprintf(keyArray, "%d", key);
	dictionary_remove_and_destroy(diccionario->dictionary, keyArray, data_destroyer);
}

void dictionaryInt_iterator(t_dictionaryInt* diccionario, void(*closure)(uint32_t,void*))
{
	int table_index;
	for (table_index = 0; table_index < diccionario->dictionary->table_max_size; table_index++) {
		t_hash_element *element = diccionario->dictionary->elements[table_index];

		while (element != NULL) {
			uint32_t key;
			sscanf(element->key, "%d", &key);
			closure(key, element->data);
			element = element->next;
		}
	}
}

void dictionaryInt_clean(t_dictionaryInt* diccionario)
{
	dictionary_clean(diccionario->dictionary);
}

void dictionaryInt_clean_and_destroy_elements(t_dictionaryInt* diccionario, void(*data_destroyer)(void*))
{
	dictionary_clean_and_destroy_elements(diccionario->dictionary, data_destroyer);
}

bool dictionaryInt_has_key(t_dictionaryInt* diccionario, uint32_t key)
{
	char keyArray[10];
	sprintf(keyArray, "%d", key);
	return dictionary_has_key(diccionario->dictionary, keyArray);
}

bool dictionaryInt_is_empty(t_dictionaryInt* diccionario)
{
	return dictionary_is_empty(diccionario->dictionary);
}

int dictionaryInt_size(t_dictionaryInt* diccionario)
{
	return dictionary_size(diccionario->dictionary);
}

void dictionaryInt_destroy(t_dictionaryInt* diccionario)
{
	dictionary_destroy(diccionario->dictionary);
	free(diccionario);
}

void dictionaryInt_destroy_and_destroy_elements(t_dictionaryInt* diccionario, void(*data_destroyer)(void*))
{
	dictionary_destroy_and_destroy_elements(diccionario->dictionary, data_destroyer);
}


t_list* dictionaryInt_toList(t_dictionaryInt* diccionario)
{
	t_list* lista = list_create();

	int table_index;
	for (table_index = 0; table_index < diccionario->dictionary->table_max_size; table_index++) {
		t_hash_element *element = diccionario->dictionary->elements[table_index];

		while (element != NULL) {
			list_add(lista, element->data);
			element = element->next;
		}
	}

	return lista;
}
