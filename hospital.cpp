#include "hospital.h"
#include "interfaz.h" // Necesario para gotoxy, color, etc.
#include <iomanip>    // Para formatear la salida

void inicializarHospital(Hospital &h) {
    for (int i = 0; i < MAX_PISOS; i++) h.filas[i] = NULL;
    for (int j = 0; j < MAX_HABITACIONES; j++) h.columnas[j] = NULL;
    h.cantidadPacientes = 0;
}

NodoMatriz* buscarHabitacion(Hospital h, int piso, int numHab) {
    if (piso < 1 || piso > MAX_PISOS || numHab < 1 || numHab > MAX_HABITACIONES) return NULL;
    NodoMatriz* actual = h.filas[piso - 1]; 
    while (actual != NULL) {
        if (actual->habitacion == numHab) return actual;
        if (actual->habitacion > numHab) return NULL; 
        actual = actual->derecha;
    }
    return NULL;
}

bool estaOcupada(Hospital h, int piso, int numHab) {
    return buscarHabitacion(h, piso, numHab) != NULL;
}

bool ingresarPaciente(Hospital &h, Paciente p, Fecha fecha, TipoHabitacion tipo, int piso, int numHab) {
    if (piso < 1 || piso > MAX_PISOS || numHab < 1 || numHab > MAX_HABITACIONES) return false;
    if (estaOcupada(h, piso, numHab)) return false;

    NodoMatriz* nuevo = new NodoMatriz();
    nuevo->paciente = p;
    nuevo->fechaIngreso = fecha;
    nuevo->tipo = tipo;
    nuevo->piso = piso;
    nuevo->habitacion = numHab;
    nuevo->arriba = nuevo->abajo = nuevo->izquierda = nuevo->derecha = NULL;

    // INSERCIÓN HORIZONTAL
    int fIdx = piso - 1;
    if (h.filas[fIdx] == NULL || h.filas[fIdx]->habitacion > numHab) {
        nuevo->derecha = h.filas[fIdx];
        if (h.filas[fIdx] != NULL) h.filas[fIdx]->izquierda = nuevo;
        h.filas[fIdx] = nuevo;
    } else {
        NodoMatriz* actual = h.filas[fIdx];
        while (actual->derecha != NULL && actual->derecha->habitacion < numHab) {
            actual = actual->derecha;
        }
        nuevo->derecha = actual->derecha;
        if (actual->derecha != NULL) actual->derecha->izquierda = nuevo;
        actual->derecha = nuevo;
        nuevo->izquierda = actual;
    }

    // INSERCIÓN VERTICAL
    int cIdx = numHab - 1;
    if (h.columnas[cIdx] == NULL || h.columnas[cIdx]->piso > piso) {
        nuevo->abajo = h.columnas[cIdx];
        if (h.columnas[cIdx] != NULL) h.columnas[cIdx]->arriba = nuevo;
        h.columnas[cIdx] = nuevo;
    } else {
        NodoMatriz* actual = h.columnas[cIdx];
        while (actual->abajo != NULL && actual->abajo->piso < piso) {
            actual = actual->abajo;
        }
        nuevo->abajo = actual->abajo;
        if (actual->abajo != NULL) actual->abajo->arriba = nuevo;
        actual->abajo = nuevo;
        nuevo->arriba = actual;
    }

    h.cantidadPacientes++;
    return true;
}

bool darAltaPaciente(Hospital &h, int piso, int numHab) {
    NodoMatriz* objetivo = buscarHabitacion(h, piso, numHab);
    if (objetivo == NULL) return false;

    // DESENLAZAR HORIZONTAL
    if (objetivo->izquierda != NULL) objetivo->izquierda->derecha = objetivo->derecha;
    else h.filas[piso - 1] = objetivo->derecha;
    
    if (objetivo->derecha != NULL) objetivo->derecha->izquierda = objetivo->izquierda;

    // DESENLAZAR VERTICAL
    if (objetivo->arriba != NULL) objetivo->arriba->abajo = objetivo->abajo;
    else h.columnas[numHab - 1] = objetivo->abajo;
    
    if (objetivo->abajo != NULL) objetivo->abajo->arriba = objetivo->arriba;

    delete objetivo;
    h.cantidadPacientes--;
    return true;
}

void mostrarHabitacionesOcupadas(Hospital h) {
    int y = 5;
    gotoxy(4, y++); color(COLOR_TITULO); cout << "=== MAPA DE HABITACIONES OCUPADAS ==="; color(COLOR_TEXTO);
    y++;
    
    if (h.cantidadPacientes == 0) {
        gotoxy(4, y++); color(8); cout << "(El hospital esta completamente vacio)"; color(COLOR_TEXTO);
        return;
    }

    for (int i = 0; i < MAX_PISOS; i++) {
        NodoMatriz* actual = h.filas[i];
        if (actual != NULL) {
            gotoxy(4, y++); color(COLOR_PANEL); cout << "PISO " << (i + 1) << ":"; color(COLOR_TEXTO);
            while (actual != NULL) {
                gotoxy(13, y++); 
                cout << "-> [Hab " << actual->habitacion << "] " << actual->paciente.nombre 
                     << " (" << actual->fechaIngreso.dia << "/" << actual->fechaIngreso.mes << ")";
                actual = actual->derecha;
            }
            y++;
        }
    }
}