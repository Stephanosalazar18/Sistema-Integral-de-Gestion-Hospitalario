// Sistema Integral de Gestion Hospitalaria (S.I.G.H.)
// Versión(Final)
// compilar
// g++ main.cpp AVL.cpp laboratorio.cpp citas.cpp hospital.cpp historia.cpp -o sistema_hospital 





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


struct RegistroAtencion {
    Fecha fecha;
    NivelUrgencia urgencia;
    int destino; // 0: Casa, 1: Laboratorio, 2: Hospitalizacion
};
const int MAX_ATENCIONES = 1000;
RegistroAtencion historialAtenciones[MAX_ATENCIONES];
int cantAtenciones = 0;


// Para el módulo de Hospitalización, llevaremos un registro de egresos para estadísticas
struct RegistroEgreso {
    Fecha fecha;
    int piso;
    int hab;
};
const int MAX_EGRESOS = 1000;
RegistroEgreso historialEgresos[MAX_EGRESOS];
int cantEgresos = 0;

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
int contadorLlegadasGlobal = 1;


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


// Contorladores de flujo, cada uno maneja la lógica de su módulo y se comunica con los demás a través de funciones puente y estructuras compartidas.
// si este codigo no explota es porque es magico, no lo toques

// controlador de Triaje, arbol AVL, prioridad, estadisticas
void controladorTriaje() {
    while(true) {
        // Añadimos la nueva opción de Estadísticas
        string ops[] = {
            "Registrar Paciente (Llegada)", 
            "Ver Sala de Espera (Prioridad)", 
            "ATENDER PACIENTE (Pasar a Consulta)", 
            "Ver Estadisticas de Atencion", 
            "Volver al Menu Principal"
        };
        int cantOps = 5; 
        
        int sel = menuInteractivo("TRIAJE DE EMERGENCIAS", ops, cantOps);

        if (sel == 0) { // Registrar
            limpiarAreaTrabajo();
            gotoxy(5, 5); cout << "--- NUEVO INGRESO A TRIAJE ---";
            
            Paciente p;
            p.cedula = inputTexto("Cedula: ", 7);

            if (existeEnAVL(colaTriaje, p.cedula)) {
                mostrarMensajeError("Este paciente ya esta en la sala de espera.");
                continue; 
            }

            p.nombre = inputTexto("Nombre Completo: ", 9);
            p.motivoConsulta = inputTexto("Motivo Consulta: ", 11);
            
            gotoxy(5, 13); cout << "Niveles: 0:Critico | 1:Urgente | 2:Moderado | 3:Leve";
            int u = inputNumero("Indique Nivel de Urgencia (0-3): ", 14, 0, 3);
            p.urgencia = (NivelUrgencia)u;
            
            p.numLlegada = contadorLlegadasGlobal++; 
            
            insertarPaciente(colaTriaje, p);
            obtenerExpediente(p);
            
            mostrarMensajeExito("Paciente registrado correctamente.");
        }
        else if (sel == 1) { // Ver Sala
            if(colaTriaje == NULL) {
                mostrarMensajeError("No hay pacientes en espera.");
                continue;
            }

            string opsRecorrido[] = {
                "Ver en PREORDEN (Raiz-Izq-Der)", 
                "Ver en INORDEN (Por Prioridad Estricta)", 
                "Ver en POSTORDEN (Izq-Der-Raiz)"
            };
            int selRecorrido = menuInteractivo("TIPO DE RECORRIDO (ARBOL AVL)", opsRecorrido, 3);

            limpiarAreaTrabajo();
            gotoxy(5, 5); color(COLOR_TITULO); cout << "--- PACIENTES EN ESPERA ---"; color(COLOR_TEXTO);
            gotoxy(5, 7);
            
            mostrarArbol(colaTriaje, selRecorrido); 
            
            pausarInterfaz();
        }
        else if (sel == 2) { // Atender
            if(colaTriaje == NULL) {
                mostrarMensajeError("No hay pacientes en espera.");
                continue;
            }

            Paciente pAtendido = eliminarPacienteMayorPrioridad(colaTriaje);
            
            Expediente* exp = obtenerExpediente(pAtendido);
            limpiarAreaTrabajo();
            gotoxy(5, 5); cout << "ATENDIENDO A: " << pAtendido.nombre;
            gotoxy(5, 6); cout << "Nivel: " << pAtendido.urgencia << " | Motivo: " << pAtendido.motivoConsulta;
            
            // Para simplificar la fecha (En un sistema real se usa la hora del servidor)
            Fecha fechaAtencion = {1, 1, 2026}; 

            string diag = inputTexto("Diagnostico Medico: ", 8);
            string trat = inputTexto("Tratamiento/Indicaciones: ", 10);
            
            agregarConsulta(exp, fechaAtencion, diag, trat, "Atendido desde Emergencia");
            mostrarMensajeExito("Consulta registrada.");

            string opsFlujo[] = {"Dar de Alta (Casa)", "Enviar a Laboratorio", "Ingresar (Hospitalizar)"};
            int cantOpsFlujo = 3;
            
            int destino = menuInteractivo("DESTINO DEL PACIENTE", opsFlujo, cantOpsFlujo);

            if (destino == 1) { // Laboratorio
                Examen ex;
                ex.cedulaPaciente = pAtendido.cedula;
                ex.nombrePaciente = pAtendido.nombre;
                ex.fechaSolicitud = fechaAtencion;
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
                
                if(ingresarPaciente(hospitalCentral, pAtendido, fechaAtencion, CUIDADOS_ESPECIALES, piso, hab)) {
                    string lugar = "Piso " + to_string(piso) + " - Hab " + to_string(hab);
                    agregarIngreso(exp, fechaAtencion, lugar, diag);
                    mostrarMensajeExito("Paciente Ingresado.");
                } else {
                    mostrarMensajeError("No se pudo ingresar (Cama ocupada).");
                }
            }
            else { // Alta
                agregarAlta(exp, fechaAtencion, "Alta rapida", "Reposo");
                mostrarMensajeExito("Paciente dado de alta.");
            }

            // --- MAGIA ESTADISTICA: Guardar el registro de la atención ---
            if (cantAtenciones < MAX_ATENCIONES) {
                historialAtenciones[cantAtenciones].fecha = fechaAtencion;
                historialAtenciones[cantAtenciones].urgencia = pAtendido.urgencia;
                historialAtenciones[cantAtenciones].destino = destino; // Coincide perfecto (0, 1, 2)
                cantAtenciones++;
            }
        }
        else if (sel == 3) { // --- NUEVO: ESTADISTICAS REQUISITO PDF ---
            limpiarAreaTrabajo();
            gotoxy(5, 5); color(COLOR_TITULO); cout << "--- ESTADISTICAS DE TRIAJE ---"; color(COLOR_TEXTO);
            
            // PDF: "por fecha, por nivel de prioridad, o ambas" 
            int d = inputNumero("Filtrar por dia (0 para ver el global total): ", 7, 0, 31);
            int m = 0, a = 0;
            if (d != 0) {
                m = inputNumero("Mes (1-12): ", 8, 1, 12);
                a = inputNumero("Anio (Ej. 2026): ", 9, 2020, 2100);
            }

            // Contadores
            int contCritico = 0, contUrgente = 0, contModerado = 0, contLeve = 0;
            int contCasa = 0, contLab = 0, contHosp = 0;
            int totalFiltrado = 0;

            for (int i = 0; i < cantAtenciones; i++) {
                RegistroAtencion reg = historialAtenciones[i];
                
                // Aplicar el filtro de fecha si el usuario no ingresó 0
                if (d != 0) {
                    if (reg.fecha.dia != d || reg.fecha.mes != m || reg.fecha.anio != a) {
                        continue; // Saltar si la fecha no coincide
                    }
                }

                totalFiltrado++;

                // Conteo por Prioridad
                if (reg.urgencia == CRITICO) contCritico++;
                else if (reg.urgencia == URGENTE) contUrgente++;
                else if (reg.urgencia == MODERADO) contModerado++;
                else if (reg.urgencia == LEVE) contLeve++;

                // Conteo por Destino/Remisión
                if (reg.destino == 0) contCasa++;
                else if (reg.destino == 1) contLab++;
                else if (reg.destino == 2) contHosp++;
            }

            // Mostrar el Dashboard Estadístico
            limpiarAreaTrabajo();
            gotoxy(5, 5); color(COLOR_TITULO); cout << "=== REPORTE ESTADISTICO ==="; color(COLOR_TEXTO);
            
            int y = 7;
            if (d == 0) { gotoxy(5, y++); color(8); cout << "[ Filtro Activo: TODAS LAS FECHAS (Global) ]"; color(COLOR_TEXTO); }
            else { gotoxy(5, y++); color(8); cout << "[ Filtro Activo: Pacientes del " << d << "/" << m << "/" << a << " ]"; color(COLOR_TEXTO); }
            
            y++;
            gotoxy(5, y++); cout << "Total Pacientes Atendidos: " << totalFiltrado;
            
            y++;
            gotoxy(5, y++); color(COLOR_PANEL); cout << ">> POR NIVEL DE PRIORIDAD:"; color(COLOR_TEXTO);
            gotoxy(5, y++); cout << "- CRITICOS (Nivel 0): " << contCritico;
            gotoxy(5, y++); cout << "- URGENTES (Nivel 1): " << contUrgente;
            gotoxy(5, y++); cout << "- MODERADOS (Nivel 2): " << contModerado;
            gotoxy(5, y++); cout << "- LEVES (Nivel 3): " << contLeve;
            
            y++;
            gotoxy(5, y++); color(COLOR_PANEL); cout << ">> POR DESTINO / REMISION:"; color(COLOR_TEXTO);
            gotoxy(5, y++); cout << "- Alta Medica (A su Casa): " << contCasa;
            gotoxy(5, y++); cout << "- Enviados a Laboratorio: " << contLab;
            gotoxy(5, y++); cout << "- Ingresados (Hospitalizados): " << contHosp;
            
            pausarInterfaz();
        }
        else return;
    }
}


