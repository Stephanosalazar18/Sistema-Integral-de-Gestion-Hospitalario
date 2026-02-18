#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>

// --- INCLUSIÓN DE MÓDULOS ---
#include "estructuras.h"
#include "interfaz.h"      // Motor gráfico y Dashboard
#include "AVL.h"           // Módulo 1: Triaje
#include "historia.h"      // Módulo 2: Historias (Cerebro)
#include "citas.h"         // Módulo 3: Citas
#include "hospital.h"      // Módulo 4: Hospitalización
#include "laboratorio.h"   // Módulo 5: Laboratorio

using namespace std;

// --- VARIABLES GLOBALES (ESTADO DEL SISTEMA) ---
// Definidas como extern en interfaz.h, aquí las instanciamos
Nodo* colaTriaje = NULL;        
ColaLaboratorio colaLab;        
PilaLaboratorio pilaResultados; 
TablaHash agendaCitas;          
Hospital hospitalCentral;       

// Base de Datos de Expedientes (Array de Punteros)
const int MAX_EXPEDIENTES = 100;
Expediente* dbExpedientes[MAX_EXPEDIENTES];
int cantExpedientes = 0;

// --- FUNCIÓN PUENTE (NEXO ENTRE MÓDULOS) ---
// Busca el expediente de un paciente. Si no existe, lo crea.
Expediente* obtenerExpediente(Paciente p) {
    // 1. Buscar si ya existe
    for(int i=0; i<cantExpedientes; i++) {
        if(dbExpedientes[i]->datosPaciente.cedula == p.cedula) {
            return dbExpedientes[i];
        }
    }
    // 2. Si no existe, crear uno nuevo
    if(cantExpedientes < MAX_EXPEDIENTES) {
        dbExpedientes[cantExpedientes] = crearExpediente(p);
        cantExpedientes++;
        return dbExpedientes[cantExpedientes-1];
    }
    return NULL; // Base de datos llena (caso extremo)
}

// Sobrecarga para buscar solo por cédula (si no tenemos el objeto Paciente completo)
Expediente* buscarExpedienteSoloCedula(string cedula) {
    for(int i=0; i<cantExpedientes; i++) {
        if(dbExpedientes[i]->datosPaciente.cedula == cedula) {
            return dbExpedientes[i];
        }
    }
    return NULL;
}

// --- CONTROLADORES DE FLUJO ---

