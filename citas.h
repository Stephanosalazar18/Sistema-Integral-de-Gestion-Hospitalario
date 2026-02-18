#ifndef CITAS_H
#define CITAS_H

#include "estructuras.h"
#include <string>

using namespace std;

// Definimos un tamaño primo para reducir colisiones
const int TAM_TABLA = 97; 

// Estructura de la Cita (Datos del PDF)
struct Cita {
    string idPaciente; // Cédula
    string nombreMedico;
    string especialidad;
    Fecha fecha;
    Hora hora;
    string motivo;
    bool activa; 
};

// Nodo para la lista enlazada (Manejo de Colisiones)
struct NodoCita {
    Cita datos;
    NodoCita* siguiente;
};

// La Tabla Hash es un arreglo de punteros (Cabeceras de listas)
struct TablaHash {
    NodoCita* tabla[TAM_TABLA]; // El arreglo principal
    int cantidadCitas;
};

// --- PROTOTIPOS DE FUNCIONES ---

void inicializarTabla(TablaHash &t);

// Función Hash (La fórmula matemática)
int generarIndiceHash(string medico, Fecha f, Hora h); 

// Operaciones
bool registrarCita(TablaHash &t, Cita nuevaCita);
bool cancelarCita(TablaHash &t, string idPaciente, Fecha f, Hora h);
void mostrarTodaLaAgenda(TablaHash t);

// Validación
bool existeChoqueHorario(TablaHash t, string medico, Fecha f, Hora h);

#endif // CITAS_H