// Controlador de Historias Clinicas: Permite gestionar los expedientes médicos de los pacientes. Cada expediente es una lista doblemente enlazada de eventos médicos (consultas, hospitalizaciones, exámenes de laboratorio, etc.). El controlador ofrece un CRUD completo para agregar, modificar, eliminar y buscar registros dentro del expediente. Además, implementa una función de búsqueda por fecha que utiliza recursividad para encontrar el registro específico. La visualización de la historia clínica se presenta en orden cronológico, mostrando claramente la secuencia de eventos médicos del paciente. También incluye estadísticas sobre el tipo y frecuencia de eventos registrados en el expediente.
void mostrarHistoriaUI(Expediente* exp) {
    limpiarAreaTrabajo();
    gotoxy(4, 5); color(COLOR_TITULO); cout << "=== HISTORIA CLINICA ==="; color(COLOR_TEXTO);
    gotoxy(4, 6); cout << "Paciente: " << exp->datosPaciente.nombre;
    gotoxy(4, 7); cout << "Cedula: " << exp->datosPaciente.cedula << " | ID Exp: " << exp->numeroHistoria;
    
    if (exp->inicio == NULL) {
        gotoxy(4, 9); color(8); cout << "(Sin registros medicos)"; color(COLOR_TEXTO);
        pausarInterfaz();
        return;
    }

    NodoHistoria* actual = exp->inicio;
    int y = 9;
    int pagina = 1;
    
    while (actual != NULL) {
        // Paginación si la historia es muy larga
        if (y > 20) { 
            gotoxy(4, 24); color(8); cout << "--- Presione ENTER para ver pagina " << pagina + 1 << " ---"; color(COLOR_TEXTO);
            _getch();
            limpiarAreaTrabajo();
            gotoxy(4, 5); color(COLOR_TITULO); cout << "=== HISTORIA CLINICA (Pag " << ++pagina << ") ==="; color(COLOR_TEXTO);
            y = 7;
        }
        
        gotoxy(4, y++); color(COLOR_EXITO); 
        cout << "[-] " << actual->fechaEvento.dia << "/" << actual->fechaEvento.mes << "/" << actual->fechaEvento.anio 
             << " - " << tipoEventoStr(actual->tipo); 
        color(COLOR_TEXTO);
        
        // Formato condicional según el tipo de evento
        if (actual->tipo == CONSULTA_MEDICA) {
            gotoxy(8, y++); cout << "Diagnostico: " << actual->diagnostico;
            gotoxy(8, y++); cout << "Tratamiento: " << actual->tratamiento;
        } else if (actual->tipo == INGRESO_HOSPITALIZACION) {
            gotoxy(8, y++); cout << "Cama: " << actual->habitacionAsignada;
            gotoxy(8, y++); cout << "Motivo: " << actual->observaciones;
        } else if (actual->tipo == EXAMEN_LABORATORIO) {
            gotoxy(8, y++); cout << "Examen: " << actual->tipoExamen;
            gotoxy(8, y++); cout << "Resultado: " << actual->resultadoLab;
        } else if (actual->tipo == ALTA_MEDICA) {
            gotoxy(8, y++); cout << "Resumen: " << actual->diagnostico;
            gotoxy(8, y++); cout << "Indicaciones: " << actual->tratamiento;
        }
        
        // Observaciones extra
        if (!actual->observaciones.empty() && actual->tipo != INGRESO_HOSPITALIZACION && actual->tipo != CONSULTA_MEDICA) {
            gotoxy(8, y++); cout << "Obs: " << actual->observaciones;
        }
        y++; // Espacio entre registros
        actual = actual->siguiente;
    }
    
    gotoxy(4, 24); color(8); cout << "--- Fin del Expediente ---"; color(COLOR_TEXTO);
    pausarInterfaz();
}

