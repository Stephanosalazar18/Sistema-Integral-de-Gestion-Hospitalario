// Módulo 2 (Lista Doble).

#include "historia.h"
#include <iostream>

using namespace std;

// Crear un expediente nuevo (lista vacía)
Expediente* crearExpediente(Paciente p) {
    Expediente* nuevo = new Expediente();
    nuevo->datosPaciente = p;
    // Puedes generar un ID aleatorio o secuencial aquí
    nuevo->numeroHistoria = rand() % 1000 + 1; 
    nuevo->inicio = NULL;
    nuevo->fin = NULL;
    return nuevo;
}

// Insertar al final (Lista Doble)
void agregarRegistroMedico(Expediente* exp, Fecha f, string diag, string trat, string obs) {
    // 1. Crear el nuevo nodo
    NodoHistoria* nuevo = new NodoHistoria();
    nuevo->fechaConsulta = f;
    nuevo->diagnostico = diag;
    nuevo->tratamiento = trat;
    nuevo->observaciones = obs;
    nuevo->siguiente = NULL; // Al ser el último, siguiente es NULL
    
    // 2. Enlazar en la lista
    if (exp->inicio == NULL) {
        // Caso: Lista vacía
        nuevo->anterior = NULL;
        exp->inicio = nuevo;
        exp->fin = nuevo;
    } else {
        // Caso: Insertar al final
        nuevo->anterior = exp->fin;  // El anterior del nuevo es el actual fin
        exp->fin->siguiente = nuevo; // El siguiente del actual fin es el nuevo
        exp->fin = nuevo;            // Movemos el puntero fin
    }
    
    cout << "Registro agregado a la historia de " << exp->datosPaciente.nombre << endl;
}

// Recorrido Iterativo (Mostrar todo)
void mostrarHistoriaClinica(Expediente* exp) {
    cout << "\n=== HISTORIA CLINICA: " << exp->datosPaciente.nombre << " ===" << endl;
    cout << "Cedula: " << exp->datosPaciente.cedula << endl;
    
    if (exp->inicio == NULL) {
        cout << "   (Sin registros medicos)" << endl;
        return;
    }
    
    NodoHistoria* actual = exp->inicio;
    int contador = 1;
    
    while (actual != NULL) {
        cout << "\n   --- Consulta #" << contador << " ---" << endl;
        cout << "   Fecha: " << actual->fechaConsulta.dia << "/" 
             << actual->fechaConsulta.mes << "/" << actual->fechaConsulta.anio << endl;
        cout << "   Diagnostico: " << actual->diagnostico << endl;
        cout << "   Tratamiento: " << actual->tratamiento << endl;
        
        actual = actual->siguiente; // Avanzar
        contador++;
    }
    cout << "======================================\n" << endl;
}

// Búsqueda Recursiva 
NodoHistoria* buscarRegistroPorFecha(NodoHistoria* actual, Fecha fBusqueda) {
    // Caso Base 1: Llegamos al final y no encontramos nada
    if (actual == NULL) {
        return NULL;
    }
    
    // Caso Base 2: Encontramos la fecha (coincidencia exacta)
    if (actual->fechaConsulta.dia == fBusqueda.dia &&
        actual->fechaConsulta.mes == fBusqueda.mes &&
        actual->fechaConsulta.anio == fBusqueda.anio) {
        return actual;
    }
    
    // Paso Recursivo: Buscar en el siguiente nodo
    return buscarRegistroPorFecha(actual->siguiente, fBusqueda);
}