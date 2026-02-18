#include <iostream>
#include <limits> // Para limpiar buffer
#include "estructuras.h"
#include "AVL.h"
#include "laboratorio.h"
#include "historia.h"
#include "citas.h"
#include "hospital.h"

using namespace std;

// --- ESTRUCTURAS GLOBALES DEL SISTEMA ---
Nodo* colaTriaje = NULL;         // Módulo 1 (AVL)
ColaLaboratorio colaLab;            // Módulo 5 (Cola)
NodoPila* pilaResultados = NULL;    // Módulo 5 (Pila)
TablaHash agendaCitas;              // Módulo 3 (Hash)
Hospital hospitalCentral;           // Módulo 4 (Matriz Dispersa)

// Simulación de Base de Datos de Historias (Array simple)
const int MAX_EXPEDIENTES = 50;
Expediente* dbExpedientes[MAX_EXPEDIENTES];
int cantExpedientes = 0;

// --- FUNCIONES AUXILIARES ---

void pausar() {
    cout << "\nPresione ENTER para continuar...";
    cin.ignore();
    cin.get();
}

// Busca un expediente existente o crea uno nuevo si no existe
Expediente* buscarOcrearExpediente(string cedula, string nombre) {
    for(int i=0; i<cantExpedientes; i++) {
        if(dbExpedientes[i]->datosPaciente.cedula == cedula) {
            return dbExpedientes[i];
        }
    }
    if(cantExpedientes < MAX_EXPEDIENTES) {
        Paciente p; p.cedula = cedula; p.nombre = nombre;
        dbExpedientes[cantExpedientes] = crearExpediente(p);
        cantExpedientes++;
        return dbExpedientes[cantExpedientes-1];
    }
    return NULL;
}

// --- MENÚS POR MÓDULO ---

void menuTriaje() {
    int opc;
    do {
        cout << "\n--- MODULO 1: TRIAJE (EMERGENCIAS) ---" << endl;
        cout << "1. Ingresar Paciente (Recepcion)" << endl;
        cout << "2. Ver Cola de Espera (Prioridad AVL)" << endl;
        cout << "3. Atender Paciente (Pasar a consulta)" << endl;
        cout << "0. Volver" << endl;
        cout << "Opcion: "; cin >> opc;

        if (opc == 1) {
            Paciente p;
            int urg;
            cout << "Cedula: "; cin >> p.cedula;
            cin.ignore(); 
            cout << "Nombre: "; getline(cin, p.nombre);
            cout << "Motivo: "; getline(cin, p.motivoConsulta);
            cout << "Nivel Urgencia (0:Critico, 1:Urgente, 2:Mod, 3:Leve): "; cin >> urg;
            p.urgencia = (NivelUrgencia)urg;
            cout << "Numero llegada (ej. 1, 2...): "; cin >> p.numLlegada;
            
            insertarPaciente(colaTriaje, p);
        }
        else if (opc == 2) {
            cout << "\nPacientes en espera (Ordenados por Gravedad):" << endl;
            mostrarArbol(colaTriaje, 1); // 1 = Inorden (De menor a mayor valor numérico = Mayor prioridad)
        }
        else if (opc == 3) {
            eliminarPacienteMayorPrioridad(colaTriaje);
        }
    } while(opc != 0);
}

void menuCitas() {
    int opc;
    do {
        cout << "\n--- MODULO 3: GESTION DE CITAS (HASH) ---" << endl;
        cout << "1. Agendar Nueva Cita" << endl;
        cout << "2. Ver Agenda Completa" << endl;
        cout << "3. Cancelar Cita" << endl;
        cout << "0. Volver" << endl;
        cout << "Opcion: "; cin >> opc;

        if (opc == 1) {
            Cita c;
            cout << "Cedula Paciente: "; cin >> c.idPaciente;
            cin.ignore();
            cout << "Nombre Medico: "; getline(cin, c.nombreMedico);
            cout << "Especialidad: "; getline(cin, c.especialidad);
            cout << "Fecha (D M A): "; cin >> c.fecha.dia >> c.fecha.mes >> c.fecha.anio;
            cout << "Hora (0-23): "; cin >> c.hora.hora; c.hora.minuto = 0;
            c.activa = true;
            registrarCita(agendaCitas, c);
        }
        else if (opc == 2) {
            mostrarTodaLaAgenda(agendaCitas);
        }
        else if (opc == 3) {
            string ced; Fecha f; Hora h;
            cout << "Cedula: "; cin >> ced;
            cout << "Fecha (D M A): "; cin >> f.dia >> f.mes >> f.anio;
            cout << "Hora: "; cin >> h.hora;
            cancelarCita(agendaCitas, ced, f, h);
        }
    } while(opc != 0);
}