NodoHistoria* seleccionarRegistro(Expediente* exp, string tituloAccion) {
    if (exp->inicio == NULL) {
        mostrarMensajeError("El expediente no tiene registros.");
        return NULL;
    }

    // 1. Contar cuántos registros tiene el paciente
    int cant = 0;
    NodoHistoria* aux = exp->inicio;
    while(aux != NULL) {
        cant++;
        aux = aux->siguiente;
    }

    // 2. Crear arreglos dinámicos en memoria
    int totalOps = cant + 3; // "Buscar Manual", [nodos...], "", "Cancelar"
    string* ops = new string[totalOps];
    Fecha* fechasMap = new Fecha[cant];

    // Mantenemos la búsqueda manual como primera opción por seguridad (y para la evaluación)
    ops[0] = "Buscar Manualmente por Fecha";
    
    // Llenar la lista con los registros
    aux = exp->inicio;
    for(int i = 0; i < cant; i++) {
        string d = (aux->fechaEvento.dia < 10 ? "0" : "") + to_string(aux->fechaEvento.dia);
        string m = (aux->fechaEvento.mes < 10 ? "0" : "") + to_string(aux->fechaEvento.mes);
        string a = to_string(aux->fechaEvento.anio);
        
        string fechaStr = d + "/" + m + "/" + a;
        string tipoStr = tipoEventoStr(aux->tipo);
        string desc = aux->descripcion;
        if(desc.length() > 25) desc = desc.substr(0, 22) + "...";
        
        ops[i+1] = fechaStr + " | " + tipoStr + " | " + desc;
        fechasMap[i] = aux->fechaEvento; // Guardamos la fecha que corresponde a esta opción
        aux = aux->siguiente;
    }

    ops[totalOps - 2] = "";
    ops[totalOps - 1] = "Cancelar y Volver";

    // 3. Mostrar el menú
    int sel = menuInteractivo(tituloAccion, ops, totalOps);

    NodoHistoria* seleccionado = NULL;

    if (sel == 0) {
        // Opción 0: Búsqueda Manual
        limpiarAreaTrabajo();
        gotoxy(5, 5); color(COLOR_TITULO); cout << "--- BUSQUEDA MANUAL ---"; color(COLOR_TEXTO);
        int dia = inputNumero("Dia (1-31): ", 7, 1, 31);
        int mes = inputNumero("Mes (1-12): ", 9, 1, 12);
        int anio = inputNumero("Anio: ", 11, 2020, 2100);
        Fecha fBusq = {dia, mes, anio};
        
        // Uso de la función RECURSIVA del PDF
        seleccionado = buscarRegistroPorFecha(exp->inicio, fBusq);
        if(!seleccionado) mostrarMensajeError("No existe registro en esa fecha.");
    }
    else if (sel == totalOps - 1 || ops[sel] == "") {
        // Canceló
        seleccionado = NULL;
    }
    else {
        // Seleccionó un registro de la lista visual
        // Extraemos su fecha oculta en el arreglo y llamamos a la función RECURSIVA para buscarlo
        Fecha fSel = fechasMap[sel - 1];
        seleccionado = buscarRegistroPorFecha(exp->inicio, fSel); 
    }

    // Liberar memoria
    delete[] ops;
    delete[] fechasMap;
    
    return seleccionado;
}

void subMenuHistoria(Expediente* exp) {
    while(true) {
        string ops[] = {
            "Ver Historia Clinica Completa",
            "Ver Detalle de un Registro (Buscar)",
            "Modificar un Registro",
            "Eliminar un Registro",
            "Volver al Menu de Pacientes"
        };
        
        string titulo = "EXPEDIENTE: " + exp->datosPaciente.nombre;
        int sel = menuInteractivo(titulo, ops, 5);

        if (sel == 0) { // Ver Completa
            mostrarHistoriaUI(exp);
        } 
        else if (sel == 1) { // Buscar/Ver
            NodoHistoria* hallado = seleccionarRegistro(exp, "SELECCIONE REGISTRO A CONSULTAR");
            
            if (hallado) {
                limpiarAreaTrabajo();
                gotoxy(5, 5); color(COLOR_EXITO); cout << "--- DETALLE DEL REGISTRO ---"; color(COLOR_TEXTO);
                gotoxy(5, 7); cout << "Fecha: " << hallado->fechaEvento.dia << "/" << hallado->fechaEvento.mes << "/" << hallado->fechaEvento.anio;
                gotoxy(5, 8); cout << "Tipo: " << tipoEventoStr(hallado->tipo);
                gotoxy(5, 9); cout << "Descripcion: " << hallado->descripcion;
                gotoxy(5, 10); cout << "Diagnostico/Result: " << (hallado->diagnostico != "" ? hallado->diagnostico : hallado->resultadoLab);
                gotoxy(5, 11); cout << "Tratamiento/Indic: " << hallado->tratamiento;
                if(hallado->observaciones != "") { gotoxy(5, 12); cout << "Obs: " << hallado->observaciones; }
                pausarInterfaz();
            }
        } 
        else if (sel == 2) { // Modificar
            NodoHistoria* hallado = seleccionarRegistro(exp, "SELECCIONE REGISTRO A MODIFICAR");
            
            if (hallado) {
                limpiarAreaTrabajo();
                gotoxy(5, 5); color(COLOR_TITULO); cout << "--- MODIFICAR REGISTRO ---"; color(COLOR_TEXTO);
                gotoxy(5, 7); cout << "Modificando registro del " << hallado->fechaEvento.dia << "/" << hallado->fechaEvento.mes << "/" << hallado->fechaEvento.anio;
                
                string nDiag = inputTexto("Nuevo Diagnostico/Resumen: ", 9);
                string nTrat = inputTexto("Nuevo Tratamiento/Indicacion: ", 11);
                string nObs = inputTexto("Nuevas Observaciones: ", 13);
                
                // Usamos la función del backend enviando la fecha del nodo que seleccionamos
                modificarRegistro(exp, hallado->fechaEvento, nDiag, nTrat, nObs);
                mostrarMensajeExito("Registro actualizado manteniendo su posicion cronologica.");
            }
        } 
        else if (sel == 3) { // Eliminar (Con confirmación)
            NodoHistoria* hallado = seleccionarRegistro(exp, "SELECCIONE REGISTRO A ELIMINAR");
            
            if (hallado) {
                limpiarAreaTrabajo();
                gotoxy(5, 5); color(COLOR_ALERTA); cout << "--- ELIMINAR REGISTRO ---"; color(COLOR_TEXTO);
                gotoxy(5, 7); cout << "Se eliminara definitivamente el registro del " << hallado->fechaEvento.dia << "/" << hallado->fechaEvento.mes << "/" << hallado->fechaEvento.anio;
                gotoxy(5, 8); cout << "Tipo: " << tipoEventoStr(hallado->tipo) << " | " << hallado->descripcion;
                
                string conf = inputTexto("Esta seguro? (S/N): ", 11);
                if (conf == "S" || conf == "s") {
                    eliminarRegistro(exp, hallado->fechaEvento);
                    mostrarMensajeExito("Registro eliminado exitosamente (Liberacion de Memoria).");
                } else {
                    mostrarMensajeExito("Operacion cancelada.");
                }
            }
        } 
        else if (sel == 4) {
            break; // Volver
        }
    }
}

void controladorHistorias() {
    while (true) {
        int totalOps = cantExpedientes + 3;
        string* ops = new string[totalOps];
        
        ops[0] = "Buscar Paciente por Cedula";
        
        for(int i = 0; i < cantExpedientes; i++) {
            ops[i + 1] = "Abrir Expediente: " + dbExpedientes[i]->datosPaciente.nombre + " (" + dbExpedientes[i]->datosPaciente.cedula + ")";
        }
        
        ops[totalOps - 2] = ""; 
        ops[totalOps - 1] = "Volver al Menu Principal";

        int sel = menuInteractivo("HISTORIAS CLINICAS", ops, totalOps);

        if (sel == 0) {
            limpiarAreaTrabajo();
            string ced = inputTexto("Ingrese Cedula a Consultar: ", 10);
            Expediente* exp = buscarExpedienteSoloCedula(ced);
            
            if (exp) subMenuHistoria(exp); // Ahora abre el submenú en lugar de solo escupir la info
            else mostrarMensajeError("Paciente no encontrado en el sistema.");
        } 
        else if (sel == totalOps - 1) {
            delete[] ops; 
            return;
        } 
        else if (ops[sel] != "") { 
            Expediente* exp = dbExpedientes[sel - 1];
            subMenuHistoria(exp); // Ahora abre el submenú en lugar de solo escupir la info
        }
        
        delete[] ops; 
    }
}

