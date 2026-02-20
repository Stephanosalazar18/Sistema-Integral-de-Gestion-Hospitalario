#include "citas.h"
#include "interfaz.h" 
#include <iostream>
#include <iomanip>
#include <ctime>
#include <sstream>

using namespace std;

// --- UTILIDADES ---

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

// --- HASHING AVANZADO ---

int funcionHashDJB2(string clave) {
    unsigned long hash = 5381;
    for (char c : clave) {
        hash = ((hash << 5) + hash) + (unsigned char)c; 
    }
    return static_cast<int>(hash % TAM_TABLA);
}

int generarIndice(string medico, Fecha f, Hora h) {
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
        if (actual->datos.activa && // Solo revisa citas activas
            actual->datos.nombreMedico == medico &&
            actual->datos.fecha.dia == f.dia &&
            actual->datos.fecha.mes == f.mes &&
            actual->datos.fecha.anio == f.anio &&
            actual->datos.hora.hora == h.hora) {
            return true; // El médico ya está ocupado a esa hora
        }
        actual = actual->siguiente;
    }
    return false;
}

// Nueva validación obligatoria del PDF: El paciente no puede estar en dos lugares a la vez
bool pacienteTieneCita(TablaHash t, string idPaciente, Fecha f, Hora h) {
    for (int i = 0; i < TAM_TABLA; i++) {
        NodoCita* actual = t.buckets[i];
        while (actual != NULL) {
            if (actual->datos.activa && 
                actual->datos.idPaciente == idPaciente &&
                actual->datos.fecha.dia == f.dia &&
                actual->datos.fecha.mes == f.mes &&
                actual->datos.fecha.anio == f.anio &&
                actual->datos.hora.hora == h.hora) {
                return true; // El paciente ya tiene otra cita a esa misma hora
            }
            actual = actual->siguiente;
        }
    }
    return false;
}

// --- OPERACIONES PRINCIPALES ---

bool registrarCita(TablaHash &t, Cita nuevaCita) {
    // Las validaciones de choque se harán en el main para dar mensajes específicos
    int indice = generarIndice(nuevaCita.nombreMedico, nuevaCita.fecha, nuevaCita.hora);
    
    NodoCita* nuevo = new NodoCita();
    nuevo->datos = nuevaCita;
    nuevo->datos.fechaRegistro = obtenerHoraServidor(); 
    
    nuevo->siguiente = t.buckets[indice];
    t.buckets[indice] = nuevo;
    
    t.cantidadCitas++; // Citas totales (históricas)
    return true;
}

bool cancelarCita(TablaHash &t, string idPaciente, Fecha f, Hora h) {
    for (int i = 0; i < TAM_TABLA; i++) {
        NodoCita* actual = t.buckets[i];
        
        while (actual != NULL) {
            // Buscamos que esté activa y que TODOS los datos coincidan a la perfección
            if (actual->datos.activa && 
                actual->datos.idPaciente == idPaciente && 
                actual->datos.fecha.dia == f.dia &&
                actual->datos.fecha.mes == f.mes &&   // <- Añadido
                actual->datos.fecha.anio == f.anio && // <- Añadido
                actual->datos.hora.hora == h.hora) {
                
                // BORRADO LÓGICO
                actual->datos.activa = false; 
                return true;
            }
            actual = actual->siguiente;
        }
    }
    return false;
}

// --- VISUALIZACIÓN ORDENADA ---

