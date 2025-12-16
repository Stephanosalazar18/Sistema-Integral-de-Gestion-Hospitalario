#include "AVL.h"
#include <iostream>

using namespace std;

// --- FUNCIONES AUXILIARES ---

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
    nodo->izq = NULL;  // IMPORTANTE: Iniciar en NULL para evitar basura
    nodo->der = NULL;  // IMPORTANTE: Iniciar en NULL
    nodo->altura = 1;
    return nodo;
}

// --- ROTACIONES ---

Nodo* rotacionDerecha(Nodo* y) {
    Nodo* x = y->izq;
    Nodo* T2 = x->der;

    // Rotación
    x->der = y;
    y->izq = T2;

    // Actualizar alturas
    y->altura = mayor(obtenerAltura(y->izq), obtenerAltura(y->der)) + 1;
    x->altura = mayor(obtenerAltura(x->izq), obtenerAltura(x->der)) + 1;

    return x;
}

Nodo* rotacionIzquierda(Nodo* x) {
    Nodo* y = x->der;
    Nodo* T2 = y->izq;

    // Rotación
    y->izq = x;
    x->der = T2;

    // Actualizar alturas
    x->altura = mayor(obtenerAltura(x->izq), obtenerAltura(x->der)) + 1;
    y->altura = mayor(obtenerAltura(y->izq), obtenerAltura(y->der)) + 1;

    return y;
}

// --- LÓGICA DE INSERCIÓN ---

Nodo* insertarRecursivo(Nodo* nodo, Paciente p) {
    // 1. Inserción normal de BST
    if (nodo == NULL)
        return nuevoNodo(p);

    // LÓGICA DE PRIORIDAD: Menor número de urgencia = Mayor prioridad (Izquierda)
    bool esMasUrgente = false;

    if (p.urgencia < nodo->dato.urgencia) {
        esMasUrgente = true; 
    } 
    else if (p.urgencia == nodo->dato.urgencia) {
        // Empate: gana el que llegó primero (menor numLlegada)
        if (p.numLlegada < nodo->dato.numLlegada) {
            esMasUrgente = true;
        }
    }

    if (esMasUrgente) {
        nodo->izq = insertarRecursivo(nodo->izq, p);
    } else {
        nodo->der = insertarRecursivo(nodo->der, p);
    }

    // 2. Actualizar altura
    nodo->altura = 1 + mayor(obtenerAltura(nodo->izq), obtenerAltura(nodo->der));

    // 3. Obtener balance
    int balance = obtenerBalance(nodo);

    // 4. Casos de rotación para balancear
    
    // Caso Izquierda Izquierda
    if (balance > 1 && (p.urgencia < nodo->izq->dato.urgencia || 
       (p.urgencia == nodo->izq->dato.urgencia && p.numLlegada < nodo->izq->dato.numLlegada))) {
        return rotacionDerecha(nodo);
    }

    // Caso Derecha Derecha
    if (balance < -1 && (p.urgencia > nodo->der->dato.urgencia || 
       (p.urgencia == nodo->der->dato.urgencia && p.numLlegada > nodo->der->dato.numLlegada))) {
        return rotacionIzquierda(nodo);
    }

    // Caso Izquierda Derecha
    if (balance > 1 && (p.urgencia > nodo->izq->dato.urgencia || 
       (p.urgencia == nodo->izq->dato.urgencia && p.numLlegada > nodo->izq->dato.numLlegada))) {
        nodo->izq = rotacionIzquierda(nodo->izq);
        return rotacionDerecha(nodo);
    }

    // Caso Derecha Izquierda
    if (balance < -1 && (p.urgencia < nodo->der->dato.urgencia || 
       (p.urgencia == nodo->der->dato.urgencia && p.numLlegada < nodo->der->dato.numLlegada))) {
        nodo->der = rotacionDerecha(nodo->der);
        return rotacionIzquierda(nodo);
    }

    return nodo;
}

// --- FUNCIONES PÚBLICAS ---

void insertarPaciente(Nodo* &raiz, Paciente p) {
    raiz = insertarRecursivo(raiz, p);
    // Mensaje de confirmación para ver en consola
    cout << "Insertado " << p.nombre << " (Prioridad: " << p.urgencia << ")" << endl;
}

// --- LÓGICA DE EXTRACCIÓN ---

Nodo* eliminarRecursivo(Nodo* raiz, Paciente &pacienteAtendido) {
    if (raiz == NULL) return raiz;

    // Buscamos siempre a la izquierda (Mayor Prioridad)
    if (raiz->izq != NULL) {
        raiz->izq = eliminarRecursivo(raiz->izq, pacienteAtendido);
    } else {
        // Encontramos al nodo objetivo
        pacienteAtendido = raiz->dato;
        Nodo* temp = raiz->der;
        delete raiz;
        return temp; 
    }

    // Rebalanceo de regreso
    raiz->altura = 1 + mayor(obtenerAltura(raiz->izq), obtenerAltura(raiz->der));
    int balance = obtenerBalance(raiz);

    if (balance > 1 && obtenerBalance(raiz->izq) >= 0)
        return rotacionDerecha(raiz);

    if (balance > 1 && obtenerBalance(raiz->izq) < 0) {
        raiz->izq = rotacionIzquierda(raiz->izq);
        return rotacionDerecha(raiz);
    }

    if (balance < -1 && obtenerBalance(raiz->der) <= 0)
        return rotacionIzquierda(raiz);

    if (balance < -1 && obtenerBalance(raiz->der) > 0) {
        raiz->der = rotacionDerecha(raiz->der);
        return rotacionIzquierda(raiz);
    }

    return raiz;
}

void eliminarPacienteMayorPrioridad(Nodo* &raiz) {
    if (raiz == NULL) {
        cout << "No hay pacientes en espera." << endl;
        return;
    }
    Paciente atendido;
    raiz = eliminarRecursivo(raiz, atendido);
    
    cout << "--- ATENDIENDO PACIENTE ---" << endl;
    cout << "Nombre: " << atendido.nombre << endl;
    cout << "Urgencia: " << atendido.urgencia << endl;
    cout << "---------------------------" << endl;
}

void mostrarArbol(Nodo* raiz, int tipoRecorrido) {
    if (raiz != NULL) {
        // INORDEN (I-R-D)
        mostrarArbol(raiz->izq, tipoRecorrido);
        cout << "[" << raiz->dato.urgencia << "] " << raiz->dato.nombre << endl;
        mostrarArbol(raiz->der, tipoRecorrido);
    }
}