// Controlador de Citas: Gestiona la agenda de citas médicas utilizando una tabla hash para almacenar las citas programadas. Permite agendar nuevas citas, visualizar la agenda completa y cancelar citas existentes. Al agendar una cita, se verifica que no haya conflictos de horario para el médico seleccionado ni para el paciente. La función de visualización muestra las citas ordenadas por fecha y hora, y ofrece opciones para filtrar por médico o paciente. El controlador también se encarga de registrar las citas en el expediente del paciente correspondiente.
void controladorCitas() {
    string listaMedicos[] = {
        "Dr. Gregory House (Diagnostico Medico)",
        "Dra. Meredith Grey (Cirugia General)",
        "Dr. Stephen Strange (Neurologia)",
        "Dra. Ana Polo (Medicina Familiar)",
        "Dr. John Watson (Medicina Interna)",
        "", 
        "Cancelar y Volver al Menu" 
    };
    int cantMedicosOps = 7;

    string nombresMedicos[] = {
        "Dr. Gregory House", 
        "Dra. Meredith Grey", 
        "Dr. Stephen Strange", 
        "Dra. Ana Polo", 
        "Dr. John Watson"
    };

    // NUEVO: Para cumplir con la estadística por "Especialidad"
    string especialidadesMedicos[] = {
        "Diagnostico Medico", 
        "Cirugia General", 
        "Neurologia", 
        "Medicina Familiar", 
        "Medicina Interna"
    };

    while(true) {
        string ops[] = {"Agendar Cita", "Consultar Agenda / Filtros", "Cancelar Cita", "Estadisticas", "Volver"};
        int cantOps = 5;
        
        int sel = menuInteractivo("CONSULTAS Y CITAS", ops, cantOps);
        
        if(sel == 0) { // --- AGENDAR CITA ---
            limpiarAreaTrabajo();
            gotoxy(5, 5); color(COLOR_TITULO); cout << "--- PASO 1: IDENTIFICACION ---"; color(COLOR_TEXTO);
            
            Cita c;
            c.idPaciente = inputTexto("Cedula: ", 7);
            c.nombrePaciente = inputTexto("Nombre: ", 9);
            c.motivo = inputTexto("Motivo Consulta: ", 11);

            int selMedico = menuInteractivo("PASO 2: SELECCIONE ESPECIALISTA", listaMedicos, cantMedicosOps);
            if (selMedico == cantMedicosOps - 1) continue; 
            
            c.nombreMedico = nombresMedicos[selMedico];
            c.especialidad = especialidadesMedicos[selMedico]; // Guardamos la especialidad

            limpiarAreaTrabajo();
            gotoxy(5, 5); color(COLOR_TITULO); cout << "--- PASO 3: FECHA Y HORA ---"; 
            gotoxy(5, 7); color(COLOR_EXITO); 
            cout << "Paciente: " << c.nombrePaciente << " | Medico: " << c.nombreMedico; 
            color(COLOR_TEXTO);
            
            c.fecha.dia = inputNumero("Dia (1-31): ", 9, 1, 31); 
            c.fecha.mes = inputNumero("Mes (1-12): ", 11, 1, 12);
            c.fecha.anio = 2026; 
            c.hora.hora = inputNumero("Hora (0-23): ", 13, 0, 23); 
            c.hora.minuto = 0;
            c.activa = true;

            if (existeChoqueHorario(agendaCitas, c.nombreMedico, c.fecha, c.hora)) {
                mostrarMensajeError("Choque de horario: El medico esta ocupado a esa hora.");
            } 
            else if (pacienteTieneCita(agendaCitas, c.idPaciente, c.fecha, c.hora)) {
                mostrarMensajeError("Choque de horario: El paciente ya tiene otra cita a esa hora.");
            } 
            else {
                registrarCita(agendaCitas, c);
                
                Paciente pDummy; pDummy.cedula = c.idPaciente; pDummy.nombre = c.nombrePaciente;
                pDummy.urgencia = LEVE; pDummy.numLlegada = 0;
                obtenerExpediente(pDummy);
                
                mostrarMensajeExito("Cita Agendada Exitosamente.");
            }
        }
        else if(sel == 1) { // --- CONSULTAR (FILTROS) ---
            string opsFiltro[] = {
                "Ver Toda la Agenda (General)",
                "Filtrar por Paciente (Cedula)",
                "Filtrar por Especialidad",
                "Filtrar por Fecha",
                "Volver"
            };
            int fSel = menuInteractivo("TIPO DE CONSULTA", opsFiltro, 5);
            
            if (fSel == 0) {
                limpiarAreaTrabajo();
                mostrarTablaHash(agendaCitas);
                pausarInterfaz();
            } 
            else if (fSel == 1) {
                limpiarAreaTrabajo();
                gotoxy(5, 5); color(COLOR_TITULO); cout << "--- FILTRO POR CEDULA ---"; color(COLOR_TEXTO);
                string busqCed = inputTexto("Ingrese la Cedula: ", 7);
                consultarCitasFiltradas(agendaCitas, 1, busqCed, {0,0,0});
                pausarInterfaz();
            }
            else if (fSel == 2) {
                int espSel = menuInteractivo("SELECCIONE ESPECIALIDAD", especialidadesMedicos, 5);
                consultarCitasFiltradas(agendaCitas, 2, especialidadesMedicos[espSel], {0,0,0});
                pausarInterfaz();
            }
            else if (fSel == 3) {
                limpiarAreaTrabajo();
                gotoxy(5, 5); color(COLOR_TITULO); cout << "--- FILTRO POR FECHA ---"; color(COLOR_TEXTO);
                int d = inputNumero("Dia (1-31): ", 7, 1, 31);
                int m = inputNumero("Mes (1-12): ", 9, 1, 12);
                int a = inputNumero("Anio: ", 11, 2020, 2100);
                consultarCitasFiltradas(agendaCitas, 3, "", {d, m, a});
                pausarInterfaz();
            }
        }
        else if(sel == 2) { // --- CANCELAR CITA ---
            while (true) {
                int maxCitas = agendaCitas.cantidadCitas;
                if (maxCitas == 0) {
                    mostrarMensajeError("No hay citas registradas en el sistema.");
                    break;
                }

                Cita* citasLista = new Cita[maxCitas];
                int totalActivas = 0;

                for (int i = 0; i < TAM_TABLA; i++) {
                    NodoCita* actual = agendaCitas.buckets[i];
                    while (actual != NULL) {
                        if (actual->datos.activa == true) { 
                            citasLista[totalActivas] = actual->datos;
                            totalActivas++;
                        }
                        actual = actual->siguiente;
                    }
                }

                if (totalActivas == 0) {
                    delete[] citasLista;
                    mostrarMensajeError("No hay citas ACTIVAS para cancelar.");
                    break;
                }

                // Burbuja
                for (int i = 0; i < totalActivas - 1; i++) {
                    for (int j = 0; j < totalActivas - i - 1; j++) {
                        Cita a = citasLista[j];
                        Cita b = citasLista[j+1];
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
                            Cita temp = citasLista[j];
                            citasLista[j] = citasLista[j+1];
                            citasLista[j+1] = temp;
                        }
                    }
                }

                int cantOpsCancel = totalActivas + 3; 
                string* opsCancel = new string[cantOpsCancel];
                
                opsCancel[0] = "Buscar Cita Manualmente (Por Cedula y Fecha)";
                
                for(int i = 0; i < totalActivas; i++) {
                    Cita c = citasLista[i];
                    string diaStr = (c.fecha.dia < 10 ? "0" : "") + to_string(c.fecha.dia);
                    string mesStr = (c.fecha.mes < 10 ? "0" : "") + to_string(c.fecha.mes);
                    string horaStr = (c.hora.hora < 10 ? "0" : "") + to_string(c.hora.hora) + ":00";
                    string nombre = c.nombrePaciente;
                    if(nombre.length() > 15) nombre = nombre.substr(0, 15) + "."; 
                    
                    opsCancel[i + 1] = "Cancelar: " + diaStr + "/" + mesStr + " " + horaStr + " | " + c.nombreMedico + " - " + nombre;
                }

                opsCancel[totalActivas + 1] = ""; 
                opsCancel[totalActivas + 2] = "Volver";

                int selC = menuInteractivo("SELECCIONE CITA A CANCELAR", opsCancel, cantOpsCancel);

                Cita cToCancel;
                if (selC > 0 && selC < cantOpsCancel - 2) { 
                    cToCancel = citasLista[selC - 1];
                }
                
                delete[] opsCancel;  
                delete[] citasLista; 

                if (selC == 0) {
                    limpiarAreaTrabajo();
                    gotoxy(5, 5); color(COLOR_TITULO); cout << "--- CANCELAR CITA (MANUAL) ---"; color(COLOR_TEXTO);
                    string ced = inputTexto("Cedula del Paciente: ", 7);
                    int dia = inputNumero("Dia de la cita (1-31): ", 9, 1, 31);
                    int mes = inputNumero("Mes de la cita (1-12): ", 11, 1, 12);
                    int hora = inputNumero("Hora de la cita (0-23): ", 13, 0, 23);
                    
                    if(cancelarCita(agendaCitas, ced, {dia, mes, 2026}, {hora, 0})) {
                        mostrarMensajeExito("Cita cancelada correctamente (Borrado Logico).");
                    } else {
                        mostrarMensajeError("No se encontro una cita activa con esos datos.");
                    }
                }
                else if (selC == cantOpsCancel - 1) {
                    break; 
                }
                else {
                    limpiarAreaTrabajo();
                    gotoxy(5, 5); color(COLOR_ALERTA); cout << "--- CONFIRMACION DE CANCELACION ---"; color(COLOR_TEXTO);
                    gotoxy(5, 7); cout << "Paciente: " << cToCancel.nombrePaciente;
                    gotoxy(5, 8); cout << "Medico: " << cToCancel.nombreMedico;
                    gotoxy(5, 9); cout << "Fecha: " << cToCancel.fecha.dia << "/" << cToCancel.fecha.mes << " a las " << cToCancel.hora.hora << ":00";
                    
                    string confirmacion = inputTexto("¿Esta seguro de cancelar esta cita? (S/N): ", 11);
                    
                    if (confirmacion == "S" || confirmacion == "s") {
                        if(cancelarCita(agendaCitas, cToCancel.idPaciente, cToCancel.fecha, cToCancel.hora)) {
                            mostrarMensajeExito("Cita cancelada (Borrado logico).");
                        } else {
                            mostrarMensajeError("Error interno al cancelar.");
                        }
                    } else {
                        mostrarMensajeExito("Operacion abortada.");
                    }
                }
            }
        }
        else if (sel == 3) { // --- ESTADISTICAS ---
            limpiarAreaTrabajo();
            gotoxy(5, 5); color(COLOR_TITULO); cout << "--- ESTADISTICAS DE AGENDA MEDICA ---"; color(COLOR_TEXTO);
            
            int d = inputNumero("Filtrar por dia (0 para ver global total): ", 7, 0, 31);
            int m = 0, a = 0;
            if (d != 0) {
                m = inputNumero("Mes (1-12): ", 9, 1, 12);
                a = inputNumero("Anio: ", 11, 2020, 2100);
            }
            
            string opsEsp[6] = {
                "TODAS LAS ESPECIALIDADES",
                "Diagnostico Medico", 
                "Cirugia General", 
                "Neurologia", 
                "Medicina Familiar", 
                "Medicina Interna"
            };
            
            int espSel = menuInteractivo("FILTRO POR ESPECIALIDAD", opsEsp, 6);
            
            mostrarEstadisticasCitas(agendaCitas, {d, m, a}, opsEsp[espSel]);
            pausarInterfaz();
        }
        else return;
    }
}

