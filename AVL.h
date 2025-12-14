#ifndef AVL_H
#define AVL_H

#include "estructuras.h"
#include <algorithm> 

struct Nodo {
  Paciente dato;   // informacion del paciente
  Nodo* izq;           // hijo izquierdo
  Nodo* der;           // hijo derecho
  int altura;          // altura del arbol
};


// Prototipos de funciones 

int obtenerAltura(Nodo* n);
int obtenerBalance(Nodo* n);
int mayor(int a, int b);

// Funciones de rotacion para arbol AVL
// se pasan punteros simples para que retornen la nueva raiz
Nodo* rotacioneDerecha(Nodo* y);
Nodo* rotacioneIzquierda(Nodo* x);

//Funciones principales
Nodo* nuevoNodo(Paciente paciente);

void insertarPaciente(Nodo* &raiz, Paciente paciente); // Paso por referencia de puntero
void eliminarPacienteMayorPrioridad(Nodo* &raiz);      // Paso por referencia
void mostrarArbol(Nodo* raiz, int tipoRecorrido);      // tipoRecorrido { 0:Pre, 1:In, 2:Post }


#endif