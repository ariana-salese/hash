#define _POSIX_C_SOURCE 200809L
#include "hash.h"
#include "lista.h"
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define FACTOR_REDIMENSION 2
#define FACTOR_DE_CARGA 10
#define CAPACIDAD_INICIAL 17
#define REDIMENSIONES_MAX 18

typedef struct hash_elem {
    char* clave;
    void* dato;
} hash_elem_t; 

struct hash {
    lista_t** tabla_hash; 
    size_t capacidad; 
    size_t cantidad;
    size_t pos_capacidad;
    hash_destruir_dato_t destruir_dato;
}; 

struct hash_iter {
    const hash_t *hash;
    lista_iter_t* iter_lista_actual;
    size_t indice_lista_actual;
};

/* ******************************************************************
 *                       FUNCIONES AUXILIARES
 * *****************************************************************/

//FUNCION DE HASHING: recibe una cadena y devuelve un numero
size_t hash_djb2(char *str) {
    size_t hash = 5381;
    int c;

    while ((c = *str++)) hash = ((hash << 5) + hash) + c;

    return hash;
}

// Recibe una cadena y la capacidad del hash y devuelve la posicion que le corresponde
size_t funcion_hash(const char* str, size_t cap) {

    char *copia_str = strdup(str);
    size_t resultado = hash_djb2(copia_str);
    free(copia_str);

    return resultado % cap;
}

//Recibe una tabla de hash y la completa con listas vacias. Devuelve false en 
//caso de error
bool completar_tabla(lista_t **tabla_hash, size_t capacidad) {
    for (size_t i = 0; i < capacidad; i++) {
        tabla_hash[i] = lista_crear();
        if (!tabla_hash[i]) { 
            for (size_t j = 0; j <= i; j++) {
                lista_destruir(tabla_hash[j], NULL);
            }
            return false;
        }
    }
    return true;
}

//Crea un elemento del hash. Devuelve false en caso de error. 
hash_elem_t* hash_elem_crear(const char* clave, void* dato) {

    hash_elem_t* hash_elem = malloc(sizeof(hash_elem_t));
    if(!hash_elem) return NULL;

    char *copia_clave = strdup(clave);

    if (!copia_clave) {
        free(hash_elem);
        return NULL;
    }

    hash_elem->clave = copia_clave;
    hash_elem->dato = dato;
    
    return hash_elem;
}

//Recorre una lista pasada por parámetro y devuelve el elemento con la clave correspondiente
//Si no se encuentra el elemento devuelve NULL
//Si el parámetro borrar es true además de devolverlo lo borra de la lista
hash_elem_t* buscar_elem_lista(lista_t* lista, const char* clave, bool borrar) {

	lista_iter_t* iter = lista_iter_crear(lista);	
    if (!iter) return NULL;

    hash_elem_t* elem_a_devolver = NULL;

    while (!lista_iter_al_final(iter)){
    	if(!strcmp(((hash_elem_t*)lista_iter_ver_actual(iter))->clave, clave)) {
    		if(borrar) elem_a_devolver = (hash_elem_t*)lista_iter_borrar(iter);   		   		
    		else elem_a_devolver = (hash_elem_t*)lista_iter_ver_actual(iter);
    	}
    	lista_iter_avanzar(iter);
    }
    lista_iter_destruir(iter);

    return elem_a_devolver;
}

const size_t primos[] = {17,31,61,127,257,509,1021,2039,4093,
8191,16381,32771,65537,131071,262139,524287,1048573,2097143,4194301};

/* ******************************************************************
 *                       PRIMITIVAS DEL HASH
 * *****************************************************************/

hash_t *hash_crear(hash_destruir_dato_t destruir_dato) {
    hash_t *hash = malloc(sizeof(hash_t));
    if (!hash) return NULL;
    
    lista_t **tabla_hash = malloc(sizeof(lista_t*) * CAPACIDAD_INICIAL); 
    if(!tabla_hash) {
        free(hash);
        return NULL;
    }
       
    hash->tabla_hash = tabla_hash;
    hash->capacidad = CAPACIDAD_INICIAL;
    hash->cantidad = 0;
    hash->pos_capacidad = 0;
    hash->destruir_dato = destruir_dato;

    if(!completar_tabla(hash->tabla_hash, hash->capacidad)){
        free(tabla_hash);
        free(hash);
        return NULL;
    } 
    return hash;
}

bool hash_redimensionar(hash_t* hash, size_t nueva_capacidad) {

	lista_t **nueva_tabla_hash = malloc(sizeof(lista_t*) * nueva_capacidad); 
    if(!nueva_tabla_hash) return false;
    
    if(!completar_tabla(nueva_tabla_hash, nueva_capacidad)){
        free(nueva_tabla_hash); 
        return false;
    }

    for (size_t i = 0; i < hash->capacidad; i++) {
        lista_t *lista_actual = hash->tabla_hash[i];
        while (!lista_esta_vacia(lista_actual)) {
            hash_elem_t *hash_elem_actual = (hash_elem_t*)lista_borrar_primero(lista_actual); 
            size_t pos = funcion_hash(hash_elem_actual->clave, nueva_capacidad);
            if (!lista_insertar_ultimo(nueva_tabla_hash[pos], hash_elem_actual)) false;
        }
        lista_destruir(lista_actual, NULL);
    }
    free(hash->tabla_hash);
    hash->capacidad = nueva_capacidad;
    hash->tabla_hash = nueva_tabla_hash;
    
    return true;
}

