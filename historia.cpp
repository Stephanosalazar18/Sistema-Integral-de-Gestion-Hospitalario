#include "historia.h"
#include <iostream>

using namespace std;

// Función Privada Auxiliar (No está en el .h) para insertar al final
// Evita repetir la lógica de punteros 4 veces
void insertarNodoGenerico(Expediente* exp, NodoHistoria* nuevo) {
    if (exp->inicio == NULL) {
        nuevo->anterior = NULL;
        exp->inicio = nuevo;
        exp->fin = nuevo;
    } else {
        nuevo->anterior = exp->fin;
        exp->fin->siguiente = nuevo;
        exp->fin = nuevo;
    }
}

// Crear expediente
Expediente* crearExpediente(Paciente p) {
    Expediente* nuevo = new Expediente();
    nuevo->datosPaciente = p;
    nuevo->numeroHistoria = rand() % 10000 + 1; // ID aleatorio simple
    nuevo->inicio = NULL;
    nuevo->fin = NULL;
    return nuevo;
}

// --- FUNCIONES DE REGISTRO ---

void agregarConsulta(Expediente* exp, Fecha f, string diag, string trat, string obs) {
    NodoHistoria* nuevo = new NodoHistoria();
    
    // Datos Comunes
    nuevo->fechaEvento = f;
    nuevo->tipo = CONSULTA_MEDICA;
    nuevo->descripcion = "Consulta Externa / Triaje";
    nuevo->observaciones = obs;
    
    // Datos Específicos
    nuevo->diagnostico = diag;
    nuevo->tratamiento = trat;
    
    // Limpieza de campos no usados
    nuevo->siguiente = NULL;
    
    insertarNodoGenerico(exp, nuevo);
    cout << " [HISTORIA] Consulta registrada." << endl;
}

void agregarIngreso(Expediente* exp, Fecha f, string habitacion, string motivo) {
    NodoHistoria* nuevo = new NodoHistoria();
    
    nuevo->fechaEvento = f;
    nuevo->tipo = INGRESO_HOSPITALIZACION;
    nuevo->descripcion = "Ingreso a Hospitalizacion";
    nuevo->observaciones = motivo;
    
    nuevo->habitacionAsignada = habitacion;
    nuevo->siguiente = NULL;

    insertarNodoGenerico(exp, nuevo);
    cout << " [HISTORIA] Ingreso registrado." << endl;
}

void agregarResultadoLab(Expediente* exp, Fecha f, string tipoEx, string resultado) {
    NodoHistoria* nuevo = new NodoHistoria();
    
    nuevo->fechaEvento = f;
    nuevo->tipo = EXAMEN_LABORATORIO;
    nuevo->descripcion = "Resultado de Laboratorio";
    
    nuevo->tipoExamen = tipoEx;
    nuevo->resultadoLab = resultado;
    nuevo->siguiente = NULL;

    insertarNodoGenerico(exp, nuevo);
    cout << " [HISTORIA] Examen registrado." << endl;
}

void agregarAlta(Expediente* exp, Fecha f, string resumen, string indicaciones) {
    NodoHistoria* nuevo = new NodoHistoria();
    
    nuevo->fechaEvento = f;
    nuevo->tipo = ALTA_MEDICA;
    nuevo->descripcion = "Alta Medica / Egreso";
    
    nuevo->diagnostico = resumen; // Reusamos campo
    nuevo->tratamiento = indicaciones; // Reusamos campo
    nuevo->siguiente = NULL;

    insertarNodoGenerico(exp, nuevo);
    cout << " [HISTORIA] Alta registrada." << endl;
}

// --- VISUALIZACION ---

// Helper para convertir Enum a Texto
string tipoEventoStr(TipoEvento t) {
    switch(t) {
        case CONSULTA_MEDICA: return "CONSULTA";
        case INGRESO_HOSPITALIZACION: return "INGRESO";
        case EXAMEN_LABORATORIO: return "LABORATORIO";
        case ALTA_MEDICA: return "ALTA";
        default: return "OTRO";
    }
}

void mostrarHistoriaClinica(Expediente* exp) {
    cout << "\n=== HISTORIA CLINICA INTEGRAL: " << exp->datosPaciente.nombre << " ===" << endl;
    cout << "ID Expediente: " << exp->numeroHistoria << " | Cedula: " << exp->datosPaciente.cedula << endl;
    cout << "---------------------------------------------------------------" << endl;

    if (exp->inicio == NULL) {
        cout << "   (Sin registros aun)" << endl;
        return;
    }

    NodoHistoria* actual = exp->inicio;
    while (actual != NULL) {
        cout << "[" << actual->fechaEvento.dia << "/" << actual->fechaEvento.mes << "/" << actual->fechaEvento.anio << "] "
             << "TIPO: " << tipoEventoStr(actual->tipo) << endl;
        
        // Mostrar detalles según el tipo
        if (actual->tipo == CONSULTA_MEDICA) {
            cout << "   Diagnostico: " << actual->diagnostico << endl;
            cout << "   Tratamiento: " << actual->tratamiento << endl;
        }
        else if (actual->tipo == INGRESO_HOSPITALIZACION) {
            cout << "   Ubicacion: " << actual->habitacionAsignada << endl;
            cout << "   Motivo: " << actual->observaciones << endl;
        }
        else if (actual->tipo == EXAMEN_LABORATORIO) {
            cout << "   Examen: " << actual->tipoExamen << endl;
            cout << "   Resultado: " << actual->resultadoLab << endl;
        }
        else if (actual->tipo == ALTA_MEDICA) {
            cout << "   Resumen: " << actual->diagnostico << endl;
            cout << "   Indicaciones: " << actual->tratamiento << endl;
        }
        
        if (!actual->observaciones.empty() && actual->tipo != INGRESO_HOSPITALIZACION) {
            cout << "   Obs: " << actual->observaciones << endl;
        }
        
        cout << "---------------------------------------------------------------" << endl;
        actual = actual->siguiente;
    }
    cout << "\n";
}

// Búsqueda Recursiva (Se mantiene igual, solo compara fechas)
NodoHistoria* buscarRegistroPorFecha(NodoHistoria* actual, Fecha fBusqueda) {
    if (actual == NULL) return NULL;
    
    if (actual->fechaEvento.dia == fBusqueda.dia &&
        actual->fechaEvento.mes == fBusqueda.mes &&
        actual->fechaEvento.anio == fBusqueda.anio) {
        return actual;
    }
    
    return buscarRegistroPorFecha(actual->siguiente, fBusqueda);
}