// Controlador de Hospitalización: Administra la ocupación de camas en el hospital utilizando una matriz dispersa para representar los pisos y habitaciones. Permite visualizar el mapa de camas ocupadas, consultar la disponibilidad general y por piso, y buscar el estado de una habitación específica. Además, ofrece la funcionalidad de dar de alta a un paciente (liberar una cama) con confirmación crítica y registro del motivo e indicaciones. El controlador también incluye estadísticas de ocupación para evaluar la eficiencia del uso de camas en el hospital.
void controladorHospital() {
    while(true) {
        string ops[] = {
            "Ver Mapa de Camas Ocupadas", 
            "Ver Disponibilidad (General y por Piso)", 
            "Consultar Habitacion Especifica",
            "DAR DE ALTA (Liberar Cama)",
            "Estadisticas de Ocupacion",
            "Volver"
        };
        int cantOps = 6; 

        int sel = menuInteractivo("ADMINISTRACION DE HOSPITALIZACION", ops, cantOps);

        if (sel == 0) { // MAPA DE CAMAS
            limpiarAreaTrabajo();
            mostrarHabitacionesOcupadas(hospitalCentral);
            pausarInterfaz();
        }
        else if (sel == 1) { // DISPONIBILIDAD GENERAL
            limpiarAreaTrabajo();
            int y = 5;
            gotoxy(4, y++); color(COLOR_TITULO); cout << "=== REPORTE DE DISPONIBILIDAD ==="; color(COLOR_TEXTO);
            y++;
            
            int total = MAX_PISOS * MAX_HABITACIONES;
            int ocupadas = hospitalCentral.cantidadPacientes;
            int libres = total - ocupadas;
            float porcentaje = (float)ocupadas / total * 100;

            gotoxy(4, y++); cout << "Total de Habitaciones: " << total;
            gotoxy(4, y++); cout << "Habitaciones Ocupadas: " << ocupadas << " (" << (int)porcentaje << "%)";
            gotoxy(4, y++); cout << "Habitaciones Libres:   " << libres;
            y++;
            
            gotoxy(4, y++); color(COLOR_PANEL); cout << ">> DISTRIBUCION POR PISOS:"; color(COLOR_TEXTO);
            for (int p = 1; p <= MAX_PISOS; p++) {
                int countOcup = 0;
                NodoMatriz* aux = hospitalCentral.filas[p - 1];
                while(aux != NULL) { countOcup++; aux = aux->derecha; }
                
                gotoxy(4, y++); 
                cout << "Piso " << p << ": " << countOcup << " ocupadas | " 
                     << (MAX_HABITACIONES - countOcup) << " libres.";
            }
            pausarInterfaz();
        }
        else if (sel == 2) { // CONSULTA ESPECÍFICA
            limpiarAreaTrabajo();
            gotoxy(5, 5); color(COLOR_TITULO); cout << "--- CONSULTAR HABITACION ---"; color(COLOR_TEXTO);
            int p = inputNumero("Piso (1-5): ", 7, 1, 5);
            int h = inputNumero("Habitacion (1-10): ", 9, 1, 10);
            
            NodoMatriz* cama = buscarHabitacion(hospitalCentral, p, h);
            limpiarAreaTrabajo();
            gotoxy(5, 5); color(COLOR_TITULO); cout << "--- ESTADO DE LA HABITACION " << p << "-" << h << " ---"; color(COLOR_TEXTO);
            
            if(cama != NULL) {
                gotoxy(5, 7); color(COLOR_ALERTA); cout << "[ OCUPADA ]"; color(COLOR_TEXTO);
                gotoxy(5, 9); cout << "Paciente: " << cama->paciente.nombre << " (CI: " << cama->paciente.cedula << ")";
                gotoxy(5, 10); cout << "Ingreso:  " << cama->fechaIngreso.dia << "/" << cama->fechaIngreso.mes << "/" << cama->fechaIngreso.anio;
                gotoxy(5, 11); cout << "Prioridad Medica: " << cama->paciente.urgencia;
            } else {
                gotoxy(5, 7); color(COLOR_EXITO); cout << "[ DISPONIBLE ]"; color(COLOR_TEXTO);
                gotoxy(5, 9); color(8); cout << "Lista para recibir a un nuevo paciente."; color(COLOR_TEXTO);
            }
            pausarInterfaz();
        }
        else if (sel == 3) { // DAR DE ALTA
            limpiarAreaTrabajo();
            gotoxy(5, 5); color(COLOR_TITULO); cout << "--- PROCESO DE ALTA ---"; color(COLOR_TEXTO);
            int p = inputNumero("Piso (1-5): ", 7, 1, 5);
            int h = inputNumero("Habitacion (1-10): ", 9, 1, 10);
            
            NodoMatriz* cama = buscarHabitacion(hospitalCentral, p, h);
            
            if(cama != NULL) {
                // PDF Req 7: Confirmación Crítica
                gotoxy(5, 11); color(COLOR_ALERTA); cout << "Paciente actual: " << cama->paciente.nombre; color(COLOR_TEXTO);
                string conf = inputTexto("Confirmar alta medica y liberar cama? (S/N): ", 13);
                
                if (conf == "S" || conf == "s") {
                    string motivo = inputTexto("Motivo del Alta: ", 15);
                    string indic = inputTexto("Indicaciones: ", 17);
                    
                    // Fecha simulada (En producción sería fecha del sistema)
                    Fecha hoy = {20, 2, 2026};
                    
                    Expediente* exp = buscarExpedienteSoloCedula(cama->paciente.cedula);
                    if(exp) agregarAlta(exp, hoy, motivo, indic);
                    
                    // Registro Histórico para las estadísticas
                    if (cantEgresos < MAX_EGRESOS) {
                        historialEgresos[cantEgresos++] = {hoy, p, h};
                    }
                    
                    darAltaPaciente(hospitalCentral, p, h);
                    mostrarMensajeExito("Paciente egresado y cama liberada de la matriz.");
                } else {
                    mostrarMensajeExito("Operacion cancelada.");
                }
            } else {
                mostrarMensajeError("No se puede dar de alta: La habitacion ya esta vacia.");
            }
        }
        else if (sel == 4) { // ESTADISTICAS
            limpiarAreaTrabajo();
            gotoxy(5, 5); color(COLOR_TITULO); cout << "--- ESTADISTICAS DE HOSPITALIZACION ---"; color(COLOR_TEXTO);
            
            int d = inputNumero("Filtrar ocupacion por dia (0 para ver global): ", 7, 0, 31);
            int m = 0, a = 0;
            if (d != 0) {
                m = inputNumero("Mes (1-12): ", 8, 1, 12);
                a = inputNumero("Anio (Ej. 2026): ", 9, 2020, 2100);
            }

            int contOcupadas = 0;
            int contCritico = 0, contUrgente = 0, contModerado = 0, contLeve = 0;

            // Recorrer la matriz actual
            for (int i = 0; i < MAX_PISOS; i++) {
                NodoMatriz* aux = hospitalCentral.filas[i];
                while(aux != NULL) {
                    bool match = true;
                    if (d != 0 && (aux->fechaIngreso.dia != d || aux->fechaIngreso.mes != m || aux->fechaIngreso.anio != a)) {
                        match = false;
                    }
                    
                    if (match) {
                        contOcupadas++;
                        if (aux->paciente.urgencia == CRITICO) contCritico++;
                        else if (aux->paciente.urgencia == URGENTE) contUrgente++;
                        else if (aux->paciente.urgencia == MODERADO) contModerado++;
                        else if (aux->paciente.urgencia == LEVE) contLeve++;
                    }
                    aux = aux->derecha;
                }
            }

            // Recorrer el historial de altas
            int contEgresosHoy = 0;
            for(int i = 0; i < cantEgresos; i++) {
                if (d == 0 || (historialEgresos[i].fecha.dia == d && historialEgresos[i].fecha.mes == m && historialEgresos[i].fecha.anio == a)) {
                    contEgresosHoy++;
                }
            }

            limpiarAreaTrabajo();
            int y = 5;
            gotoxy(5, y++); color(COLOR_TITULO); cout << "=== REPORTE ESTADISTICO ==="; color(COLOR_TEXTO);
            y++;
            if (d == 0) { gotoxy(5, y++); color(8); cout << "[ Filtro: HISTORICO COMPLETO ]"; color(COLOR_TEXTO); }
            else { gotoxy(5, y++); color(8); cout << "[ Filtro: " << d << "/" << m << "/" << a << " ]"; color(COLOR_TEXTO); }
            y++;
            
            gotoxy(5, y++); color(COLOR_PANEL); cout << ">> CAMAS ACTUALMENTE OCUPADAS: " << contOcupadas; color(COLOR_TEXTO);
            gotoxy(5, y++); cout << "Desglose por Prioridad del Paciente:";
            gotoxy(5, y++); cout << "- Criticos:  " << contCritico;
            gotoxy(5, y++); cout << "- Urgentes:  " << contUrgente;
            gotoxy(5, y++); cout << "- Moderados: " << contModerado;
            gotoxy(5, y++); cout << "- Leves:     " << contLeve;
            y++;
            
            gotoxy(5, y++); color(COLOR_PANEL); cout << ">> MOVIMIENTO DE CAMAS:"; color(COLOR_TEXTO);
            gotoxy(5, y++); cout << "Camas liberadas (Egresos) en la fecha: " << contEgresosHoy;
            
            pausarInterfaz();
        }
        else return;
    }
}