void controladorTriaje() {
    while(true) {
        vector<string> ops = {
            "Registrar Paciente (Llegada)", 
            "Ver Sala de Espera (Prioridad)", 
            "ATENDER PACIENTE (Pasar a Consulta)", 
            "Volver al Menu Principal"
        };
        
        int sel = menuInteractivo("TRIAJE DE EMERGENCIAS", ops);

        if (sel == 0) { // Registrar
            limpiarAreaTrabajo();
            gotoxy(5, 5); cout << "--- NUEVO INGRESO A TRIAJE ---";
            
            Paciente p;
            
            // 1. Validaciones de Texto (No vacíos) usando el nuevo inputTexto
            p.cedula = inputTexto("Cedula: ", 7);
            p.nombre = inputTexto("Nombre Completo: ", 9);
            p.motivoConsulta = inputTexto("Motivo Consulta: ", 11);
            
            // 2. Validación de Urgencia (Estricta 0-3) usando inputNumero con rango
            gotoxy(5, 13); cout << "Niveles: 0:Critico | 1:Urgente | 2:Moderado | 3:Leve";
            int u = inputNumero("Indique Nivel de Urgencia (0-3): ", 14, 0, 3);
            p.urgencia = (NivelUrgencia)u;
            
            p.numLlegada = rand() % 1000; 
            
            // Inserción y creación de expediente
            insertarPaciente(colaTriaje, p);
            obtenerExpediente(p);
            
            mostrarMensajeExito("Paciente registrado correctamente.");
        }
        else if (sel == 1) { // Ver Sala
            limpiarAreaTrabajo();
            gotoxy(5, 5); cout << "--- PACIENTES EN ESPERA (DETALLADO) ---";
            gotoxy(5, 7);
            // El dashboard muestra el resumen, aquí mostramos el detalle completo
            if(colaTriaje == NULL) cout << "No hay pacientes.";
            else mostrarArbol(colaTriaje, 1); 
            pausarInterfaz();
        }
        else if (sel == 2) { // Atender
            if(colaTriaje == NULL) {
                mostrarMensajeError("No hay pacientes en espera.");
                continue;
            }

            Paciente pAtendido = colaTriaje->dato;
            eliminarPacienteMayorPrioridad(colaTriaje);
            
            Expediente* exp = obtenerExpediente(pAtendido);
            limpiarAreaTrabajo();
            gotoxy(5, 5); cout << "ATENDIENDO A: " << pAtendido.nombre;
            gotoxy(5, 6); cout << "Nivel: " << pAtendido.urgencia << " | Motivo: " << pAtendido.motivoConsulta;
            
            string diag = inputTexto("Diagnostico Medico: ", 8);
            string trat = inputTexto("Tratamiento/Indicaciones: ", 10);
            
            agregarConsulta(exp, {1,1,2025}, diag, trat, "Atendido desde Emergencia");
            mostrarMensajeExito("Consulta registrada.");

            // Flujo de derivación
            vector<string> opsFlujo = {"Dar de Alta (Casa)", "Enviar a Laboratorio", "Ingresar (Hospitalizar)"};
            int destino = menuInteractivo("DESTINO DEL PACIENTE", opsFlujo);

            if (destino == 1) { // Laboratorio
                Examen ex;
                ex.cedulaPaciente = pAtendido.cedula;
                ex.nombrePaciente = pAtendido.nombre;
                ex.fechaSolicitud = {1,1,2025};
                ex.procesado = false;
                ex.resultado = "PENDIENTE";
                
                limpiarAreaTrabajo();
                gotoxy(5,5); cout << "SOLICITUD DE EXAMEN";
                gotoxy(5,7); cout << "Tipos: 0:Hema, 1:Quim, 2:Imag, 3:Orina";
                int t = inputNumero("Tipo de Examen (0-3): ", 9, 0, 3);
                ex.tipo = (TipoExamen)t;
                
                if(!validarColaLaboratorio(colaLab, ex.cedulaPaciente)){
                     encolarSolicitud(colaLab, ex);
                     mostrarMensajeExito("Enviado a Laboratorio.");
                } else {
                     mostrarMensajeError("El paciente ya tiene solicitud pendiente.");
                }
            }
            else if (destino == 2) { // Hospitalización
                limpiarAreaTrabajo();
                gotoxy(5,5); cout << "INGRESO HOSPITALARIO";
                int piso = inputNumero("Asignar Piso (1-5): ", 7, 1, 5);
                int hab = inputNumero("Asignar Habitacion (1-10): ", 9, 1, 10);
                
                if(ingresarPaciente(hospitalCentral, pAtendido, {1,1,2025}, CUIDADOS_ESPECIALES, piso, hab)) {
                    string lugar = "Piso " + to_string(piso) + " - Hab " + to_string(hab);
                    agregarIngreso(exp, {1,1,2025}, lugar, diag);
                    mostrarMensajeExito("Paciente Ingresado.");
                } else {
                    mostrarMensajeError("No se pudo ingresar (Cama ocupada).");
                }
            }
            else {
                agregarAlta(exp, {1,1,2025}, "Alta rapida", "Reposo");
                mostrarMensajeExito("Paciente dado de alta.");
            }
        }
        else return;
    }
}