bool hash_guardar(hash_t *hash, const char *clave, void *dato) {
    
    size_t pos = funcion_hash(clave, hash->capacidad);

    hash_elem_t* hash_elem = hash_elem_crear(clave, dato):

    void* dato_ant = hash_borrar(hash, clave); 
    if (dato_ant && hash->destruir_dato) hash->destruir_dato(dato_ant); 

    lista_insertar_ultimo(hash->tabla_hash[pos], hash_elem);
 
    hash->cantidad++;

    if (lista_largo(hash->tabla_hash[pos]) > FACTOR_DE_CARGA && hash->pos_capacidad < REDIMENSIONES_MAX){
        hash->pos_capacidad++;
        return(hash_redimensionar(hash,primos[hash->pos_capacidad]));
    }

    return true;
}

void *hash_borrar(hash_t *hash, const char *clave) {

	size_t pos = funcion_hash(clave, hash->capacidad);

	hash_elem_t* elem_a_borrar = buscar_elem_lista(hash->tabla_hash[pos], clave, true);

	if(!elem_a_borrar) return NULL;

	void* dato = elem_a_borrar->dato;

    free(elem_a_borrar->clave);
	free(elem_a_borrar);

	hash->cantidad--;

    if (hash->cantidad * 4 <= hash->capacidad && hash->pos_capacidad > 0) {
    	hash->pos_capacidad--;
        hash_redimensionar(hash, primos[hash->pos_capacidad]);
    }

	return dato;
}

void *hash_obtener(const hash_t *hash, const char *clave) {
    
    size_t pos = funcion_hash(clave, hash->capacidad);

    hash_elem_t* elem = buscar_elem_lista(hash->tabla_hash[pos], clave, false);

    if(elem) return elem->dato;
    return NULL;
}

bool hash_pertenece(const hash_t *hash, const char *clave) {

    size_t pos = funcion_hash(clave, hash->capacidad);

    hash_elem_t* elem = buscar_elem_lista(hash->tabla_hash[pos], clave, false);

    if(!elem) return false;
    return true;
}

size_t hash_cantidad(const hash_t *hash) {
    return hash->cantidad;
}

void hash_destruir(hash_t *hash) {
    for (size_t i = 0; i < hash->capacidad; i++) {
        lista_t *lista_actual = hash->tabla_hash[i];
        
        while (!lista_esta_vacia(lista_actual)) {
            hash_elem_t *elem_a_borrar = lista_borrar_primero(lista_actual);
            if(hash->destruir_dato) hash->destruir_dato(elem_a_borrar->dato); 
            free(elem_a_borrar->clave);        
            free(elem_a_borrar);
        }
        lista_destruir(lista_actual, NULL);
    }
    free(hash->tabla_hash);
    free(hash);
}

/* ******************************************************************
 *                      PRIMITIVAS DEL ITERADOR
 * *****************************************************************/

void posicionar_en_primer_elem(hash_iter_t *iter) {

	if(!lista_largo(iter->hash->tabla_hash[iter->indice_lista_actual]))
		hash_iter_avanzar(iter);
}

hash_iter_t *hash_iter_crear(const hash_t *hash) {

    hash_iter_t *iter = malloc(sizeof(hash_iter_t));

    if(!iter) return NULL;
    iter->hash = hash;
    iter->indice_lista_actual = 0;

    iter->iter_lista_actual = lista_iter_crear(hash->tabla_hash[iter->indice_lista_actual]);

    posicionar_en_primer_elem(iter);

    return iter;
}

bool hash_iter_avanzar(hash_iter_t *iter) {

	if(hash_iter_al_final(iter)) return false;

	//Si no esta al final de la lista avanza
	if(!lista_iter_al_final(iter->iter_lista_actual)){	
		lista_iter_avanzar(iter->iter_lista_actual);
		if(!lista_iter_al_final(iter->iter_lista_actual)) return true; //Si sigue sin estar al final devuelve true
	} 

	iter->indice_lista_actual++;
	lista_iter_destruir(iter->iter_lista_actual);
	
	//Mientras este vacia, busco en la prox
	while(iter->indice_lista_actual < iter->hash->capacidad && !lista_largo(iter->hash->tabla_hash[iter->indice_lista_actual])){
		iter->indice_lista_actual++;
	}

	if(iter->indice_lista_actual == iter->hash->capacidad){
		iter->iter_lista_actual = NULL; //Para simbolizar el fin del iter del hash, dejo esta variable en NULL
		return true;
	}

	//Cuando encontré una lista que no está vacía, creo el iterador
	iter->iter_lista_actual = lista_iter_crear(iter->hash->tabla_hash[iter->indice_lista_actual]);
	return true;	
}

const char *hash_iter_ver_actual(const hash_iter_t *iter) {

	if(!hash_iter_al_final(iter)){
		return ((hash_elem_t*)(lista_iter_ver_actual(iter->iter_lista_actual)))->clave;
	}
    return NULL;
}

bool hash_iter_al_final(const hash_iter_t *iter) {

    if(iter->iter_lista_actual == NULL) return true;
    return false;
}

void hash_iter_destruir(hash_iter_t *iter) {
    lista_iter_destruir(iter->iter_lista_actual);
    free(iter);
}
