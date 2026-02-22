#ifndef INTERFAZ_H
#define INTERFAZ_H

// CONFIGURACIÓN DE COMPILACIÓN (Soluciona errores de Byte y Títulos)
#define WIN32_LEAN_AND_MEAN 
#include <windows.h>

#include <iostream>
#include <conio.h>
#include <string> 
#include "estructuras.h"
#include "AVL.h"      
#include "hospital.h" 
#include "citas.h" 

using namespace std;

// --- COLORES ---
#define COLOR_FONDO 0
#define COLOR_TEXTO 7
#define COLOR_TITULO 11 
#define COLOR_SELECCION 240 
#define COLOR_ALERTA 12 
#define COLOR_EXITO 10  
#define COLOR_PANEL 3   
#define COLOR_MARCO 15

// --- VARIABLES GLOBALES EXTERNAS ---
extern Nodo* colaTriaje;
extern Hospital hospitalCentral; 
extern TablaHash agendaCitas;

// --- CONTROL DE CONSOLA ---
// --- CONTROL DE CONSOLA ---

// 1. Manejador de eventos del sistema (Para capturar Ctrl+C)
inline BOOL WINAPI manejadorCtrlC(DWORD dwCtrlType) {
    if (dwCtrlType == CTRL_C_EVENT) {
        // Lanzamos una alerta nativa de Windows (C Puro, sin STL)
        int respuesta = MessageBoxA(NULL, 
            "Esta seguro que desea cerrar el Sistema Hospitalario?", 
            "Confirmacion de Salida", 
            MB_YESNO | MB_ICONWARNING | MB_DEFBUTTON2 | MB_SETFOREGROUND);
        
        if (respuesta == IDYES) {
            ExitProcess(0); // Cierra el programa limpiamente
        }
        return TRUE; // Devuelve TRUE para evitar que la consola "explote" si elige NO
    }
    return FALSE;
}

// 2. Configuración maestra de la ventana
inline void configurarVentana() {
    // Fijamos un área de trabajo garantizada para evitar que el texto "salte" y rompa el marco
    system("mode con cols=130 lines=35");

    HWND consola = GetConsoleWindow();
    
    // Maximizar
    ShowWindow(consola, SW_SHOWMAXIMIZED);

    // Bloquear redimensionamiento (Quita el botón de maximizar y evita arrastrar los bordes)
    LONG estilo = GetWindowLong(consola, GWL_STYLE);
    estilo &= ~WS_SIZEBOX;      
    estilo &= ~WS_MAXIMIZEBOX;  
    SetWindowLong(consola, GWL_STYLE, estilo);

    // Ocultar las barras de desplazamiento lateral
    ShowScrollBar(consola, SB_BOTH, FALSE);

    // Le decimos a Windows que nosotros manejaremos el Ctrl+C
    SetConsoleCtrlHandler(manejadorCtrlC, TRUE);
}

inline void ocultarCursor() {
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 100;
    info.bVisible = FALSE;
    SetConsoleCursorInfo(consoleHandle, &info);
}

inline void mostrarCursor() {
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 20;
    info.bVisible = TRUE;
    SetConsoleCursorInfo(consoleHandle, &info);
}

inline void gotoxy(int x, int y) {
    HANDLE hcon = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD dwPos;
    dwPos.X = x;
    dwPos.Y = y;
    SetConsoleCursorPosition(hcon, dwPos);
}

inline void color(int color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}

// Limpiador reforzado para no dejar "fantasmas" en pantalla
inline void limpiarAreaTrabajo() {
    for (int y = 3; y <= 27; y++) {
        gotoxy(2, y);
        cout << string(77, ' '); // Borra estrictamente el cuadro izquierdo
    }
}
// --- DIBUJADO ---

inline void dibujarCuadro(int x1, int y1, int x2, int y2) {
    color(COLOR_MARCO);
    for (int i = x1; i <= x2; i++) {
        gotoxy(i, y1); cout << char(205);
        gotoxy(i, y2); cout << char(205);
    }
    for (int i = y1; i <= y2; i++) {
        gotoxy(x1, i); cout << char(186);
        gotoxy(x2, i); cout << char(186);
    }
    gotoxy(x1, y1); cout << char(201);
    gotoxy(x2, y1); cout << char(187);
    gotoxy(x1, y2); cout << char(200);
    gotoxy(x2, y2); cout << char(188);
    color(COLOR_TEXTO);
}

inline void encabezado() {
    system("cls");
    dibujarCuadro(0, 0, 118, 29); 
    gotoxy(2, 1); 
    color(COLOR_EXITO);
    cout << " SISTEMA INTEGRAL DE GESTION HOSPITALARIA (S.I.G.H.) v3.0 ";
    color(COLOR_TEXTO);
    
    for(int y=1; y<29; y++) {
        gotoxy(80, y); color(COLOR_MARCO); cout << char(186); 
    }
}

