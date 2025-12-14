// Módulo 1 (Árbol AVL).
#include "AVL.h"
#include <iostream>

using namespace std;

int  obtenerAltura(Nodo* n) {
  if(n == NULL) return 0;
  return n->altura;
}

int mayor(int a, int b){
  return (a > b) ? a : b;
}

int obtenerBalance(Nodo* n){
  if(n==NULL) return 0;
  return obtenerAltura(n->izq) - obtenerAltura(n->der);
}

Nodo* nuevoNodo(Paciente paciente) {
  Nodo* nodo = new Nodo();
  nodo->dato = paciente;
  nodo->izq = NULL;
  nodo->der = NULL;
  nodo->altura = 1;
  return nodo;
}


// Rotaciones del diavl

Nodo* rotacionDerecha(Nodo* y){
  Nodo* x = y->izq;
  Nodo* z = x->der;

  // realizando rotaciones
  x->der = y;
  y->izq = z;

  // actualizando alturas
  y->altura = mayor(obtenerAltura(y->izq), obtenerAltura(y->der)) +1;
  x->altura = mayor(obtenerAltura(x->izq), obtenerAltura(x->der)) +1;

  return x;
}

Nodo* rotacionIzquierda(Nodo* x){
  Nodo* y = x->der;
  Nodo* z = y->izq;

  // Realizando rotaciones
  y->izq = x;
  x->der = z;

  // Actualizando alturas
  x->altura = mayor(obtenerAltura(x->izq), obtenerAltura(x->der)) +1;
  y->altura = mayor(obtenerAltura(y->izq), obtenerAltura(y->der)) +1;

  return y;
}