void controladorLaboratorio() {
    while(true) {
        vector<string> ops = {
            "Registrar Solicitud (Manual)", 
            "PROCESAR MUESTRA (Cola -> Pila)", 
            "Ver Historial Resultados", 
            "Volver"
        };
        int sel = menuInteractivo("LABORATORIO CLINICO", ops);

        if(sel == 0) { // Manual (Paciente externo)
            limpiarAreaTrabajo();
            Examen ex;
            ex.cedulaPaciente = inputTexto("Cedula: ", 6);
            ex.nombrePaciente = inputTexto("Nombre: ", 8);
            int t = inputNumero("Tipo (0-4): ", 10, 0, 4);
            ex.tipo = (TipoExamen)t;
            ex.fechaSolicitud = {1,1,2025};
            ex.procesado = false;
            
            // Crear expediente si no existe (es nuevo)
            Paciente pDummy; pDummy.cedula = ex.cedulaPaciente; pDummy.nombre = ex.nombrePaciente;
            obtenerExpediente(pDummy);

            if(encolarSolicitud(colaLab, ex)) mostrarMensajeExito("Solicitud enviada.");
        }
        else if(sel == 1) { // PROCESAR (CONEXIÓN CON HISTORIA)
            if(colaEstaVacia(colaLab)) {
                mostrarMensajeError("No hay muestras pendientes.");
            } else {
                limpiarAreaTrabajo();
                
                // 1. Obtener puntero al dato actual
                Examen* siguiente = &colaLab.ultimo->siguiente->datos;
                
                gotoxy(5, 5); cout << "PROCESANDO: " << siguiente->nombrePaciente;
                gotoxy(5, 6); cout << "Examen: " << obtenerNombreExamen(siguiente->tipo);
                
                string res = inputTexto("Resultado del Analisis: ", 9);
                
                // --- CORRECCIÓN CRÍTICA AQUÍ ---
                // Guardamos los datos en variables locales ANTES de que procesarExamen borre el nodo
                string cedulaTemp = siguiente->cedulaPaciente;
                TipoExamen tipoTemp = siguiente->tipo;
                // -------------------------------

                // 2. Procesar (Esto elimina el nodo de la cola y libera esa memoria)
                procesarExamen(colaLab, pilaResultados, res);
                
                // 3. Guardar en Historia Clínica (Usando las variables temporales seguras)
                Expediente* exp = buscarExpedienteSoloCedula(cedulaTemp); // Usamos cedulaTemp
                
                if(exp) {
                    // Usamos tipoTemp
                    agregarResultadoLab(exp, {1,1,2025}, obtenerNombreExamen(tipoTemp), res);
                    mostrarMensajeExito("Procesado y guardado en Historia Clinica.");
                } else {
                    mostrarMensajeExito("Procesado (Paciente sin historia previa).");
                }
            }
        }
        else if(sel == 2) {
            limpiarAreaTrabajo();
            mostrarPila(pilaResultados);
            pausarInterfaz();
        }
        else return;
    }
}

void controladorHospital() {
    while(true) {
        vector<string> ops = {
            "Ver Mapa de Camas", 
            "Ver Disponibilidad", 
            "DAR DE ALTA (Liberar Cama)",
            "Volver"
        };
        int sel = menuInteractivo("HOSPITALIZACION", ops);

        if (sel == 0) {
            limpiarAreaTrabajo();
            mostrarHabitacionesOcupadas(hospitalCentral);
            pausarInterfaz();
        }
        else if (sel == 1) {
            limpiarAreaTrabajo();
            mostrarHabitacionesLibres(hospitalCentral);
            pausarInterfaz();
        }
        else if (sel == 2) { // DAR DE ALTA (CONEXIÓN CON HISTORIA)
            limpiarAreaTrabajo();
            gotoxy(5, 5); cout << "--- PROCESO DE ALTA ---";
            int p = inputNumero("Piso (1-5): ", 7, 1, 5);
            int h = inputNumero("Habitacion (1-10): ", 9, 1, 10);
            
            // Buscar primero quién está ahí para el reporte
            NodoMatriz* cama = buscarHabitacion(hospitalCentral, p, h);
            
            if(cama != NULL) {
                string motivo = inputTexto("Motivo del Alta: ", 11);
                string indic = inputTexto("Indicaciones: ", 13);
                
                // 1. Guardar en Historia
                Expediente* exp = buscarExpedienteSoloCedula(cama->paciente.cedula);
                if(exp) {
                    agregarAlta(exp, {1,1,2025}, motivo, indic);
                }
                
                // 2. Liberar Cama (Módulo 4)
                darAltaPaciente(hospitalCentral, p, h);
                mostrarMensajeExito("Paciente egresado y cama liberada.");
            } else {
                mostrarMensajeError("Esa habitacion ya esta vacia.");
            }
        }
        else return;
    }
}