// Controlador de Laboratorio Clínico: Gestiona las solicitudes de análisis clínicos utilizando una cola para las muestras pendientes y una pila para los resultados procesados. Permite registrar nuevas solicitudes de examen, procesar las muestras en orden de llegada, y visualizar el historial de resultados. Además, ofrece estadísticas sobre los tipos de exámenes realizados y su frecuencia. Al procesar una muestra, se registra el resultado en el expediente del paciente correspondiente, asegurando un seguimiento completo de su historia clínica.
void controladorLaboratorio() {
    while(true) {
        string ops[] = {
            "Registrar Solicitud (Manual)", 
            "PROCESAR MUESTRA (Cola -> Pila)", 
            "Ver Historial Resultados (PILA LIFO)", 
            "Estadisticas de Examenes",
            "Volver"
        };
        int cantOps = 5; 
        
        int sel = menuInteractivo("LABORATORIO CLINICO", ops, cantOps);

        if(sel == 0) { // MANUAL
            limpiarAreaTrabajo();
            gotoxy(5, 5); color(COLOR_TITULO); cout << "--- NUEVA SOLICITUD DE EXAMEN ---"; color(COLOR_TEXTO);
            
            Examen ex;
            ex.cedulaPaciente = inputTexto("Cedula: ", 7);
            ex.nombrePaciente = inputTexto("Nombre: ", 9);
            
            gotoxy(5, 11); color(8); cout << "0:Hema | 1:Quim | 2:Imag | 3:Orina | 4:Heces"; color(COLOR_TEXTO);
            int t = inputNumero("Tipo (0-4): ", 12, 0, 4);
            ex.tipo = (TipoExamen)t;
            
            int dia = inputNumero("Dia de solicitud (1-31): ", 14, 1, 31);
            int mes = inputNumero("Mes (1-12): ", 15, 1, 12);
            int anio = inputNumero("Anio: ", 16, 2020, 2100);
            ex.fechaSolicitud = {dia, mes, anio};
            
            ex.procesado = false;
            
            Paciente pDummy; pDummy.cedula = ex.cedulaPaciente; pDummy.nombre = ex.nombrePaciente;
            obtenerExpediente(pDummy);

            if(encolarSolicitud(colaLab, ex)) {
                mostrarMensajeExito("Solicitud encolada correctamente.");
            } else {
                mostrarMensajeError("No se pudo encolar (Cola llena o Paciente duplicado).");
            }
        }
        else if(sel == 1) { // PROCESAR 
            if(colaEstaVacia(colaLab)) {
                mostrarMensajeError("No hay muestras pendientes en la Cola.");
            } else {
                limpiarAreaTrabajo();
                Examen* siguiente = &colaLab.ultimo->siguiente->datos;
                
                gotoxy(5, 5); color(COLOR_ALERTA); cout << ">>> PROCESANDO MUESTRA <<<"; color(COLOR_TEXTO);
                gotoxy(5, 7); cout << "Paciente: " << siguiente->nombrePaciente;
                gotoxy(5, 8); cout << "Examen:   " << obtenerNombreExamen(siguiente->tipo);
                
                string res = inputTexto("Resultado del Analisis: ", 10);
                
                string cedulaTemp = siguiente->cedulaPaciente;
                TipoExamen tipoTemp = siguiente->tipo;
                Fecha fechaTemp = siguiente->fechaSolicitud;

                procesarExamen(colaLab, pilaResultados, res);
                
                Expediente* exp = buscarExpedienteSoloCedula(cedulaTemp); 
                if(exp) {
                    agregarResultadoLab(exp, fechaTemp, obtenerNombreExamen(tipoTemp), res);
                    mostrarMensajeExito("Examen procesado a la Pila y guardado en su Historia Clinica.");
                } else {
                    mostrarMensajeExito("Examen procesado a la Pila (Paciente sin historia previa).");
                }
            }
        }
        else if(sel == 2) { // MOSTRAR PILA
            limpiarAreaTrabajo();
            mostrarPila(pilaResultados);
            pausarInterfaz();
        }
        else if(sel == 3) { // ESTADISTICAS
            limpiarAreaTrabajo();
            gotoxy(5, 5); color(COLOR_TITULO); cout << "--- ESTADISTICAS DE LABORATORIO ---"; color(COLOR_TEXTO);
            
            int d = inputNumero("Filtrar por dia (0 para ver global total): ", 7, 0, 31);
            int m = 0, a = 0;
            if (d != 0) {
                m = inputNumero("Mes (1-12): ", 9, 1, 12);
                a = inputNumero("Anio: ", 11, 2020, 2100);
            }
            
            string opsTipos[7] = {
                "TODOS LOS EXAMENES",
                "Hematologia", 
                "Quimica", 
                "Imagenologia", 
                "Uroanalisis", 
                "Coproanalisis"
            };
            
            int tipoSel = menuInteractivo("FILTRO POR TIPO DE EXAMEN", opsTipos, 6);
            
            // Pasamos el tipo exacto (restando 1 porque el 0 es "TODOS")
            mostrarEstadisticasLaboratorio(pilaResultados, d, m, a, tipoSel - 1);
            pausarInterfaz();
        }
        else return;
    }
}

