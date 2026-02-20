#include "laboratorio.h"
#include "interfaz.h" // Necesario para gotoxy, color, limpiarAreaTrabajo
#include <iomanip>

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
    if (colaEstaVacia(c)) return false;

    NodoLaboratorio* actual = c.ultimo->siguiente;
    NodoLaboratorio* primero = actual;
    do {
        if (actual->datos.cedulaPaciente == cedula) return true;
        actual = actual->siguiente;
    } while (actual != primero); 

    return false; 
}

bool encolarSolicitud(ColaLaboratorio &c, Examen ex) {
    if (colaEstaLlena(c)) return false;
    if (validarColaLaboratorio(c, ex.cedulaPaciente)) return false;

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
    return true;
}

Examen desencolarSolicitud(ColaLaboratorio &c) {
    Examen vacio; 
    if (colaEstaVacia(c)) return vacio;

    NodoLaboratorio* nodoEliminar = c.ultimo->siguiente;
    Examen datosRetorno = nodoEliminar->datos;

    if (c.ultimo == nodoEliminar) {
        c.ultimo = NULL;
    } else {
        c.ultimo->siguiente = nodoEliminar->siguiente;
    }

    delete nodoEliminar; 
    c.cantidad--;
    return datosRetorno;
}

void eliminarLaboratorio(ColaLaboratorio &c, string cedula) {
    if (colaEstaVacia(c)) return;

    NodoLaboratorio* actual = c.ultimo->siguiente; 
    NodoLaboratorio* anterior = c.ultimo;          
    bool encontrado = false;

    do {
        if (actual->datos.cedulaPaciente == cedula) {
            encontrado = true;
            break; 
        }
        anterior = actual;
        actual = actual->siguiente;
    } while (actual != c.ultimo->siguiente); 

    if (!encontrado) return;

    if (c.cantidad == 1) {
        c.ultimo = NULL;
    } else {
        anterior->siguiente = actual->siguiente;
        if (actual == c.ultimo) {
            c.ultimo = anterior; 
        }
    }

    delete actual;
    c.cantidad--;
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
    nuevo->siguiente = p.tope;
    p.tope = nuevo;
    p.cantidad++;
}

void mostrarPila(PilaLaboratorio p) {
    int y = 5;
    gotoxy(4, y++); color(COLOR_TITULO); cout << "=== HISTORIAL DE RESULTADOS (PILA LIFO) ==="; color(COLOR_TEXTO);
    y++;

    if (pilaEstaVacia(p)) {
        gotoxy(4, y++); color(8); cout << "(La pila de resultados esta vacia)"; color(COLOR_TEXTO);
        return;
    }

    NodoLaboratorio* actual = p.tope;
    int contador = 0;

    while (actual != NULL) {
        if (y > 23) { // Paginación
            gotoxy(4, y+1); color(8); cout << "--- Presione ENTER para ver mas ---"; color(COLOR_TEXTO);
            _getch();
            limpiarAreaTrabajo(); y = 5;
            gotoxy(4, y++); color(COLOR_TITULO); cout << "=== HISTORIAL DE RESULTADOS (Continuacion) ==="; color(COLOR_TEXTO); y++;
        }

        gotoxy(4, y++); color(COLOR_PANEL); 
        cout << "[" << setfill('0') << setw(2) << actual->datos.fechaSolicitud.dia << "/" 
             << setfill('0') << setw(2) << actual->datos.fechaSolicitud.mes << "/" 
             << actual->datos.fechaSolicitud.anio << "] ";
        color(COLOR_EXITO); cout << actual->datos.nombrePaciente; color(COLOR_TEXTO);
        cout << " - " << obtenerNombreExamen(actual->datos.tipo);
        
        gotoxy(4, y++); cout << "   >> Resultado: " << actual->datos.resultado;
        gotoxy(4, y++); color(8); cout << "--------------------------------------------------------"; color(COLOR_TEXTO);
        
        actual = actual->siguiente;
        contador++;
    }
    gotoxy(4, y+1); cout << "Total procesados: " << contador;
}

// =========================================================
//  PROCESAMIENTO (Cola -> Pila) & ESTADISTICAS
// =========================================================

void procesarExamen(ColaLaboratorio &cola, PilaLaboratorio &pilaResultados, string resultadoMedico) {
    if (colaEstaVacia(cola)) return;

    Examen examen = desencolarSolicitud(cola);
    examen.resultado = resultadoMedico;
    examen.procesado = true;

    pushPila(pilaResultados, examen);
}

void mostrarEstadisticasLaboratorio(PilaLaboratorio p, int d, int m, int a, int fTipo) {
    int contHem = 0, contQui = 0, contImg = 0, contOri = 0, contHec = 0;
    int totalFiltrado = 0;

    NodoLaboratorio* actual = p.tope;
    
    // Recorrer la pila completa O(N)
    while (actual != NULL) {
        bool matchFecha = true;
        bool matchTipo = true;

        if (d != 0) {
            if (actual->datos.fechaSolicitud.dia != d || actual->datos.fechaSolicitud.mes != m || actual->datos.fechaSolicitud.anio != a) matchFecha = false;
        }
        if (fTipo != -1) {
            if ((int)actual->datos.tipo != fTipo) matchTipo = false;
        }

        if (matchFecha && matchTipo) {
            totalFiltrado++;
            switch(actual->datos.tipo) {
                case HEMATOLOGIA: contHem++; break;
                case QUIMICA: contQui++; break;
                case IMAGENOLOGIA: contImg++; break;
                case ORINA: contOri++; break;
                case HECES: contHec++; break;
            }
        }
        actual = actual->siguiente;
    }

    limpiarAreaTrabajo();
    int y = 5;
    gotoxy(5, y++); color(COLOR_TITULO); cout << "=== ESTADISTICAS DE LABORATORIO ==="; color(COLOR_TEXTO);
    y++;
    gotoxy(5, y++); color(8); cout << "Filtros aplicados:";
    gotoxy(5, y++); cout << "- Fecha: " << (d == 0 ? "Global (Todas)" : to_string(d) + "/" + to_string(m) + "/" + to_string(a));
    gotoxy(5, y++); cout << "- Tipo Examen: " << (fTipo == -1 ? "Todos" : obtenerNombreExamen((TipoExamen)fTipo));
    color(COLOR_TEXTO); y++;

    gotoxy(5, y++); color(COLOR_PANEL); cout << ">> EXAMENES PROCESADOS (En Pila):"; color(COLOR_TEXTO);
    gotoxy(5, y++); cout << "- Hematologia:   " << contHem;
    gotoxy(5, y++); cout << "- Quimica:       " << contQui;
    gotoxy(5, y++); cout << "- Imagenologia:  " << contImg;
    gotoxy(5, y++); cout << "- Uroanalisis:   " << contOri;
    gotoxy(5, y++); cout << "- Coproanalisis: " << contHec;
    y++;
    gotoxy(5, y++); cout << "TOTAL COINCIDENCIAS: " << totalFiltrado;
}