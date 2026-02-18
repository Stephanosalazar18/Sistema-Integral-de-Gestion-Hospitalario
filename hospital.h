#ifndef HOSPITAL_H
#define HOSPITAL_H

#include "estructuras.h"
#include <iostream>

using namespace std;

// Dimensiones del Hospital (Límites lógicos)
const int MAX_PISOS = 5;        // Pisos del 1 al 5
const int MAX_HABITACIONES = 10; // Habitaciones por piso (1 al 10)

// Nodo de la Matriz Dispersa (4 Direcciones)
struct NodoMatriz {
    // Datos de Hospitalización
    Paciente paciente;
    Fecha fechaIngreso;
    TipoHabitacion tipo;
    
    // Coordenadas
    int piso;       // Fila (1..5)
    int habitacion; // Columna (1..10)
    
    // Punteros de Navegación
    NodoMatriz* derecha;   // Siguiente habitación en el mismo piso
    NodoMatriz* izquierda; // Anterior habitación en el mismo piso
    NodoMatriz* abajo;     // Misma habitación en el piso de abajo
    NodoMatriz* arriba;    // Misma habitación en el piso de arriba
};

// Estructura de Control de la Matriz
struct Hospital {
    // Arreglos de punteros (Cabeceras)
    // filas[i] apunta al primer nodo del Piso i+1
    NodoMatriz* filas[MAX_PISOS]; 
    
    // columnas[j] apunta al primer nodo de la Habitación j+1 (en cualquier piso)
    NodoMatriz* columnas[MAX_HABITACIONES];
    
    int cantidadPacientes;
};

// --- FUNCIONES ---

void inicializarHospital(Hospital &h);

// Operaciones Principales
bool ingresarPaciente(Hospital &h, Paciente p, Fecha fecha, TipoHabitacion tipo, int piso, int numHab);
bool darAltaPaciente(Hospital &h, int piso, int numHab);

// Búsqueda y Validación
bool estaOcupada(Hospital h, int piso, int numHab);
NodoMatriz* buscarHabitacion(Hospital h, int piso, int numHab);

// Reportes
void mostrarHabitacionesOcupadas(Hospital h);
void mostrarHabitacionesLibres(Hospital h); // Calculado: Total - Ocupadas
void mostrarPacientesPorPiso(Hospital h, int piso);

#endif // HOSPITAL_H