void controladorHistorias() {
    // Módulo de solo lectura y consultas
    limpiarAreaTrabajo();
    string ced = inputTexto("Ingrese Cedula a Consultar: ", 10);
    
    Expediente* exp = buscarExpedienteSoloCedula(ced);
    
    if(exp) {
        limpiarAreaTrabajo();
        // Nota: mostrarHistoriaClinica usa cout simple.
        // Lo imprimimos en la posición inicial
        gotoxy(2, 4);
        mostrarHistoriaClinica(exp);
    } else {
        mostrarMensajeError("Paciente no tiene expediente.");
    }
    pausarInterfaz();
}

void controladorCitas() {
    while(true) {
        vector<string> ops = {"Agendar Cita", "Ver Agenda", "Cancelar Cita", "Volver"};
        int sel = menuInteractivo("CONSULTAS Y CITAS", ops);
        
        if(sel == 0) {
            limpiarAreaTrabajo();
            Cita c;
            c.idPaciente = inputTexto("Cedula: ", 6);
            c.nombreMedico = inputTexto("Medico: ", 8);
            c.fecha.dia = inputNumero("Dia (1-31): ", 10, 1, 31);
            c.fecha.mes = 5; 
            c.fecha.anio = 2025;
            c.hora.hora = inputNumero("Hora (0-23): ", 13, 0, 23);
            c.hora.minuto = 0;
            
            if(registrarCita(agendaCitas, c)) {
                // Crear expediente preventivo
                Paciente pDummy; pDummy.cedula = c.idPaciente; pDummy.nombre = "Paciente Cita";
                obtenerExpediente(pDummy);
                mostrarMensajeExito("Cita Agendada.");
            } else {
                mostrarMensajeError("Choque de Horario.");
            }
        }
        else if(sel == 1) {
            limpiarAreaTrabajo();
            mostrarTablaHash(agendaCitas);
            pausarInterfaz();
        }
        else if(sel == 2) {
            limpiarAreaTrabajo();
            string c = inputTexto("Cedula: ", 6);
            cancelarCita(agendaCitas, c, {10,5,2025}, {9,0}); // Simplificado
            pausarInterfaz();
        }
        else return;
    }
}

// --- MAIN PRINCIPAL ---

int main() {
    // Configuración
    ocultarCursor();
    SetConsoleTitle("SISTEMA HOSPITALARIO V3.0 (INTEGRADO)");
    srand(time(0));

    // Inicializaciones
    inicializarCola(colaLab);
    inicializarPila(pilaResultados);
    inicializarTabla(agendaCitas);
    inicializarHospital(hospitalCentral);

    encabezado(); // Dibujar marco estático

    while (true) {
        vector<string> menu = {
            "TRIAJE (Emergencias -> Ingresos)",
            "HISTORIAS CLINICAS (Consultar)",
            "CITAS MEDICAS (Hash Table)",
            "HOSPITALIZACION (Pisos y Camas)",
            "LABORATORIO (Colas y Resultados)",
            "SALIR"
        };

        int sel = menuInteractivo("PANEL DE CONTROL GENERAL", menu);

        switch (sel) {
            case 0: controladorTriaje(); break;
            case 1: controladorHistorias(); break;
            case 2: controladorCitas(); break;
            case 3: controladorHospital(); break;
            case 4: controladorLaboratorio(); break;
            case 5: 
                limpiarAreaTrabajo();
                gotoxy(30, 15); cout << "Cerrando sistema...";
                return 0;
        }
    }
    return 0;
}