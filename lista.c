#include "lista.h"
#include <stdlib.h>

typedef struct nodo {
    void* dato;
    struct nodo* prox;
} nodo_t;

struct lista {
    nodo_t* prim;
    nodo_t* ult;
    size_t largo;
};

struct lista_iter {
    lista_t *lista;
    nodo_t* anterior;
    nodo_t* actual;
};

nodo_t *nodo_crear(void *dato, nodo_t *prox) {
    nodo_t *nodo = malloc(sizeof(nodo_t));

    if (nodo == NULL) {
        return NULL;
    }
    nodo->dato = dato;
    nodo->prox = prox;
    return nodo;
}

lista_t *lista_crear(void) {
    lista_t *lista = malloc(sizeof(lista_t));
    if (!lista) return NULL;
    
    lista->prim = NULL;
    lista->ult = NULL;
    lista->largo = 0;
    return lista;
}

bool lista_esta_vacia(const lista_t *lista) {
    return lista->largo == 0;
}

bool lista_insertar_primero(lista_t *lista, void *dato) {
    nodo_t *nuevo_nodo = nodo_crear(dato, lista->prim);
    if (!nuevo_nodo) return false;

    if (lista_esta_vacia(lista)) {
        lista->ult = nuevo_nodo;
    }
    lista->prim = nuevo_nodo;
    lista->largo++;
    return true;
}  

bool lista_insertar_ultimo(lista_t *lista, void *dato) {
    nodo_t *nuevo_nodo = nodo_crear(dato, NULL);
    if (!nuevo_nodo) return false;

    if (lista_esta_vacia(lista)) {
        lista->prim = nuevo_nodo;
    } else {
        lista->ult->prox = nuevo_nodo;
    }
    lista->ult = nuevo_nodo;
    lista->largo++;
    return true;
}

void *lista_borrar_primero(lista_t *lista) {
    if (lista_esta_vacia(lista)) return NULL;

    nodo_t *nodo_a_eliminar = lista->prim;
    void *dato = nodo_a_eliminar->dato;

    if (lista->largo == 1) lista->ult = NULL;
    lista->prim = lista->prim->prox;
    lista->largo--;
    free(nodo_a_eliminar);
    return dato;
}

void *lista_ver_primero(const lista_t *lista) {
    if (lista_esta_vacia(lista)) return NULL;
    return lista->prim->dato;
}

void *lista_ver_ultimo(const lista_t* lista) {
    if (lista_esta_vacia(lista)) return NULL;
    return lista->ult->dato;
}

size_t lista_largo(const lista_t *lista) {
    return lista->largo;
}

void lista_destruir(lista_t *lista, void (*destruir_dato)(void *)) {
    while (lista->prim) {
        if (destruir_dato != NULL) destruir_dato(lista->prim->dato);
        nodo_t *nodo = lista->prim;
        lista->prim = lista->prim->prox;
        free(nodo);
    }
    free(lista);
}

lista_iter_t *lista_iter_crear(lista_t *lista) {
    lista_iter_t *iter = malloc(sizeof(lista_iter_t));
    if (iter == NULL) return NULL;

    iter->lista = lista;
    iter->actual = lista->prim;
    iter->anterior = NULL;
    return iter;
}

bool lista_iter_avanzar(lista_iter_t *iter) {
    if (iter->actual == NULL) return false;
    iter->anterior = iter->actual;
    iter->actual = iter->actual->prox;
    return true;
}

void *lista_iter_ver_actual(const lista_iter_t *iter) {
    if (iter->actual == NULL) return NULL;
    return iter->actual->dato;
}

bool lista_iter_al_final(const lista_iter_t *iter) {
    return iter->actual == NULL;
}

void lista_iter_destruir(lista_iter_t *iter) {
    free(iter);
}

bool lista_iter_insertar(lista_iter_t *iter, void *dato) {
    nodo_t *nuevo_nodo = nodo_crear(dato, iter->actual);
    if (nuevo_nodo == NULL) return false;
    
    //verifica si el iterador se encuentra en el primer elemento
    if (iter->anterior == NULL) {
        iter->lista->prim = nuevo_nodo;
    } else { //cualquier otro caso 
        iter->anterior->prox = nuevo_nodo;
    }
    //verifica si el iterador se encuentra al final o si la lista esta vacia
    if (lista_iter_al_final(iter) || iter->actual == NULL) { 
        iter->lista->ult = nuevo_nodo;
    }
    iter->lista->largo++;
    iter->actual = nuevo_nodo;
    return true;
}

void *lista_iter_borrar(lista_iter_t *iter) {
    if (lista_iter_al_final(iter)) return NULL;
    nodo_t *nodo_a_eliminar = iter->actual;
    void* dato = iter->actual->dato;
    
    //verifica si el iterador se encuentra en el primer elemento
    if (iter->anterior == NULL) {
        iter->lista->prim = iter->lista->prim->prox;
    } else { //cualquier otro caso 
        iter->anterior->prox = iter->actual->prox;
    }
    //verifica si el eliminado es el ultimo de la lista
    if (iter->actual->prox == NULL) {
        iter->lista->ult = iter->anterior;
    }
    iter->actual = iter->actual->prox;
    iter->lista->largo--;
    free(nodo_a_eliminar);
    return dato;
}

void lista_iterar(lista_t *lista, bool visitar(void *dato, void *extra), void *extra) {
    nodo_t* actual = lista->prim;

    while (actual && visitar(actual->dato, extra)) {
        actual = actual->prox;
    } 
}
