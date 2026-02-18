#include "laboratorio.h"

// Helper para convertir Enum a Texto (útil para imprimir)
string obtenerNombreExamen(TipoExamen tipo) {
    switch(tipo) {
        case HEMATOLOGIA: return "HEMATOLOGIA";
        case QUIMICA: return "QUIMICA";
        case IMAGENOLOGIA: return "IMAGENOLOGIA";
        case ORINA: return "UROANALISIS";
        case HECES: return "COPROANALISIS";
        default: return "OTRO";
    }
}

// =========================================================
//  COLA CIRCULAR
// =========================================================

void inicializarCola(ColaLaboratorio &c) {
    c.ultimo = NULL;
    c.cantidad = 0;
}

bool colaEstaVacia(ColaLaboratorio c) {
    return (c.ultimo == NULL);
}

bool colaEstaLlena(ColaLaboratorio c) {
    return (c.cantidad >= LIMITE_LABORATORIO);
}

bool validarColaLaboratorio(ColaLaboratorio c, string cedula) {
    
    if (colaEstaVacia(c)) {
        return false;
    }

    
    NodoLaboratorio* actual = c.ultimo->siguiente;
    NodoLaboratorio* primero = actual;

    do {
        if (actual->datos.cedulaPaciente == cedula) {
            return true; // ¡Encontrado!
        }
        
        actual = actual->siguiente;

    } while (actual != primero); // Repetimos hasta volver al inicio

    return false; // Dio la vuelta completa y no lo encontró
}

bool encolarSolicitud(ColaLaboratorio &c, Examen ex) {
    if (colaEstaLlena(c)) {
        cout << "[ERROR] Cola llena." << endl;
        return false;
    }

    // --- NUEVA VALIDACIÓN AGREGADA ---
    if (validarColaLaboratorio(c, ex.cedulaPaciente)) {
        cout << "[ERROR] El paciente " << ex.cedulaPaciente << " YA esta en la cola de espera." << endl;
        return false;
    }
    // ---------------------------------

    NodoLaboratorio* nuevo = new NodoLaboratorio();
    nuevo->datos = ex;

    if (colaEstaVacia(c)) {
        nuevo->siguiente = nuevo;
        c.ultimo = nuevo;
    } else {
        nuevo->siguiente = c.ultimo->siguiente;
        c.ultimo->siguiente = nuevo;
        c.ultimo = nuevo;
    }

    c.cantidad++;
    cout << "Solicitud registrada correctamente." << endl;
    return true;
}

// Sacar de la Cola Circular (Desencolar)
Examen desencolarSolicitud(ColaLaboratorio &c) {
    Examen vacio; // Retorno dummy
    if (colaEstaVacia(c)) return vacio;

    // El nodo a sacar es el "primero" (ultimo->siguiente)
    NodoLaboratorio* nodoEliminar = c.ultimo->siguiente;
    Examen datosRetorno = nodoEliminar->datos;

    if (c.ultimo == nodoEliminar) {
        // CASO: Solo quedaba un elemento
        c.ultimo = NULL;
    } else {
        // CASO: Hay más elementos
        // El último se salta al que vamos a eliminar y apunta al siguiente
        c.ultimo->siguiente = nodoEliminar->siguiente;
    }

    delete nodoEliminar; // Liberamos memoria
    c.cantidad--;
    return datosRetorno;
}

void eliminarLaboratorio(ColaLaboratorio &c, string cedula) {
    // 1. Validación inicial
    if (colaEstaVacia(c)) {
        cout << "[!] La cola esta vacia, no se puede eliminar." << endl;
        return;
    }

    NodoLaboratorio* actual = c.ultimo->siguiente; // Empezamos por el "primero" lógico
    NodoLaboratorio* anterior = c.ultimo;          // El anterior al primero es el último
    bool encontrado = false;

    // 2. Buscamos el nodo recorriendo el círculo
    do {
        if (actual->datos.cedulaPaciente == cedula) {
            encontrado = true;
            break; // Dejamos de recorrer, 'actual' apunta al que queremos borrar
        }
        
        // Avanzamos punteros
        anterior = actual;
        actual = actual->siguiente;

    } while (actual != c.ultimo->siguiente); // Repetir hasta volver al inicio

    // 3. Si no se encontró
    if (!encontrado) {
        cout << "[!] Paciente con cedula " << cedula << " no encontrado en la cola." << endl;
        return;
    }

    // 4. Lógica de eliminación
    if (c.cantidad == 1) {
        // CASO A: Era el único nodo
        c.ultimo = NULL;
    } 
    else {
        // CASO B: Hay más nodos
        
        // Paso clave: Saltamos el nodo actual
        anterior->siguiente = actual->siguiente;

        // CASO ESPECIAL: Si estamos borrando el que era marcado como 'ultimo'
        if (actual == c.ultimo) {
            c.ultimo = anterior; // El 'ultimo' ahora es el anterior
        }
    }

    // 5. Liberar memoria
    delete actual;
    c.cantidad--;
    cout << "[OK] Solicitud del paciente " << cedula << " eliminada exitosamente." << endl;
}

// =========================================================
//  PILA (LIFO) - Historial de Resultados
// =========================================================

void inicializarPila(PilaLaboratorio &p) {
    p.tope = NULL;
    p.cantidad = 0;
}

bool pilaEstaVacia(PilaLaboratorio p) {
    return (p.tope == NULL);
}

void pushPila(PilaLaboratorio &p, Examen ex) {
    NodoLaboratorio* nuevo = new NodoLaboratorio();
    nuevo->datos = ex;

    // Pila estándar: Nuevo apunta al tope actual
    nuevo->siguiente = p.tope;
    // Tope se mueve al nuevo
    p.tope = nuevo;

    p.cantidad++;
}

void mostrarPila(PilaLaboratorio p) {
    cout << "\n=== HISTORIAL DE RESULTADOS (PILA) ===" << endl;
    if (pilaEstaVacia(p)) {
        cout << "  (Vacia)" << endl;
    } else {
        NodoLaboratorio* actual = p.tope;
        while (actual != NULL) {
            cout << " [" << actual->datos.fechaSolicitud.dia << "/" << actual->datos.fechaSolicitud.mes << "] "
                 << actual->datos.nombrePaciente << " - " 
                 << obtenerNombreExamen(actual->datos.tipo) << endl;
            cout << "  >> Resultado: " << actual->datos.resultado << endl;
            cout << "---------------------------------------" << endl;
            actual = actual->siguiente;
        }
    }
    cout << "======================================\n" << endl;
}

// =========================================================
//  PROCESAMIENTO (Conexión Cola -> Pila)
// =========================================================

void procesarExamen(ColaLaboratorio &cola, PilaLaboratorio &pilaResultados, string resultadoMedico) {
    if (colaEstaVacia(cola)) {
        cout << "[!] No hay solicitudes pendientes." << endl;
        return;
    }

    // 1. Sacamos al paciente más antiguo (FIFO) de la cola
    Examen examen = desencolarSolicitud(cola);

    // 2. "Procesamos" (Agregamos el resultado)
    examen.resultado = resultadoMedico;
    examen.procesado = true;

    // 3. Guardamos en el historial (Pila)
    pushPila(pilaResultados, examen);

    cout << "\n>>> Examen PROCESADO con exito." << endl;
    cout << "Paciente: " << examen.nombrePaciente << endl;
    cout << "Resultado guardado en historial." << endl;
}