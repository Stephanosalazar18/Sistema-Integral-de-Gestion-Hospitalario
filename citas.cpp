#include "citas.h"
#include <iostream>

using namespace std;

void inicializarTabla(TablaHash &t) {
    for (int i = 0; i < TAM_TABLA; i++) {
        t.tabla[i] = NULL; // Importante: Limpiar basura de memoria
    }
    t.cantidadCitas = 0;
}

// Función Hash basada en el PDF
// Sumamos valores ASCII del médico + valores de fecha/hora para crear un índice único
int generarIndiceHash(string medico, Fecha f, Hora h) {
    long suma = 0;
    
    // 1. Sumar ASCII del nombre del médico
    for (char c : medico) {
        suma += (int)c;
    }
    
    // 2. Sumar Fecha y Hora para diferenciar turnos
    suma += (f.dia * 100) + f.mes + f.anio;
    suma += h.hora + h.minuto;
    
    // 3. Módulo para que encaje en el tamaño de la tabla (0 a 96)
    return suma % TAM_TABLA;
}

bool existeChoqueHorario(TablaHash t, string medico, Fecha f, Hora h) {
    int indice = generarIndiceHash(medico, f, h);
    
    // Recorremos solo la lista de ese índice (Colisiones)
    NodoCita* actual = t.tabla[indice];
    while (actual != NULL) {
        if (actual->datos.nombreMedico == medico &&
            actual->datos.fecha.dia == f.dia &&
            actual->datos.hora.hora == h.hora) {
            return true; // Ya existe cita con ese médico a esa hora
        }
        actual = actual->siguiente;
    }
    return false;
}

bool registrarCita(TablaHash &t, Cita nuevaCita) {
    // 1. Validar choque de horario
    if (existeChoqueHorario(t, nuevaCita.nombreMedico, nuevaCita.fecha, nuevaCita.hora)) {
        cout << "[ERROR] Choque de horario: El Dr. " << nuevaCita.nombreMedico 
             << " ya esta ocupado a las " << nuevaCita.hora.hora << ":00." << endl;
        return false;
    }

    // 2. Obtener índice
    int indice = generarIndiceHash(nuevaCita.nombreMedico, nuevaCita.fecha, nuevaCita.hora);
    
    // 3. Crear Nodo
    NodoCita* nuevo = new NodoCita();
    nuevo->datos = nuevaCita;
    nuevo->siguiente = NULL;
    
    // 4. Insertar al principio de la lista (Manejo de colisión)
    if (t.tabla[indice] == NULL) {
        t.tabla[indice] = nuevo; // Estaba vacío
    } else {
        nuevo->siguiente = t.tabla[indice]; // Conectamos al antiguo primero
        t.tabla[indice] = nuevo;            // El nuevo pasa a ser el primero
    }
    
    t.cantidadCitas++;
    cout << "Cita registrada [Index Hash: " << indice << "]" << endl;
    return true;
}

void mostrarTodaLaAgenda(TablaHash t) {
    cout << "\n=== AGENDA MEDICA (TABLA HASH) ===" << endl;
    for (int i = 0; i < TAM_TABLA; i++) {
        if (t.tabla[i] != NULL) {
            NodoCita* actual = t.tabla[i];
            while (actual != NULL) {
                cout << "[Index " << i << "] " 
                     << actual->datos.fecha.dia << "/" << actual->datos.fecha.mes << " "
                     << actual->datos.hora.hora << ":00 - " 
                     << "Dr. " << actual->datos.nombreMedico 
                     << " (Pac: " << actual->datos.idPaciente << ")" << endl;
                actual = actual->siguiente;
            }
        }
    }
    cout << "==================================\n" << endl;
}

bool cancelarCita(TablaHash &t, string idPaciente, Fecha f, Hora h) {
    // Nota: Como es Hash, idealmente deberíamos buscar por Médico para ir directo al índice.
    // Si no tenemos el médico, debemos buscar en toda la tabla (Barrido completo).
    
    for (int i = 0; i < TAM_TABLA; i++) {
        NodoCita* actual = t.tabla[i];
        NodoCita* anterior = NULL;
        
        while (actual != NULL) {
            // Criterio de búsqueda: Paciente + Fecha + Hora
            if (actual->datos.idPaciente == idPaciente && 
                actual->datos.fecha.dia == f.dia &&
                actual->datos.hora.hora == h.hora) {
                
                // Encontrado: Eliminar nodo
                if (anterior == NULL) {
                    t.tabla[i] = actual->siguiente; // Era la cabeza
                } else {
                    anterior->siguiente = actual->siguiente; // Saltamos el nodo
                }
                
                delete actual;
                t.cantidadCitas--;
                cout << "Cita cancelada exitosamente." << endl;
                return true;
            }
            anterior = actual;
            actual = actual->siguiente;
        }
    }
    cout << "No se encontro la cita especificada." << endl;
    return false;
}