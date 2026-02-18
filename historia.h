#ifndef HISTORIA_H
#define HISTORIA_H

#include "estructuras.h"
#include <string>

using namespace std;

// Tipos de eventos que pueden ocurrirle a un paciente
enum TipoEvento {
    CONSULTA_MEDICA,
    INGRESO_HOSPITALIZACION,
    EXAMEN_LABORATORIO,
    ALTA_MEDICA,
    OTRO_EVENTO
};

// Nodo de la Lista Doble (Ahora más robusto)
struct NodoHistoria {
    // --- DATOS COMUNES ---
    Fecha fechaEvento;
    TipoEvento tipo;
    string descripcion;   // Resumen general (ej: "Consulta por fiebre")
    string observaciones; // Notas adicionales

    // --- DATOS ESPECÍFICOS (Se llenan según el tipo) ---
    
    // Para CONSULTA / ALTA
    string diagnostico;
    string tratamiento;

    // Para HOSPITALIZACION
    string habitacionAsignada; // Ej: "Piso 1 - Hab 5"

    // Para LABORATORIO
    string tipoExamen;    // Ej: "Hematología"
    string resultadoLab;  // Ej: "Hemoglobina: 12.5"

    // --- PUNTEROS DE LISTA DOBLE ---
    NodoHistoria* anterior;
    NodoHistoria* siguiente;
};

// Cabecera del Expediente (Se mantiene igual, estructura lógica)
struct Expediente {
    Paciente datosPaciente;   // Copia de datos básicos para encabezado
    int numeroHistoria;       // ID único
    NodoHistoria* inicio;     // Cabeza
    NodoHistoria* fin;        // Cola (Insertar O(1))
};

// --- FUNCIONES DEL MÓDULO ---

// Gestión
Expediente* crearExpediente(Paciente p);

// FUNCIONES DE REGISTRO ESPECIALIZADAS (Para facilitar el uso desde main)
// 1. Registro básico de consulta (Lo que ya tenías)
void agregarConsulta(Expediente* exp, Fecha f, string diag, string trat, string obs);

// 2. Registro de Ingreso a Hospitalización
void agregarIngreso(Expediente* exp, Fecha f, string habitacion, string motivo);

// 3. Registro de Resultado de Laboratorio
void agregarResultadoLab(Expediente* exp, Fecha f, string tipoEx, string resultado);

// 4. Registro de Alta Médica
void agregarAlta(Expediente* exp, Fecha f, string resumen, string indicaciones);

// Visualización
void mostrarHistoriaClinica(Expediente* exp);

// Búsqueda
NodoHistoria* buscarRegistroPorFecha(NodoHistoria* actual, Fecha fBusqueda);

#endif // HISTORIA_H