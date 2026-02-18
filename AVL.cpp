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
    nodo->izq = NULL;  // Iniciar en NULL para evitar basura
    nodo->der = NULL;  // Iniciar en NULL
    nodo->altura = 1;
    return nodo;
}

// Rotaciones del diavl

Nodo* rotacionDerecha(Nodo* y) {
    Nodo* x = y->izq;
    Nodo* T2 = x->der;

    // realizando rotaciones
    x->der = y;
    y->izq = T2;

    // actualizando alturas
    y->altura = mayor(obtenerAltura(y->izq), obtenerAltura(y->der)) + 1;
    x->altura = mayor(obtenerAltura(x->izq), obtenerAltura(x->der)) + 1;

    return x;
}

Nodo* rotacionIzquierda(Nodo* x) {
    Nodo* y = x->der;
    Nodo* T2 = y->izq;

  // Realizando rotaciones
    y->izq = x;
    x->der = T2;

  // Actualizando alturas
    x->altura = mayor(obtenerAltura(x->izq), obtenerAltura(x->der)) + 1;
    y->altura = mayor(obtenerAltura(y->izq), obtenerAltura(y->der)) + 1;

    return y;
}



Nodo* insertarRecursivo(Nodo* nodo, Paciente p) {

     // 1. insercion normal de BST
    if (nodo == NULL)
        return nuevoNodo(p);

    // Logica de prioridad
     // "Menor valor" = "Mayor Prioridad" (izquierda)
     // CRITICO (0) < LEVE (3), asi que los graves van a la izquierda


    bool esMasUrgente = false;

    if (p.urgencia < nodo->dato.urgencia) {
        esMasUrgente = true; // tiene urgencia mas grave (0 < 2)

    } 
    else if (p.urgencia == nodo->dato.urgencia) {

        // si son iguales, se decide por orden de llegada
        if (p.numLlegada < nodo->dato.numLlegada) {
            esMasUrgente = true;
        }

    }

  // Decidir direccion basado en la prioridad
    if (esMasUrgente) {
        nodo->izq = insertarRecursivo(nodo->izq, p);

    } else {
        nodo->der = insertarRecursivo(nodo->der, p);

    }

    // 2. actualizar altura de este nodo ancestro
    nodo->altura = 1 + mayor(obtenerAltura(nodo->izq), obtenerAltura(nodo->der));

    // 3. Obtener balance para saber si hubo desbalanceo
    int balance = obtenerBalance(nodo);

    //4. Si el nodo se desbalanceo, hay 4 casos
    

    //Caso Izquierda Izquierda, El nuevo nodo es mas urgente y se metio en el subarbol izquierdo del izquierdo
    // comparamos los datos del paciente con el hijo izquierdo para saber donde cayo
    if (balance > 1 && (p.urgencia < nodo->izq->dato.urgencia || 
       (p.urgencia == nodo->izq->dato.urgencia && p.numLlegada < nodo->izq->dato.numLlegada))) {
        return rotacionDerecha(nodo);
    }

    //Caso Derecha Derecha, el nuevo nodo es menos urgente y se fue a la derecha de la derecha
    if (balance < -1 && (p.urgencia > nodo->der->dato.urgencia || 
       (p.urgencia == nodo->der->dato.urgencia && p.numLlegada > nodo->der->dato.numLlegada))) {
        return rotacionIzquierda(nodo);
    }

    // Caso Izquierda Derecha, el nuevo nodo es mas urgente y se fue a la izquierda de la derecha
    if (balance > 1 && (p.urgencia > nodo->izq->dato.urgencia || 
       (p.urgencia == nodo->izq->dato.urgencia && p.numLlegada > nodo->izq->dato.numLlegada))) {
        nodo->izq = rotacionIzquierda(nodo->izq);
        return rotacionDerecha(nodo);
    }

    // Caso Derecha Izquierda, el nuevo nodo es menos urgente y se fue a la izquierda de la izquierda
    if (balance < -1 && (p.urgencia < nodo->der->dato.urgencia || 
       (p.urgencia == nodo->der->dato.urgencia && p.numLlegada < nodo->der->dato.numLlegada))) {
        nodo->der = rotacionDerecha(nodo->der);
        return rotacionIzquierda(nodo);
    }
    
    // Si no hubo desbalance, retornar el nodo tal cual
    return nodo;

    // si esta mier** se mete en rotaciones infinitas, 
    // APAGO OTTO
}


