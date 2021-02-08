#ifndef LISTA_H
#define LISTA_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

/* ******************************************************************
 *                DEFINICION DE LOS TIPOS DE DATOS
 * *****************************************************************/

typedef struct lista lista_t;

typedef struct lista_iter lista_iter_t;

/* *****************************************************************
 *                      PRIMITIVAS DE LA LISTA
 * *****************************************************************/

// Crea una lista.
// Post: devuelve una nueva lista vacía.
lista_t *lista_crear(void);

// Destruye la lista. Si se recibe la función destruir_dato por parámetro,
// para cada uno de los elementos de la lista llama a destruir_dato.
// Pre: la lista fue creada. destruir_dato es una función capaz de destruir
// los datos de la lista, o NULL en caso de que no se la utilice.
// Post: se eliminaron todos los elementos de la cola.
void lista_destruir(lista_t *lista, void (*destruir_dato)(void *));

// Devuelve verdadero si la lista no tiene elementos, false en caso contrario.
// Pre: la lista fue creada.
bool lista_esta_vacia(const lista_t *lista);

// Agrega un nuevo elemento al inicio de la lista. Devuelve falso en caso de error.
// Pre: la lista fue creada.
// Post: se agregó un nuevo elemento al inicio de la lista.
bool lista_insertar_primero(lista_t *lista, void *dato);

// Agrega un nuevo elemento al final de la lista. Devuelve falso en caso de error.
// Pre: la lista fue creada.
// Post: se agregó un nuevo elemento al final de la lista.
bool lista_insertar_ultimo(lista_t *lista, void *dato);

//Elimina el primer elemento de la lista y lo devuelve.
//Pre: la lista fue creada
//Post: se elimino el primer elemento
void *lista_borrar_primero(lista_t *lista);

// Obtiene el valor del primer elemento de la lista. Si la lista tiene
// elementos, se devuelve el valor del primero, si está vacía devuelve NULL.
// Pre: la lista fue creada.
// Post: se devolvió el primer elemento de la lista, cuando no está vacía.
void *lista_ver_primero(const lista_t *lista);

// Obtiene el valor del ultimo elemento de la lista. Si la lista tiene
// elementos, se devuelve el valor del ultimo, si está vacía devuelve NULL.
// Pre: la lista fue creada.
// Post: se devolvió el ultimo elemento de la lista, cuando no está vacía.
void *lista_ver_ultimo(const lista_t* lista);

// Devuelve el largo de la lista (cantidad de elementos en ella)
// Pre: la lista fue creada.
// Post: se devolvio el largo de la lista. 
size_t lista_largo(const lista_t *lista);

/* *****************************************************************
 *                      PRIMITIVAS DEL ITERADOR EXTERNO
 * *****************************************************************/

// Crea un iterador.
// Pre: la lista fue creada.
// Post: se devolvio un iterador que apunta al primer elemento de la lista. 
lista_iter_t *lista_iter_crear(lista_t *lista);

// Avanza al siguiente dato de la lista.
// Pre: el itedor fue creado.
// Post: el iterador avanzo un dato.
bool lista_iter_avanzar(lista_iter_t *iter);

// Obtiene el dato almacenado en la posicion alctual a la que apunta el 
//iterador. Si la lista esta vacia, devuleve NULL.
// Pre: el iterador fue creado
// Post: se devolvio el dato actual o NULL en caso de que la lista este vacia
void *lista_iter_ver_actual(const lista_iter_t *iter);

// Devuelve true si el iterador se encuentra al final de la lista, es decir, 
// apunta al ultimo dato en esta. 
// Pre: el iterador fue creado.
bool lista_iter_al_final(const lista_iter_t *iter);

// Destruye el iterador y todos sus datos.
// Pre: el iterador fue creado.
// Post: el iterador y sus datos fueron destruidos.
void lista_iter_destruir(lista_iter_t *iter);

// Inserta el dato en la posicion anterior al que apunta actualmente
// el iterador. Devulve NULL si se produce un error. 
// Pre: el iterador fue creado
// Post: se insterto el dato en la posicion previa al acual, devuelve
// false en caso de fallar. 
bool lista_iter_insertar(lista_iter_t *iter, void *dato);

// Elimina el dato actual de la lista al que apunta el iterador  y lo devuelve. 
// Devuelve NULL en caso de fallar
// Pre: el iterador fue creado
// Post: se elimino y devolvio el dato actual. Devolvio NULL en caso de error. 
void *lista_iter_borrar(lista_iter_t *iter);

/* *****************************************************************
 *                      DECLARACION ITERADOR INTERNO
 * *****************************************************************/

// visita cada elemento y le aplica la funcion visitar, si devuelve true continua
// iterando o hasta llegar al final de la lista. 
void lista_iterar(lista_t *lista, bool visitar(void *dato, void *extra), void *extra);

/* *****************************************************************
 *                      PRUEBAS UNITARIAS
 * *****************************************************************/

void pruebas_lista_estudiante(void);

#endif