// Dummy data 
void cargarDatosPrueba() {
    // 1. PACIENTES EN TRIAJE (Árbol AVL) - Todos los niveles de Urgencia
    Paciente p1; p1.cedula = "V-11111111"; p1.nombre = "Ana Gabriel"; p1.motivoConsulta = "Paro cardiaco"; p1.urgencia = CRITICO; p1.numLlegada = contadorLlegadasGlobal++;
    insertarPaciente(colaTriaje, p1); obtenerExpediente(p1);

    Paciente p2; p2.cedula = "V-22222222"; p2.nombre = "Simon Diaz"; p2.motivoConsulta = "Hemorragia interna"; p2.urgencia = URGENTE; p2.numLlegada = contadorLlegadasGlobal++;
    insertarPaciente(colaTriaje, p2); obtenerExpediente(p2);

    Paciente p3; p3.cedula = "V-33333333"; p3.nombre = "Celia Cruz"; p3.motivoConsulta = "Dolor abdominal agudo"; p3.urgencia = MODERADO; p3.numLlegada = contadorLlegadasGlobal++;
    insertarPaciente(colaTriaje, p3); obtenerExpediente(p3);

    Paciente p4; p4.cedula = "V-44444444"; p4.nombre = "Oscar D Leon"; p4.motivoConsulta = "Dolor de cabeza"; p4.urgencia = LEVE; p4.numLlegada = contadorLlegadasGlobal++;
    insertarPaciente(colaTriaje, p4); obtenerExpediente(p4);

    // 2. PACIENTES HOSPITALIZADOS (Matriz Dispersa) - Diversos Tipos de Habitación
    Paciente p5; p5.cedula = "V-55555555"; p5.nombre = "Hector Lavoe"; p5.motivoConsulta = "Post-operatorio"; p5.urgencia = LEVE; p5.numLlegada = 0;
    Expediente* exp5 = obtenerExpediente(p5);
    ingresarPaciente(hospitalCentral, p5, {15, 2, 2026}, CUIDADOS_ESPECIALES, 1, 1);
    agregarIngreso(exp5, {15, 2, 2026}, "Piso 1 - Hab 1", "Recuperacion de cirugia a corazon abierto");

    Paciente p6; p6.cedula = "V-66666666"; p6.nombre = "Ruben Blades"; p6.motivoConsulta = "Neumonia"; p6.urgencia = URGENTE; p6.numLlegada = 0;
    Expediente* exp6 = obtenerExpediente(p6);
    ingresarPaciente(hospitalCentral, p6, {18, 2, 2026}, AISLAMIENTO, 3, 4);
    agregarIngreso(exp6, {18, 2, 2026}, "Piso 3 - Hab 4", "Aislamiento por enfermedad respiratoria infecciosa");

    Paciente p7; p7.cedula = "V-77777777"; p7.nombre = "Willie Colon"; p7.motivoConsulta = "Fractura de pierna"; p7.urgencia = MODERADO; p7.numLlegada = 0;
    Expediente* exp7 = obtenerExpediente(p7);
    ingresarPaciente(hospitalCentral, p7, {20, 2, 2026}, DOBLE, 2, 5);
    agregarIngreso(exp7, {20, 2, 2026}, "Piso 2 - Hab 5", "Observacion traumatologica");

    // 3. HISTORIA CLÍNICA RICA (Paciente ya dado de alta - Ideal para probar búsqueda recursiva)
    Paciente p8; p8.cedula = "V-88888888"; p8.nombre = "Tito Puente"; p8.urgencia = LEVE; p8.numLlegada = 0;
    Expediente* exp8 = obtenerExpediente(p8);
    agregarConsulta(exp8, {10, 1, 2026}, "Infeccion Viral", "Antivirales y reposo", "Paciente presenta fiebre alta");
    agregarResultadoLab(exp8, {11, 1, 2026}, "Hematologia", "Leucocitos altos (Infeccion)");
    agregarAlta(exp8, {15, 1, 2026}, "Recuperacion total", "Tomar mucha agua");

    // 4. CITAS MÉDICAS (Tabla Hash) - Activas y Canceladas (Borrado Lógico)
    Paciente p9; p9.cedula = "V-99999999"; p9.nombre = "Marc Anthony"; p9.urgencia = LEVE; p9.numLlegada = 0;
    obtenerExpediente(p9);
    
    Cita c1; 
    c1.idPaciente = "V-99999999"; c1.nombrePaciente = "Marc Anthony"; 
    c1.nombreMedico = "Dr. Gregory House"; c1.especialidad = "Diagnostico Medico"; c1.motivo = "Chequeo General";
    c1.fecha = {25, 2, 2026}; c1.hora = {10, 0}; c1.activa = true; 
    registrarCita(agendaCitas, c1);

    Paciente p10; p10.cedula = "V-10101010"; p10.nombre = "Gilberto S. Rosa"; p10.urgencia = LEVE; p10.numLlegada = 0;
    obtenerExpediente(p10);
    
    Cita c2;
    c2.idPaciente = "V-10101010"; c2.nombrePaciente = "Gilberto S. Rosa"; 
    c2.nombreMedico = "Dra. Meredith Grey"; c2.especialidad = "Cirugia General"; c2.motivo = "Evaluacion pre-operatoria";
    c2.fecha = {26, 2, 2026}; c2.hora = {14, 0}; c2.activa = true;
    registrarCita(agendaCitas, c2);

    // Cita Cancelada para visualizar el estado CANCEL en Agenda y Estadísticas
    Cita c3;
    c3.idPaciente = "V-88888888"; c3.nombrePaciente = "Tito Puente"; 
    c3.nombreMedico = "Dr. Stephen Strange"; c3.especialidad = "Neurologia"; c3.motivo = "Migranas frecuentes";
    c3.fecha = {20, 2, 2026}; c3.hora = {9, 0}; c3.activa = false; // IMPORTANTE: False = Borrado Lógico
    registrarCita(agendaCitas, c3);

    // 5. LABORATORIO CLÍNICO (Cola y Pila)
    // En cola (Pendientes)
    Examen ex1;
    ex1.cedulaPaciente = "V-33333333"; ex1.nombrePaciente = "Celia Cruz"; ex1.tipo = HEMATOLOGIA; ex1.fechaSolicitud = {20, 2, 2026}; ex1.procesado = false; ex1.resultado = "PENDIENTE";
    encolarSolicitud(colaLab, ex1);

    Examen ex2;
    ex2.cedulaPaciente = "V-44444444"; ex2.nombrePaciente = "Oscar D Leon"; ex2.tipo = ORINA; ex2.fechaSolicitud = {20, 2, 2026}; ex2.procesado = false; ex2.resultado = "PENDIENTE";
    encolarSolicitud(colaLab, ex2);

    // Procesados (En Pila de resultados) - Los encolamos y procesamos mágicamente en código
    Examen ex3;
    ex3.cedulaPaciente = "V-55555555"; ex3.nombrePaciente = "Hector Lavoe"; ex3.tipo = QUIMICA; ex3.fechaSolicitud = {19, 2, 2026}; ex3.procesado = false; ex3.resultado = "PENDIENTE";
    encolarSolicitud(colaLab, ex3);
    procesarExamen(colaLab, pilaResultados, "Glucosa 95 mg/dL, Colesterol 180 mg/dL"); // Automáticamente va a la Pila
    
    Examen ex4;
    ex4.cedulaPaciente = "V-66666666"; ex4.nombrePaciente = "Ruben Blades"; ex4.tipo = IMAGENOLOGIA; ex4.fechaSolicitud = {19, 2, 2026}; ex4.procesado = false; ex4.resultado = "PENDIENTE";
    encolarSolicitud(colaLab, ex4);
    procesarExamen(colaLab, pilaResultados, "Radiografia de torax: Leve opacidad pulmonar");
}

