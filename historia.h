#ifndef HISTORIA_H
#define HISTORIA_H

#include "estructuras.h"
#include <string>

using namespace std;

// Nodo de la Lista Doble: Representa una visita o consulta médica
struct NodoHistoria {
    // Datos clínicos 
    Fecha fechaConsulta;
    string diagnostico;
    string tratamiento;
    string observaciones;
    
    // Punteros dobles
    NodoHistoria* anterior;
    NodoHistoria* siguiente;
};

// Cabecera del Expediente: Agrupa al paciente con sus registros
struct Expediente {
    Paciente datosPaciente;   // Copia de datos básicos
    int numeroHistoria;       // ID único del expediente
    NodoHistoria* inicio;     // Primer registro (Cabeza)
    NodoHistoria* fin;        // Último registro (Cola) - Para insertar rápido al final
};

// --- FUNCIONES DEL MÓDULO ---

// Gestión Básica
Expediente* crearExpediente(Paciente p);
void agregarRegistroMedico(Expediente* exp, Fecha f, string diag, string trat, string obs);

// Visualización
void mostrarHistoriaClinica(Expediente* exp);

// Búsqueda Recursiva 
NodoHistoria* buscarRegistroPorFecha(NodoHistoria* actual, Fecha fBusqueda);

#endif // HISTORIA_H