void insertarPaciente(Nodo* &raiz, Paciente p) {
    raiz = insertarRecursivo(raiz, p);
    cout << "Insertado " << p.nombre << " (Prioridad: " << p.urgencia << ")" << endl;
}


// Funcion recursiva para eliminar
Nodo* eliminarRecursivo(Nodo* raiz, Paciente& paciente) {
  if(raiz == NULL) return raiz;

  if(raiz->izq != NULL) {
    raiz->izq = eliminarRecursivo(raiz->izq, paciente);
  } else {
    // encontramos al candidato, la raiz actual no tiene izquierda
    // Guardamos los datos para devolverlos
    paciente = raiz->dato;

    // Caso 1, sin hijos o solo hijo derecho
    // no puede tener hijo izq porque ya verificamos arriba esa vuelta 
    Nodo* aux = raiz->der;
    delete raiz;
    return aux;
  }

  // actualizamos altura
  raiz->altura = 1 + mayor(obtenerAltura(raiz->izq), obtenerAltura(raiz->der));

  // obtenemos el balance
  int balance = obtenerBalance(raiz);

  // Casos de rotacion(Rebalanceo al eliminar)
  // Izquierda Izquierda
  if(balance > 1 && obtenerBalance(raiz->izq) >= 0) {
    return rotacionDerecha(raiz);
  }

  // Izquierda Derecha
  if (balance > 1 && obtenerBalance(raiz->izq) < 0) {
    raiz->izq = rotacionIzquierda(raiz->izq);
    return rotacionDerecha(raiz);
  }

  // Derecha Derecha
  if(balance < -1 && obtenerBalance(raiz->der) <= 0) {
    return rotacionIzquierda(raiz);
  }

  // Derecha Izquierda
  if(balance < -1 && obtenerBalance(raiz->der) > 0) {
    raiz->der = rotacionDerecha(raiz->der);
    return rotacionIzquierda(raiz);
  }

  return raiz;
}



void eliminarPacienteMayorPrioridad(Nodo* &raiz) {
  if (raiz == NULL) {
    cout << "No hay nodos a eliminar. " << endl;
    return;
  };

  Paciente atendido;
  raiz = eliminarRecursivo(raiz, atendido);

  cout << "--- ATENDIENDO PACIENTE ---" << endl;
  cout << "Nombre: " << atendido.nombre << endl;
  cout << "Condicion: " << obtenerNombreUrgencia(atendido.urgencia) << endl;
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

// Visualización del árbol
// void mostrarArbol(Nodo* raiz, int tipoRecorrido) {
//     if (raiz != NULL) {
//         // 0: Preorden, 1: Inorden, 2: Postorden
        
//         if(tipoRecorrido == 0) { // Preorden (R-I-D)
//             cout << "[" << obtenerNombreUrgencia(raiz->dato.urgencia) << "] " 
//                  << raiz->dato.nombre << " (Llegada: " << raiz->dato.numLlegada << ")" << endl;
//         }
        
//         mostrarArbol(raiz->izq, tipoRecorrido);
        
//         if(tipoRecorrido == 1) { // Inorden (I-R-D) -> Mostrará ordenado por prioridad
//              cout << "[" << obtenerNombreUrgencia(raiz->dato.urgencia) << "] " 
//                  << raiz->dato.nombre << " (Llegada: " << raiz->dato.numLlegada << ")" << endl;
//         }
        
//         mostrarArbol(raiz->der, tipoRecorrido);
        
//         if(tipoRecorrido == 2) { // Postorden (I-D-R)
//              cout << "[" << obtenerNombreUrgencia(raiz->dato.urgencia) << "] " 
//                  << raiz->dato.nombre << " (Llegada: " << raiz->dato.numLlegada << ")" << endl;
//         }
//     }
// }