int main() {
    ocultarCursor();

    // configurarVentana();
    
    SetConsoleTitleA("SISTEMA HOSPITALARIO V3.0 (FINAL)"); // SetConsoleTitleA para evitar error en VSCode
    srand(time(0));

    inicializarCola(colaLab);
    inicializarPila(pilaResultados);
    inicializarTabla(agendaCitas);
    inicializarHospital(hospitalCentral);

    // LUIS RECUERDA CARGAR LOS DATOS DE PRUEBA ANTES DE INICIAR EL PANEL DE CONTROL, DESCOMENTA LA SIGUIENTE LINEA
    cargarDatosPrueba();

    // grafica
    encabezado(); 

    while (true) {
        // REEMPLAZO FINAL DE VECTOR POR ARREGLO ESTÁTICO
        string menu[] = {
            "TRIAJE (Emergencias -> Ingresos)",
            "HISTORIAS CLINICAS (Consultar)",
            "CITAS MEDICAS (Hash Table)",
            "HOSPITALIZACION (Pisos y Camas)",
            "LABORATORIO (Colas y Resultados)",
            "SALIR"
        };
        int cantOps = 6; // Tamaño del menú principal

        // Llamada final al motor gráfico
        int sel = menuInteractivo("PANEL DE CONTROL GENERAL", menu, cantOps);

        switch (sel) {
            case 0: controladorTriaje(); break;
            case 1: controladorHistorias(); break;
            case 2: controladorCitas(); break;
            case 3: controladorHospital(); break;
            case 4: controladorLaboratorio(); break;
            case 5: 
                // LIMPIA TODA LA CONSOLA AL SALIR (Marcos, dashboard, etc.)
                system("cls"); 
                gotoxy(2, 2); 
                color(COLOR_TITULO);
                cout << "Cerrando Sistema Integral de Gestion Hospitalaria (S.I.G.H.)...";
                gotoxy(2, 4);
                color(COLOR_EXITO);
                cout << "¡Hasta pronto!\n\n";
                color(COLOR_TEXTO);
                mostrarCursor(); // Devolvemos el cursor a la normalidad para la terminal
                return 0;
        }
    }
    return 0;
}