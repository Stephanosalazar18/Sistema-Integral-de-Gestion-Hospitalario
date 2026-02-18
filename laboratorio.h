#ifndef LABORATORIO_H
#define LABORATORIO_H

#include "estructuras.h"
#include <string>

using namespace std;

const int MAX_COLA = 5; 

struct Examen {
    string cedulaPaciente;
    TipoExamen tipo;
    Fecha fechaSolicitud;
    string resultado;
    bool procesado;
};

struct ColaLaboratorio {
    Examen* arreglo;
    int frente;
    int final;
    int cantidad;
    int capacidad;
};

struct NodoPila {
    Examen datos;
    NodoPila* siguiente;
};

// --- FUNCIONES ---
void inicializarCola(ColaLaboratorio &c, int capacidad);
void liberarCola(ColaLaboratorio &c);

// OJO AQUÍ: Debe coincidir con el .cpp
bool encolarSolicitud(ColaLaboratorio &c, Examen ex); 
Examen desencolarSolicitud(ColaLaboratorio &c);
bool colaEstaVacia(ColaLaboratorio c);
bool colaEstaLlena(ColaLaboratorio c);

void pushPila(NodoPila* &tope, Examen ex);
void mostrarPila(NodoPila* tope);

void procesarExamen(ColaLaboratorio &cola, NodoPila* &pilaResultados, string resultadoMedico);

#endif // LABORATORIO_H