void mostrarTablaHash(TablaHash t) {
    int y = 5; 
    
    gotoxy(4, y++); color(COLOR_TITULO); 
    cout << "=== AGENDA MEDICA (HISTORIAL DE CITAS) ==="; color(COLOR_TEXTO);
    
    if (t.cantidadCitas == 0) {
        gotoxy(4, y+1); color(8); 
        cout << "(No hay citas en el sistema)"; color(COLOR_TEXTO);
        return;
    }

    NodoCita** arreglo = new NodoCita*[t.cantidadCitas];
    int idx = 0;
    int activasCount = 0;
    
    for (int i = 0; i < TAM_TABLA; i++) {
        NodoCita* actual = t.buckets[i];
        while (actual != NULL) {
            arreglo[idx++] = actual;
            if(actual->datos.activa) activasCount++;
            actual = actual->siguiente;
        }
    }

    // Ordenamiento Burbuja
    for (int i = 0; i < t.cantidadCitas - 1; i++) {
        for (int j = 0; j < t.cantidadCitas - i - 1; j++) {
            Cita a = arreglo[j]->datos;
            Cita b = arreglo[j+1]->datos;
            bool intercambiar = false;
            
            if (a.fecha.anio > b.fecha.anio) intercambiar = true;
            else if (a.fecha.anio == b.fecha.anio) {
                if (a.fecha.mes > b.fecha.mes) intercambiar = true;
                else if (a.fecha.mes == b.fecha.mes) {
                    if (a.fecha.dia > b.fecha.dia) intercambiar = true;
                    else if (a.fecha.dia == b.fecha.dia) {
                        if (a.hora.hora > b.hora.hora) intercambiar = true;
                    }
                }
            }
            
            if (intercambiar) {
                NodoCita* temp = arreglo[j];
                arreglo[j] = arreglo[j+1];
                arreglo[j+1] = temp;
            }
        }
    }

    int pagina = 1;
    int maxPorPagina = 12; 
    int totalPaginas = (t.cantidadCitas + maxPorPagina - 1) / maxPorPagina;

    for (int i = 0; i < t.cantidadCitas; i++) {
        if (i % maxPorPagina == 0) {
            if (i > 0) {
                gotoxy(4, y+2); color(8); cout << "--- Presione ENTER para ver pagina " << pagina + 1 << " de " << totalPaginas << " ---"; color(COLOR_TEXTO);
                _getch();
                limpiarAreaTrabajo(); y = 5;
                gotoxy(4, y++); color(COLOR_TITULO); 
                cout << "=== AGENDA MEDICA (Pag " << ++pagina << "/" << totalPaginas << ") ==="; color(COLOR_TEXTO);
            }
            
            gotoxy(4, y++); color(COLOR_PANEL);
            // Ajustamos las columnas para que quepa el ESTADO
            cout << "+------------+-------+------------------+----------------------+---------+";
            gotoxy(4, y++);
            cout << "| FECHA      | HORA  | ESPECIALISTA     | PACIENTE Y CEDULA    | ESTADO  |";
            gotoxy(4, y++);
            cout << "+------------+-------+------------------+----------------------+---------+";
            color(COLOR_TEXTO);
        }

        Cita c = arreglo[i]->datos;
        
        string med = c.nombreMedico;
        if (med.length() > 16) med = med.substr(0, 13) + "...";
        
        string pacInfo = c.nombrePaciente + " (" + c.idPaciente + ")";
        if (pacInfo.length() > 20) pacInfo = pacInfo.substr(0, 17) + "...";

        string estado = c.activa ? "ACTIVA" : "CANCEL";

        gotoxy(4, y++);
        cout << "| " 
             << setfill('0') << setw(2) << c.fecha.dia << "/" 
             << setfill('0') << setw(2) << c.fecha.mes << "/" 
             << c.fecha.anio << " | "
             << setfill('0') << setw(2) << c.hora.hora << ":00 | "
             << setfill(' ') << left << setw(16) << med << " | "
             << left << setw(20) << pacInfo << " | ";
             
        // Color visual para el estado
        if(c.activa) color(COLOR_EXITO); else color(COLOR_ALERTA);
        cout << left << setw(7) << estado; color(COLOR_TEXTO);
        cout << " |";
             
        cout << right; 
    }

    gotoxy(4, y++); color(COLOR_PANEL);
    cout << "+------------+-------+------------------+----------------------+---------+"; color(COLOR_TEXTO);
    
    gotoxy(4, y+1); cout << "Total Historial: " << t.cantidadCitas << " | Citas Activas: " << activasCount;

    delete[] arreglo; 
}

