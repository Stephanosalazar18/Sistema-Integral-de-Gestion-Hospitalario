#include "laboratorio.h"
#include <iostream>

using namespace std;

// --- COLA CIRCULAR ---

void inicializarCola(ColaLaboratorio &c, int capacidad) {
    c.arreglo = new Examen[capacidad];
    c.capacidad = capacidad;
    c.frente = 0;
    c.final = -1;
    c.cantidad = 0;
}

bool colaEstaVacia(ColaLaboratorio c) {
    return (c.cantidad == 0);
}

bool colaEstaLlena(ColaLaboratorio c) {
    return (c.cantidad == c.capacidad);
}

// AQUÍ ESTABA EL POSIBLE ERROR DE FIRMA
bool encolarSolicitud(ColaLaboratorio &c, Examen ex) {
    if (colaEstaLlena(c)) {
        cout << "[ERROR] Cola llena." << endl;
        return false;
    }

    c.final = (c.final + 1) % c.capacidad;
    c.arreglo[c.final] = ex;
    c.cantidad++;
    
    cout << "Examen registrado para paciente " << ex.cedulaPaciente << " en posicion " << c.final << endl;
    return true;
}

Examen desencolarSolicitud(ColaLaboratorio &c) {
    Examen vacio; 
    if (colaEstaVacia(c)) return vacio; 

    Examen sacado = c.arreglo[c.frente];
    c.frente = (c.frente + 1) % c.capacidad;
    c.cantidad--;
    return sacado;
}

// --- PILA ---

void pushPila(NodoPila* &tope, Examen ex) {
    NodoPila* nuevo = new NodoPila();
    nuevo->datos = ex;
    nuevo->siguiente = tope;
    tope = nuevo;
}

void mostrarPila(NodoPila* tope) {
    cout << "\n=== HISTORIAL DE RESULTADOS (PILA LIFO) ===" << endl;
    if (tope == NULL) {
        cout << "No hay resultados." << endl;
        return;
    }
    NodoPila* actual = tope;
    while (actual != NULL) {
        cout << "Paciente: " << actual->datos.cedulaPaciente 
             << " | Resultado: " << actual->datos.resultado << endl;
        actual = actual->siguiente;
    }
    cout << "===========================================\n" << endl;
}

// --- PROCESAMIENTO ---

void procesarExamen(ColaLaboratorio &cola, NodoPila* &pilaResultados, string resultadoMedico) {
    if (colaEstaVacia(cola)) {
        cout << "No hay examenes pendientes." << endl;
        return;
    }

    Examen ex = desencolarSolicitud(cola);
    ex.procesado = true;
    ex.resultado = resultadoMedico;
    
    pushPila(pilaResultados, ex);
    
    cout << ">>> Procesado examen de " << ex.cedulaPaciente << endl;
}