// --- HELPER: Recorrido AVL para Dashboard (REESCRITO CON ARREGLOS) ---
// Se pasa el arreglo y un contador por referencia para llevar el control manual
inline void recolectarPacientesDashboard(Nodo* raiz, string lista[], int& contador, int limite) {
    if (raiz == NULL || contador >= limite) return;

    // 1. Izquierda (Mayor prioridad en AVL estándar)
    recolectarPacientesDashboard(raiz->izq, lista, contador, limite);

    // 2. Raíz (Procesar si aún hay cupo)
    if (contador < limite) {
        string linea = raiz->dato.nombre;
        // Truncar nombre si es muy largo para que quepa
        if (linea.length() > 15) linea = linea.substr(0, 15) + ".";
        
        // Formato: "Nombre (Nivel: X)"
        linea += " (Nv:" + to_string(raiz->dato.urgencia) + ")";
        
        // Guardamos y aumentamos el contador manual
        lista[contador] = linea;
        contador++;
    }

    // 3. Derecha
    recolectarPacientesDashboard(raiz->der, lista, contador, limite);
}

// --- DASHBOARD ---
inline void actualizarDashboard() {
    int x = 82; 
    
    // SECCIÓN 1: TRIAJE (UCI) - LISTA DINÁMICA
    color(COLOR_PANEL);
    gotoxy(x, 2); cout << "== SALA DE ESPERA (UCI) ==";
    color(COLOR_TEXTO);
    
    // Limpiar zona de lista (10 líneas)
    for(int i=0; i<10; i++) { 
        gotoxy(x, 3+i); 
        cout << "                             "; // Borrar rastro anterior
    }

    // --- REEMPLAZO DE VECTOR POR ARREGLO PURO ---
    string pacientesTop[10]; // Arreglo estático de 10 posiciones
    int cantPacientes = 0;   // Contador manual
    
    recolectarPacientesDashboard(colaTriaje, pacientesTop, cantPacientes, 10);

    if (cantPacientes == 0) {
        gotoxy(x, 3); color(8); cout << "[ Sala Vacia ]"; color(COLOR_TEXTO);
    } else {
        for(int i = 0; i < cantPacientes; i++) {
            gotoxy(x, 3 + i);
            // Colores según urgencia (visual extra)
            if(pacientesTop[i].find("Nv:0") != string::npos) color(COLOR_ALERTA);
            else if(pacientesTop[i].find("Nv:1") != string::npos) color(14); // Amarillo
            else color(COLOR_TEXTO);
            
            cout << (i+1) << ". " << pacientesTop[i];
        }
        color(COLOR_TEXTO);
    }

    // SECCIÓN 2: CAMAS
    color(COLOR_PANEL);
    gotoxy(x, 14); cout << "== OCUPACION PISOS ==";
    color(COLOR_TEXTO);
    
    int ocupadas = hospitalCentral.cantidadPacientes;
    int total = MAX_PISOS * MAX_HABITACIONES;
    int porcentaje = (total > 0) ? (ocupadas * 100) / total : 0;

    gotoxy(x, 15); cout << "Camas: " << ocupadas << "/" << total << " (" << porcentaje << "%)";
    
    // Barra
    gotoxy(x, 16); cout << "[";
    int barras = porcentaje / 10;
    for(int i=0; i<10; i++) {
        if(i < barras) { color(COLOR_ALERTA); cout << char(219); } // Bloque sólido
        else { color(8); cout << char(176); } // Sombra
    }
    color(COLOR_TEXTO); cout << "]";

    // SECCIÓN 3: CITAS
    color(COLOR_PANEL);
    gotoxy(x, 18); cout << "== AGENDA MEDICA ==";
    color(COLOR_TEXTO);
    
    // Contar cuántas citas están verdaderamente activas
    int activasReales = 0;
    for(int i = 0; i < TAM_TABLA; i++) { 
        NodoCita* actual = agendaCitas.buckets[i];
        while(actual != NULL) {
            if(actual->datos.activa) activasReales++;
            actual = actual->siguiente;
        }
    }
    
    // Imprimir las métricas limpiando el rastro (los espacios al final son para limpiar pantalla)
    gotoxy(x, 19); cout << "Citas Activas: " << activasReales << "      ";
    gotoxy(x, 20); cout << "Canceladas:    " << (agendaCitas.cantidadCitas - activasReales) << "      ";
}