void consultarCitasFiltradas(TablaHash t, int tipoFiltro, string filtroTexto, Fecha filtroF) {
    int y = 5; 
    limpiarAreaTrabajo();
    gotoxy(4, y++); color(COLOR_TITULO); 
    cout << "=== RESULTADOS DE LA BUSQUEDA ==="; color(COLOR_TEXTO);

    NodoCita** arreglo = new NodoCita*[t.cantidadCitas];
    int idx = 0;
    
    // Extracción Condicional (Filtro)
    for (int i = 0; i < TAM_TABLA; i++) {
        NodoCita* actual = t.buckets[i];
        while (actual != NULL) {
            bool coincide = false;
            if (tipoFiltro == 1 && actual->datos.idPaciente == filtroTexto) coincide = true;
            else if (tipoFiltro == 2 && actual->datos.especialidad == filtroTexto) coincide = true;
            else if (tipoFiltro == 3 && actual->datos.fecha.dia == filtroF.dia && 
                     actual->datos.fecha.mes == filtroF.mes && actual->datos.fecha.anio == filtroF.anio) coincide = true;

            if (coincide) {
                arreglo[idx++] = actual;
            }
            actual = actual->siguiente;
        }
    }

    if (idx == 0) {
        gotoxy(4, y+1); color(COLOR_ALERTA); 
        cout << "No se encontraron citas con esos criterios."; color(COLOR_TEXTO);
        delete[] arreglo;
        return;
    }

    // Ordenamiento de Burbuja para los resultados
    for (int i = 0; i < idx - 1; i++) {
        for (int j = 0; j < idx - i - 1; j++) {
            Cita a = arreglo[j]->datos;
            Cita b = arreglo[j+1]->datos;
            bool intercambiar = false;
            
            if (a.fecha.anio > b.fecha.anio) intercambiar = true;
            else if (a.fecha.anio == b.fecha.anio) {
                if (a.fecha.mes > b.fecha.mes) intercambiar = true;
                else if (a.fecha.mes == b.fecha.mes) {
                    if (a.fecha.dia > b.fecha.dia) intercambiar = true;
                    else if (a.fecha.dia == b.fecha.dia) {
                        if (a.hora.hora > b.hora.hora) intercambiar = true;
                    }
                }
            }
            if (intercambiar) {
                NodoCita* temp = arreglo[j];
                arreglo[j] = arreglo[j+1];
                arreglo[j+1] = temp;
            }
        }
    }

    // Mostrar Tabla de Resultados
    gotoxy(4, y++); color(COLOR_PANEL);
    cout << "+------------+-------+------------------+----------------------+---------+";
    gotoxy(4, y++);
    cout << "| FECHA      | HORA  | ESPECIALISTA     | PACIENTE Y CEDULA    | ESTADO  |";
    gotoxy(4, y++);
    cout << "+------------+-------+------------------+----------------------+---------+";
    color(COLOR_TEXTO);

    for (int i = 0; i < idx; i++) {
        Cita c = arreglo[i]->datos;
        
        string med = c.nombreMedico;
        if (med.length() > 16) med = med.substr(0, 13) + "...";
        string pacInfo = c.nombrePaciente + " (" + c.idPaciente + ")";
        if (pacInfo.length() > 20) pacInfo = pacInfo.substr(0, 17) + "...";
        string estado = c.activa ? "ACTIVA" : "CANCEL";

        gotoxy(4, y++);
        cout << "| " << setfill('0') << setw(2) << c.fecha.dia << "/" << setfill('0') << setw(2) << c.fecha.mes << "/" << c.fecha.anio << " | "
             << setfill('0') << setw(2) << c.hora.hora << ":00 | " << setfill(' ') << left << setw(16) << med << " | "
             << left << setw(20) << pacInfo << " | ";
        if(c.activa) color(COLOR_EXITO); else color(COLOR_ALERTA);
        cout << left << setw(7) << estado; color(COLOR_TEXTO);
        cout << " |"; cout << right; 
    }
    gotoxy(4, y++); color(COLOR_PANEL);
    cout << "+------------+-------+------------------+----------------------+---------+"; color(COLOR_TEXTO);
    gotoxy(4, y+1); cout << "Total de coincidencias encontradas: " << idx;

    delete[] arreglo;
}

// --- ESTADÍSTICAS (Requisito L) ---
void mostrarEstadisticasCitas(TablaHash t, Fecha fFiltro, string especialidadFiltro) {
    int activas = 0;
    int canceladas = 0;

    for (int i = 0; i < TAM_TABLA; i++) {
        NodoCita* actual = t.buckets[i];
        while (actual != NULL) {
            bool matchFecha = true;
            bool matchEspec = true;

            // Filtro Fecha (si día != 0, significa que se está filtrando)
            if (fFiltro.dia != 0) {
                if (actual->datos.fecha.dia != fFiltro.dia || actual->datos.fecha.mes != fFiltro.mes || actual->datos.fecha.anio != fFiltro.anio) {
                    matchFecha = false;
                }
            }

            // Filtro Especialidad (¡CORRECCIÓN DEL BUG AQUÍ!)
            if (especialidadFiltro != "TODAS LAS ESPECIALIDADES") {
                if (actual->datos.especialidad != especialidadFiltro) matchEspec = false;
            }

            // Si pasa ambos filtros, sumamos
            if (matchFecha && matchEspec) {
                if (actual->datos.activa) activas++;
                else canceladas++;
            }
            actual = actual->siguiente;
        }
    }

    limpiarAreaTrabajo();
    int y = 5;
    gotoxy(5, y++); color(COLOR_TITULO); cout << "=== REPORTE ESTADISTICO DE CITAS ==="; color(COLOR_TEXTO);
    y++;
    gotoxy(5, y++); color(8); 
    cout << "Filtros aplicados:";
    gotoxy(5, y++); cout << "- Fecha: " << (fFiltro.dia == 0 ? "Global (Todas)" : to_string(fFiltro.dia) + "/" + to_string(fFiltro.mes) + "/" + to_string(fFiltro.anio));
    gotoxy(5, y++); cout << "- Especialidad: " << especialidadFiltro;
    color(COLOR_TEXTO); y++;

    gotoxy(5, y++); color(COLOR_PANEL); cout << ">> RESULTADOS DEL FILTRO:"; color(COLOR_TEXTO);
    gotoxy(5, y++); cout << "- Citas Activas (Agendadas):  " << activas;
    gotoxy(5, y++); cout << "- Citas Canceladas:           " << canceladas;
    gotoxy(5, y++); cout << "------------------------------------------";
    gotoxy(5, y++); cout << "TOTAL HISTORICO:              " << (activas + canceladas);
}