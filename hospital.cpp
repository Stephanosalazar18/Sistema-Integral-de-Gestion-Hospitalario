// Módulo 4 (Matriz Dispersa).
#include "hospital.h"

void inicializarHospital(Hospital &h) {
    for (int i = 0; i < MAX_PISOS; i++) h.filas[i] = NULL;
    for (int j = 0; j < MAX_HABITACIONES; j++) h.columnas[j] = NULL;
    h.cantidadPacientes = 0;
}

// Función auxiliar para buscar (reutilizable)
NodoMatriz* buscarHabitacion(Hospital h, int piso, int numHab) {
    // Validar rangos
    if (piso < 1 || piso > MAX_PISOS || numHab < 1 || numHab > MAX_HABITACIONES) return NULL;

    // Recorremos la fila correspondiente (es más rápido buscar en la fila)
    NodoMatriz* actual = h.filas[piso - 1]; // Ajuste de índice (Piso 1 es indice 0)
    
    while (actual != NULL) {
        if (actual->habitacion == numHab) return actual;
        if (actual->habitacion > numHab) return NULL; // Como está ordenado, si nos pasamos, no está
        actual = actual->derecha;
    }
    return NULL;
}

bool estaOcupada(Hospital h, int piso, int numHab) {
    return buscarHabitacion(h, piso, numHab) != NULL;
}

bool ingresarPaciente(Hospital &h, Paciente p, Fecha fecha, TipoHabitacion tipo, int piso, int numHab) {
    // 1. Validaciones
    if (piso < 1 || piso > MAX_PISOS || numHab < 1 || numHab > MAX_HABITACIONES) {
        cout << "[ERROR] Habitacion o Piso invalido." << endl;
        return false;
    }
    if (estaOcupada(h, piso, numHab)) {
        cout << "[ERROR] La habitacion " << piso << "-" << numHab << " ya esta ocupada." << endl;
        return false;
    }

    // 2. Crear Nodo
    NodoMatriz* nuevo = new NodoMatriz();
    nuevo->paciente = p;
    nuevo->fechaIngreso = fecha;
    nuevo->tipo = tipo;
    nuevo->piso = piso;
    nuevo->habitacion = numHab;
    // Inicializar punteros
    nuevo->arriba = nuevo->abajo = nuevo->izquierda = nuevo->derecha = NULL;

    // 3. INSERCIÓN HORIZONTAL (EN LA FILA / PISO)
    int fIdx = piso - 1;
    if (h.filas[fIdx] == NULL || h.filas[fIdx]->habitacion > numHab) {
        // Caso: Insertar al inicio de la fila
        nuevo->derecha = h.filas[fIdx];
        if (h.filas[fIdx] != NULL) h.filas[fIdx]->izquierda = nuevo;
        h.filas[fIdx] = nuevo;
    } else {
        // Caso: Insertar en medio o final
        NodoMatriz* actual = h.filas[fIdx];
        while (actual->derecha != NULL && actual->derecha->habitacion < numHab) {
            actual = actual->derecha;
        }
        // Insertar después de 'actual'
        nuevo->derecha = actual->derecha;
        if (actual->derecha != NULL) actual->derecha->izquierda = nuevo;
        actual->derecha = nuevo;
        nuevo->izquierda = actual;
    }

    // 4. INSERCIÓN VERTICAL (EN LA COLUMNA / HABITACIÓN)
    int cIdx = numHab - 1;
    if (h.columnas[cIdx] == NULL || h.columnas[cIdx]->piso > piso) {
        // Caso: Insertar al inicio de la columna (arriba del todo)
        nuevo->abajo = h.columnas[cIdx];
        if (h.columnas[cIdx] != NULL) h.columnas[cIdx]->arriba = nuevo;
        h.columnas[cIdx] = nuevo;
    } else {
        // Caso: Insertar en medio o abajo
        NodoMatriz* actual = h.columnas[cIdx];
        while (actual->abajo != NULL && actual->abajo->piso < piso) {
            actual = actual->abajo;
        }
        // Insertar debajo de 'actual'
        nuevo->abajo = actual->abajo;
        if (actual->abajo != NULL) actual->abajo->arriba = nuevo;
        actual->abajo = nuevo;
        nuevo->arriba = actual;
    }

    h.cantidadPacientes++;
    cout << "Ingreso exitoso en habitacion " << piso << "-" << numHab << endl;
    return true;
}

bool darAltaPaciente(Hospital &h, int piso, int numHab) {
    NodoMatriz* objetivo = buscarHabitacion(h, piso, numHab);
    if (objetivo == NULL) {
        cout << "[ERROR] No hay paciente en esa habitacion." << endl;
        return false;
    }

    // 1. DESENLAZAR HORIZONTAL
    if (objetivo->izquierda != NULL) {
        objetivo->izquierda->derecha = objetivo->derecha;
    } else {
        // Era el primero de la fila
        h.filas[piso - 1] = objetivo->derecha;
    }
    if (objetivo->derecha != NULL) {
        objetivo->derecha->izquierda = objetivo->izquierda;
    }

    // 2. DESENLAZAR VERTICAL
    if (objetivo->arriba != NULL) {
        objetivo->arriba->abajo = objetivo->abajo;
    } else {
        // Era el primero de la columna
        h.columnas[numHab - 1] = objetivo->abajo;
    }
    if (objetivo->abajo != NULL) {
        objetivo->abajo->arriba = objetivo->arriba;
    }

    cout << "Paciente " << objetivo->paciente.nombre << " dado de alta. Habitacion liberada." << endl;
    delete objetivo;
    h.cantidadPacientes--;
    return true;
}

// REPORTES

void mostrarHabitacionesOcupadas(Hospital h) {
    cout << "\n=== HABITACIONES OCUPADAS (Matriz Dispersa) ===" << endl;
    // Recorremos usando las cabeceras de FILAS (Pisos) es lo más natural visualmente
    for (int i = 0; i < MAX_PISOS; i++) {
        NodoMatriz* actual = h.filas[i];
        if (actual != NULL) {
            cout << "PISO " << (i + 1) << ": ";
            while (actual != NULL) {
                cout << "[Hab " << actual->habitacion << ": " << actual->paciente.nombre << "] -> ";
                actual = actual->derecha;
            }
            cout << "FIN" << endl;
        }
    }
    cout << "===============================================\n" << endl;
}

void mostrarHabitacionesLibres(Hospital h) {
    cout << "\n=== HABITACIONES LIBRES ===" << endl;
    // Para listar libres, iteramos el mapa lógico completo y verificamos existencia
    for (int p = 1; p <= MAX_PISOS; p++) {
        cout << "Piso " << p << " libres: ";
        for (int habit = 1; habit <= MAX_HABITACIONES; habit++) {
            if (!estaOcupada(h, p, habit)) {
                cout << habit << " ";
            }
        }
        cout << endl;
    }
    cout << "===========================\n" << endl;
}