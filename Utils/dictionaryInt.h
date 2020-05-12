#pragma once
#include <commons/collections/dictionary.h>
#include <commons/collections/list.h>
#include <netdb.h>

typedef struct
{
	t_dictionary* dictionary;
} t_dictionaryInt;

extern t_dictionaryInt* dictionaryInt_create();
extern void dictionaryInt_put(t_dictionaryInt* diccionario, uint32_t key, void *data);
extern void *dictionaryInt_get(t_dictionaryInt* diccionario, uint32_t key);
extern void *dictionaryInt_remove(t_dictionaryInt* diccionario, uint32_t key);
extern void dictionaryInt_remove_and_destroy(t_dictionaryInt* diccionario, uint32_t key, void(*data_destroyer)(void*));
extern void dictionaryInt_iterator(t_dictionaryInt* diccionario, void(*closure)(uint32_t,void*));
extern void dictionaryInt_clean(t_dictionaryInt* diccionario);
extern void dictionaryInt_clean_and_destroy_elements(t_dictionaryInt* diccionario, void(*data_destroyer)(void*));
extern bool dictionaryInt_has_key(t_dictionaryInt* diccionario, uint32_t key);
extern bool dictionaryInt_is_empty(t_dictionaryInt* diccionario);
extern int dictionaryInt_size(t_dictionaryInt* diccionario);
extern void dictionaryInt_destroy(t_dictionaryInt* diccionario);
extern void dictionaryInt_destroy_and_destroy_elements(t_dictionaryInt* diccionario, void(*data_destroyer)(void*));

extern t_list* dictionaryInt_toList(t_dictionaryInt* diccionario);