void menuHospital() {
    int opc;
    do {
        cout << "\n--- MODULO 4: HOSPITALIZACION (MATRIZ) ---" << endl;
        cout << "1. Ingresar Paciente a Cama" << endl;
        cout << "2. Ver Mapa de Camas (Ocupadas)" << endl;
        cout << "3. Ver Camas Libres" << endl;
        cout << "4. Dar de Alta" << endl;
        cout << "0. Volver" << endl;
        cout << "Opcion: "; cin >> opc;

        if (opc == 1) {
            Paciente p; Fecha f; int piso, hab, tipo;
            cout << "Cedula: "; cin >> p.cedula;
            cin.ignore();
            cout << "Nombre: "; getline(cin, p.nombre);
            cout << "Fecha Ingreso (D M A): "; cin >> f.dia >> f.mes >> f.anio;
            cout << "Tipo (0:Indiv, 3:Doble...): "; cin >> tipo;
            cout << "Piso (1-5): "; cin >> piso;
            cout << "Habitacion (1-10): "; cin >> hab;
            
            ingresarPaciente(hospitalCentral, p, f, (TipoHabitacion)tipo, piso, hab);
        }
        else if (opc == 2) mostrarHabitacionesOcupadas(hospitalCentral);
        else if (opc == 3) mostrarHabitacionesLibres(hospitalCentral);
        else if (opc == 4) {
            int p, h;
            cout << "Piso: "; cin >> p;
            cout << "Habitacion: "; cin >> h;
            darAltaPaciente(hospitalCentral, p, h);
        }
    } while(opc != 0);
}

void menuLaboratorio() {
    int opc;
    do {
        cout << "\n--- MODULO 5: LABORATORIO (COLA/PILA) ---" << endl;
        cout << "1. Registrar Solicitud de Examen" << endl;
        cout << "2. Procesar Siguiente Examen (Bioanalista)" << endl;
        cout << "3. Ver Historial Resultados" << endl;
        cout << "0. Volver" << endl;
        cout << "Opcion: "; cin >> opc;

        if (opc == 1) {
            Examen ex;
            cout << "Cedula Paciente: "; cin >> ex.cedulaPaciente;
            ex.fechaSolicitud = {1,1,2025}; // Fecha dummy por rapidez
            ex.procesado = false;
            encolarSolicitud(colaLab, ex);
        }
        else if (opc == 2) {
            string res;
            cout << "Ingrese resultado del analisis: ";
            cin.ignore(); getline(cin, res);
            procesarExamen(colaLab, pilaResultados, res);
        }
        else if (opc == 3) mostrarPila(pilaResultados);
    } while(opc != 0);
}

void menuHistorias() {
    cout << "\n--- MODULO 2: HISTORIAS CLINICAS (LISTA DOBLE) ---" << endl;
    string ced, nom;
    cout << "Ingrese Cedula del Paciente a consultar: "; cin >> ced;
    
    // Buscar o crear dummy para no complicar el flujo
    Expediente* exp = buscarOcrearExpediente(ced, "Paciente (Sin Nombre Registrado)");
    
    int opc;
    do {
        cout << "\n Expediente: " << exp->datosPaciente.cedula << endl;
        cout << "1. Agregar Consulta Medica" << endl;
        cout << "2. Ver Historia Completa" << endl;
        cout << "3. Buscar Consulta por Fecha (Recursivo)" << endl;
        cout << "0. Volver" << endl;
        cout << "Opcion: "; cin >> opc;
        
        if (opc == 1) {
            Fecha f; string d, t, o;
            cout << "Fecha (D M A): "; cin >> f.dia >> f.mes >> f.anio;
            cin.ignore();
            cout << "Diagnostico: "; getline(cin, d);
            cout << "Tratamiento: "; getline(cin, t);
            cout << "Observaciones: "; getline(cin, o);
            agregarRegistroMedico(exp, f, d, t, o);
        }
        else if (opc == 2) mostrarHistoriaClinica(exp);
        else if (opc == 3) {
            Fecha f;
            cout << "Fecha a buscar (D M A): "; cin >> f.dia >> f.mes >> f.anio;
            NodoHistoria* res = buscarRegistroPorFecha(exp->inicio, f);
            if (res) cout << ">>> ENCONTRADO: " << res->diagnostico << " | Tratamiento: " << res->tratamiento << endl;
            else cout << ">>> NO SE ENCONTRARON REGISTROS EN ESA FECHA." << endl;
        }
    } while(opc != 0);
}

// --- MAIN PRINCIPAL ---

int main() {
    // Inicializar todas las estructuras globales
    inicializarCola(colaLab, 5);
    inicializarTabla(agendaCitas);
    inicializarHospital(hospitalCentral);

    int opcion;
    do {
        cout << "\n==========================================" << endl;
        cout << " SISTEMA INTEGRAL DE GESTION HOSPITALARIA " << endl;
        cout << "==========================================" << endl;
        cout << "1. Modulo TRIAJE (Arbol AVL)" << endl;
        cout << "2. Modulo HISTORIAS CLINICAS (Lista Doble)" << endl;
        cout << "3. Modulo CITAS MEDICAS (Tabla Hash)" << endl;
        cout << "4. Modulo HOSPITALIZACION (Matriz Dispersa)" << endl;
        cout << "5. Modulo LABORATORIO (Cola/Pila)" << endl;
        cout << "0. SALIR" << endl;
        cout << "==========================================" << endl;
        cout << "Seleccione una opcion: ";
        cin >> opcion;

        switch (opcion) {
            case 1: menuTriaje(); break;
            case 2: menuHistorias(); break;
            case 3: menuCitas(); break;
            case 4: menuHospital(); break;
            case 5: menuLaboratorio(); break;
            case 0: cout << "Cerrando sistema..." << endl; break;
            default: cout << "Opcion invalida." << endl;
        }
        
        if(opcion != 0) pausar();

    } while (opcion != 0);

    return 0;
}