// --- MENÚ Y INPUTS ---
inline int menuInteractivo(string titulo, string opciones[], int numOpciones) {
    int seleccion = 0;
    bool elegir = false;
    char tecla;
    
    while(seleccion < numOpciones && opciones[seleccion] == "") seleccion++;

    int topeOpcionesVisuales = 7; 
    int offset = 0; 

    limpiarAreaTrabajo();
    gotoxy(4, 4); color(COLOR_TITULO); cout << ">> " << titulo; color(COLOR_TEXTO);
    dibujarCuadro(2, 3, 78, 25); 

    while(!elegir) {
        actualizarDashboard(); 

        if (seleccion < offset) offset = seleccion;
        if (seleccion >= offset + topeOpcionesVisuales) offset = seleccion - topeOpcionesVisuales + 1;

        for(int i = 0; i < topeOpcionesVisuales; i++) {
            gotoxy(8, 8 + (i * 2)); cout << string(68, ' '); 
        }

        for(int i = 0; i < topeOpcionesVisuales && (i + offset) < numOpciones; i++) {
            int idxReal = i + offset;
            gotoxy(10, 8 + (i * 2));
            
            if (opciones[idxReal] == "") continue; 
            
            if(idxReal == seleccion) {
                color(COLOR_SELECCION); cout << " > " << opciones[idxReal] << "   "; 
            } else {
                color(COLOR_TEXTO); cout << "   " << opciones[idxReal] << "   ";
            }
        }
        color(COLOR_TEXTO);

        if (offset > 0) { gotoxy(40, 6); color(COLOR_TITULO); cout << "^^^"; color(COLOR_TEXTO); }
        else { gotoxy(40, 6); cout << "   "; }
        
        if (offset + topeOpcionesVisuales < numOpciones) { gotoxy(40, 8 + (topeOpcionesVisuales * 2)); color(COLOR_TITULO); cout << "vvv"; color(COLOR_TEXTO); }
        else { gotoxy(40, 8 + (topeOpcionesVisuales * 2)); cout << "   "; }

        tecla = _getch();

        if (tecla == 72) { 
            do {
                seleccion--;
                if (seleccion < 0) seleccion = numOpciones - 1;
            } while (opciones[seleccion] == ""); 
        }
        else if (tecla == 80) { 
            do {
                seleccion++;
                if (seleccion >= numOpciones) seleccion = 0;
            } while (opciones[seleccion] == ""); 
        }
        else if (tecla == 13) { elegir = true; }
    }
    return seleccion;
}

inline string inputTexto(string etiqueta, int y) {
    string dato;
    mostrarCursor();
    
    while(true) {
        gotoxy(5, y); cout << string(70, ' '); // Limpia la linea por completo
        gotoxy(5, y); color(COLOR_TITULO); cout << etiqueta; color(COLOR_TEXTO);
        
        getline(cin, dato);
        
        bool valido = false;
        for(char c : dato) { if(c != ' ') valido = true; } // Verifica que no sean solo espacios
        
        if(valido) break;
        
        gotoxy(5, y+1); color(COLOR_ALERTA); cout << "[!] El campo no puede estar vacio.";
        _getch();
        gotoxy(5, y+1); cout << string(70, ' '); // Borra la alerta sin dejar residuos
    }
    
    ocultarCursor();
    return dato;
}

inline int inputNumero(string etiqueta, int y, int min, int max) {
    string inputStr;
    int dato;
    mostrarCursor();
    
    while(true) {
        gotoxy(5, y); cout << string(70, ' ');
        gotoxy(5, y); color(COLOR_TITULO); cout << etiqueta; color(COLOR_TEXTO);
        
        getline(cin, inputStr);
        
        try {
            dato = stoi(inputStr); // Intenta convertir de texto a número
            if (dato >= min && dato <= max) {
                break; // Es valido!
            }
        } catch (...) {
            // Si el usuario ingresa texto (ej: "hola"), la conversión falla y cae aquí silenciosamente
        }
        
        gotoxy(5, y+1); color(COLOR_ALERTA); 
        cout << "[!] Ingrese un valor numerico entre " << min << " y " << max << ".";
        _getch();
        gotoxy(5, y+1); cout << string(70, ' '); // Borra la alerta sin dejar residuos
    }
    
    ocultarCursor();
    return dato;
}

inline void mostrarMensajeExito(string msg) {
    gotoxy(4, 26); cout << string(74, ' '); 
    gotoxy(4, 26); color(COLOR_EXITO); cout << "[OK] " << msg; color(COLOR_TEXTO);
    _getch();
    gotoxy(4, 26); cout << string(74, ' '); 
}

inline void mostrarMensajeError(string msg) {
    gotoxy(4, 26); cout << string(74, ' ');
    gotoxy(4, 26); color(COLOR_ALERTA); cout << "[ERROR] " << msg; color(COLOR_TEXTO);
    _getch();
    gotoxy(4, 26); cout << string(74, ' ');
}

inline void pausarInterfaz() {
    gotoxy(4, 27); cout << string(74, ' ');
    gotoxy(4, 27); color(8); cout << "Presione ENTER para continuar..."; _getch();
    gotoxy(4, 27); cout << string(74, ' ');
}

#endif // INTERFAZ_H