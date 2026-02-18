#ifndef CITAS_H
#define CITAS_H

#include "estructuras.h"
#include <string>

using namespace std;

// Tamaño primo para reducir colisiones (Mantenemos la constante)
const int TAM_TABLA = 97; 

struct Cita {
    // Datos del Proyecto (PDF)
    string idPaciente; 
    string nombreMedico;
    string especialidad;
    Fecha fecha;
    Hora hora;
    string motivo;
    
    // Tu aporte: Auditoría
    string fechaRegistro; // Hora real del sistema al crear la cita
    bool activa; 
};

struct NodoCita {
    Cita datos;
    NodoCita* siguiente;
};

struct TablaHash {
    // Usamos tu nomenclatura de 'buckets' pero estática para facilitar la gestión de memoria
    NodoCita* buckets[TAM_TABLA]; 
    int cantidadCitas;
};

// --- PROTOTIPOS ---

void inicializarTabla(TablaHash &t);
string obtenerHoraServidor(); // Tu función

// Tu algoritmo DJB2 adaptado a los datos de la Cita
int funcionHashDJB2(string clave); 
int generarIndice(string medico, Fecha f, Hora h);

// Operaciones
bool registrarCita(TablaHash &t, Cita nuevaCita);
bool cancelarCita(TablaHash &t, string idPaciente, Fecha f, Hora h);

// Tu visualización mejorada
void mostrarTablaHash(TablaHash t); 

// Validaciones
bool existeChoqueHorario(TablaHash t, string medico, Fecha f, Hora h);

#endif // CITAS_H