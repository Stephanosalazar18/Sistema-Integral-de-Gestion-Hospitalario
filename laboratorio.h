#ifndef LABORATORIO_H
#define LABORATORIO_H

#include "estructuras.h" // Incluimos para usar Fecha, TipoExamen, etc.
#include <iostream>
#include <string>

using namespace std;

// Límite de la cola circular (Requerimiento PDF/Usuario)
const int LIMITE_LABORATORIO = 10; 

// Estructura que agrupa los datos de un examen
struct Examen {
    string cedulaPaciente;
    string nombrePaciente;
    TipoExamen tipo;       // Usamos el enum de estructuras.h
    Fecha fechaSolicitud;  // Usamos el struct de estructuras.h
    string resultado;      // Para cuando se procese
    bool procesado;
};

// Nodo Genérico (sirve para la Cola y para la Pila)
struct NodoLaboratorio {
    Examen datos;
    NodoLaboratorio* siguiente; // Puntero al siguiente nodo (o al primero en circular)
};

// Estructura de Cola Circular (Manejada solo con puntero al 'ultimo')
struct ColaLaboratorio {
    NodoLaboratorio* ultimo; // En una circular, ultimo->siguiente es el primero
    int cantidad;            // Para validar si está llena O(1)
};

// Estructura de Pila (LIFO)
struct PilaLaboratorio {
    NodoLaboratorio* tope;   // La cima de la pila
    int cantidad;
};

// --- FUNCIONES ---

// Inicialización
void inicializarCola(ColaLaboratorio &c);
void inicializarPila(PilaLaboratorio &p);

// Validaciones
bool colaEstaVacia(ColaLaboratorio c);
bool colaEstaLlena(ColaLaboratorio c);
bool pilaEstaVacia(PilaLaboratorio p);
bool validarColaLaboratorio(ColaLaboratorio c, string cedula);

// Operaciones de Cola Circular
// Nota: Pasamos 'Examen' completo para mantener el estándar
bool encolarSolicitud(ColaLaboratorio &c, Examen ex);
Examen desencolarSolicitud(ColaLaboratorio &c);

// Operaciones de Pila
void pushPila(PilaLaboratorio &p, Examen ex);
void mostrarPila(PilaLaboratorio p); // Muestra historial
void eliminarLaboratorio(ColaLaboratorio &c, string cedula);

// Lógica Principal
// Mueve de la Cola -> Pila y asigna resultado
void procesarExamen(ColaLaboratorio &cola, PilaLaboratorio &pilaResultados, string resultadoMedico);

// Auxiliar para imprimir tipo de examen
string obtenerNombreExamen(TipoExamen tipo);

#endif // LABORATORIO_H