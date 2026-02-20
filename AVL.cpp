#include "AVL.h"
#include <iostream>

using namespace std;

int obtenerAltura(Nodo* n) {
    if (n == NULL) return 0;
    return n->altura;
}

int mayor(int a, int b) {
    return (a > b) ? a : b;
}

int obtenerBalance(Nodo* n) {
    if (n == NULL) return 0;
    return obtenerAltura(n->izq) - obtenerAltura(n->der);
}

Nodo* nuevoNodo(Paciente p) {
    Nodo* nodo = new Nodo();
    nodo->dato = p;
    nodo->izq = NULL;  
    nodo->der = NULL;  
    nodo->altura = 1;
    return nodo;
}

Nodo* rotacionDerecha(Nodo* y) {
    Nodo* x = y->izq;
    Nodo* T2 = x->der;
    x->der = y;
    y->izq = T2;
    y->altura = mayor(obtenerAltura(y->izq), obtenerAltura(y->der)) + 1;
    x->altura = mayor(obtenerAltura(x->izq), obtenerAltura(x->der)) + 1;
    return x;
}

Nodo* rotacionIzquierda(Nodo* x) {
    Nodo* y = x->der;
    Nodo* T2 = y->izq;
    y->izq = x;
    x->der = T2;
    x->altura = mayor(obtenerAltura(x->izq), obtenerAltura(x->der)) + 1;
    y->altura = mayor(obtenerAltura(y->izq), obtenerAltura(y->der)) + 1;
    return y;
}

// Función auxiliar para saber si una cédula ya existe en la sala de espera
bool existeEnAVL(Nodo* raiz, string cedula) {
    // Si llegamos a una hoja vacía, no se encontró
    if (raiz == NULL) {
        return false;
    }
    
    // Si el nodo actual tiene la cédula, ¡existe!
    if (raiz->dato.cedula == cedula) {
        return true;
    }
    
    // Buscamos en la rama izquierda y en la rama derecha
    return existeEnAVL(raiz->izq, cedula) || existeEnAVL(raiz->der, cedula);
}

// Función auxiliar para saber quién tiene más prioridad
bool esMasPrioritario(Paciente nuevo, Paciente actual) {
    if (nuevo.urgencia < actual.urgencia) {
        return true; // 0 (Critico) gana sobre 3 (Leve)
    } 
    else if (nuevo.urgencia == actual.urgencia) {
        return nuevo.numLlegada < actual.numLlegada; // Si es igual, gana el que llegó antes (ticket menor)
    }
    return false;
}

Nodo* insertarRecursivo(Nodo* nodo, Paciente p) {
    if (nodo == NULL) return nuevoNodo(p);

    if (esMasPrioritario(p, nodo->dato)) {
        nodo->izq = insertarRecursivo(nodo->izq, p);
    } else {
        nodo->der = insertarRecursivo(nodo->der, p);
    }

    nodo->altura = 1 + mayor(obtenerAltura(nodo->izq), obtenerAltura(nodo->der));
    int balance = obtenerBalance(nodo);

    // Casos de rotación simplificados y seguros
    if (balance > 1 && esMasPrioritario(p, nodo->izq->dato)) {
        return rotacionDerecha(nodo);
    }
    if (balance < -1 && !esMasPrioritario(p, nodo->der->dato)) {
        return rotacionIzquierda(nodo);
    }
    if (balance > 1 && !esMasPrioritario(p, nodo->izq->dato)) {
        nodo->izq = rotacionIzquierda(nodo->izq);
        return rotacionDerecha(nodo);
    }
    if (balance < -1 && esMasPrioritario(p, nodo->der->dato)) {
        nodo->der = rotacionDerecha(nodo->der);
        return rotacionIzquierda(nodo);
    }
    
    return nodo;
}

void insertarPaciente(Nodo* &raiz, Paciente p) {
    raiz = insertarRecursivo(raiz, p);
    // Eliminado el cout para no romper la interfaz gráfica
}

Nodo* eliminarRecursivo(Nodo* raiz, Paciente& paciente) {
    if(raiz == NULL) return raiz;

    if(raiz->izq != NULL) {
        raiz->izq = eliminarRecursivo(raiz->izq, paciente);
    } else {
        paciente = raiz->dato;
        Nodo* aux = raiz->der;
        delete raiz;
        return aux;
    }

    raiz->altura = 1 + mayor(obtenerAltura(raiz->izq), obtenerAltura(raiz->der));
    int balance = obtenerBalance(raiz);

    if(balance > 1 && obtenerBalance(raiz->izq) >= 0) return rotacionDerecha(raiz);
    if(balance > 1 && obtenerBalance(raiz->izq) < 0) {
        raiz->izq = rotacionIzquierda(raiz->izq);
        return rotacionDerecha(raiz);
    }
    if(balance < -1 && obtenerBalance(raiz->der) <= 0) return rotacionIzquierda(raiz);
    if(balance < -1 && obtenerBalance(raiz->der) > 0) {
        raiz->der = rotacionDerecha(raiz->der);
        return rotacionIzquierda(raiz);
    }

    return raiz;
}

Paciente eliminarPacienteMayorPrioridad(Nodo* &raiz) {
    Paciente atendido;
    if (raiz != NULL) {
        // eliminarRecursivo busca al de mayor prioridad, lo borra y guarda sus datos en "atendido"
        raiz = eliminarRecursivo(raiz, atendido); 
    }
    return atendido; // Devolvemos al paciente correcto al main
}

void mostrarArbol(Nodo* raiz, int tipoRecorrido) {
    if (raiz == NULL) return;

    // PREORDEN: Raíz - Izquierda - Derecha (tipoRecorrido == 0)
    if (tipoRecorrido == 0) {
        cout << "      [Llegada: " << raiz->dato.numLlegada << "] CI: " << raiz->dato.cedula 
             << " | " << raiz->dato.nombre << " (Nv: " << raiz->dato.urgencia << ")" << endl;
    }

    mostrarArbol(raiz->izq, tipoRecorrido);

    // INORDEN: Izquierda - Raíz - Derecha (tipoRecorrido == 1)
    if (tipoRecorrido == 1) {
        cout << "      [Llegada: " << raiz->dato.numLlegada << "] CI: " << raiz->dato.cedula 
             << " | " << raiz->dato.nombre << " (Nv: " << raiz->dato.urgencia << ")" << endl;
    }

    mostrarArbol(raiz->der, tipoRecorrido);

    // POSTORDEN: Izquierda - Derecha - Raíz (tipoRecorrido == 2)
    if (tipoRecorrido == 2) {
        cout << "      [Llegada: " << raiz->dato.numLlegada << "] CI: " << raiz->dato.cedula 
             << " | " << raiz->dato.nombre << " (Nv: " << raiz->dato.urgencia << ")" << endl;
    }
}
