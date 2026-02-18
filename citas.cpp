#include "citas.h"
#include <iostream>
#include <iomanip>
#include <ctime>
#include <sstream>

using namespace std;

// --- UTILIDADES (Tu código) ---

string obtenerHoraServidor() {
    time_t ahora = time(0);
    tm *lctm = localtime(&ahora);

    stringstream ss;
    ss << setfill('0') << setw(2) << lctm->tm_hour << ":"
       << setfill('0') << setw(2) << lctm->tm_min << ":"
       << setfill('0') << setw(2) << lctm->tm_sec;
    
    return ss.str();
}

void inicializarTabla(TablaHash &t) {
    for (int i = 0; i < TAM_TABLA; i++) {
        t.buckets[i] = NULL;
    }
    t.cantidadCitas = 0;
}

// --- HASHING AVANZADO (Fusión) ---

// Tu algoritmo DJB2 (Optimizado)
int funcionHashDJB2(string clave) {
    unsigned long hash = 5381;
    for (char c : clave) {
        hash = ((hash << 5) + hash) + (unsigned char)c; // hash * 33 + c
    }
    return static_cast<int>(hash % TAM_TABLA);
}

// Generador de clave única según PDF (Medico + Fecha + Hora)
int generarIndice(string medico, Fecha f, Hora h) {
    // Convertimos los datos complejos a una sola cadena para hashear
    stringstream ss;
    ss << medico << f.dia << f.mes << f.anio << h.hora << h.minuto;
    string claveUnica = ss.str();
    
    return funcionHashDJB2(claveUnica);
}

// --- VALIDACIONES ---

bool existeChoqueHorario(TablaHash t, string medico, Fecha f, Hora h) {
    int indice = generarIndice(medico, f, h);
    NodoCita* actual = t.buckets[indice];
    
    while (actual != NULL) {
        if (actual->datos.nombreMedico == medico &&
            actual->datos.fecha.dia == f.dia &&
            actual->datos.hora.hora == h.hora) {
            return true;
        }
        actual = actual->siguiente;
    }
    return false;
}

// --- OPERACIONES PRINCIPALES ---

bool registrarCita(TablaHash &t, Cita nuevaCita) {
    // 1. Validar disponibilidad
    if (existeChoqueHorario(t, nuevaCita.nombreMedico, nuevaCita.fecha, nuevaCita.hora)) {
        cout << "[ERROR] Choque de horario: El Dr. " << nuevaCita.nombreMedico 
             << " ya tiene cita a esa hora." << endl;
        return false;
    }

    // 2. Calcular índice con DJB2
    int indice = generarIndice(nuevaCita.nombreMedico, nuevaCita.fecha, nuevaCita.hora);
    
    // 3. Crear nodo (Incluyendo tu timestamp)
    NodoCita* nuevo = new NodoCita();
    nuevo->datos = nuevaCita;
    nuevo->datos.fechaRegistro = obtenerHoraServidor(); // Tu funcionalidad agregada
    
    // 4. Inserción al inicio (Tu lógica eficiente O(1))
    nuevo->siguiente = t.buckets[indice];
    t.buckets[indice] = nuevo;
    
    t.cantidadCitas++;
    cout << " [OK] Cita registrada (Index: " << indice << ") | Audit: " << nuevo->datos.fechaRegistro << endl;
    return true;
}

// Tu lógica de eliminación aplicada al struct Cita
bool cancelarCita(TablaHash &t, string idPaciente, Fecha f, Hora h) {
    // Nota: Como la clave Hash depende del Médico, aquí tenemos que buscar 
    // recorriendo toda la tabla si no nos dan el médico. 
    // Asumiremos búsqueda lineal completa para ser seguros.

    for (int i = 0; i < TAM_TABLA; i++) {
        NodoCita* actual = t.buckets[i];
        NodoCita* anterior = NULL;
        
        while (actual != NULL) {
            // Buscamos coincidencia
            if (actual->datos.idPaciente == idPaciente && 
                actual->datos.fecha.dia == f.dia &&
                actual->datos.hora.hora == h.hora) {
                
                // ENCONTRADO - Eliminación
                if (anterior == NULL) {
                    // Era la cabeza del bucket
                    t.buckets[i] = actual->siguiente;
                } else {
                    // Estaba en medio o final
                    anterior->siguiente = actual->siguiente;
                }
                
                delete actual;
                t.cantidadCitas--;
                cout << " [OK] Cita eliminada correctamente." << endl;
                return true;
            }
            
            anterior = actual;
            actual = actual->siguiente;
        }
    }
    cout << " [!] No se encontro la cita para cancelar." << endl;
    return false;
}

// Tu visualización de tabla (Adaptada a Cita)
void mostrarTablaHash(TablaHash t) {
    cout << "\n\t===== VISUALIZACION DE MEMORIA (HASH TABLE) =====" << endl;
    cout << "\t+--------+--------------------------------------+" << endl;
    
    for (int i = 0; i < TAM_TABLA; i++) {
        NodoCita* actual = t.buckets[i];
        
        // Solo mostramos buckets ocupados para no llenar la pantalla de vacíos
        if (actual != NULL) {
            cout << "\t| [" << setw(2) << i << "] -> | ";
            while (actual != NULL) {
                cout << "[" << actual->datos.fecha.dia << "/" << actual->datos.fecha.mes 
                     << " " << actual->datos.hora.hora << ":00"
                     << " Dr." << actual->datos.nombreMedico << "] -> ";
                actual = actual->siguiente;
            }
            cout << "NULL" << endl;
        }
    }
    cout << "\t+--------+--------------------------------------+" << endl;
    cout << "\t Total de Citas Activas: " << t.cantidadCitas << endl;
    cout << "\n";
}