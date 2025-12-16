// main <3
#include <iostream>
#include "estructuras.h"
#include "AVL.h"

using namespace std;

// Función auxiliar solo para crear pacientes rápido en el test
Paciente crearPacienteTest(string nombre, string cedula, NivelUrgencia urg, int llegada) {
    Paciente p;
    p.nombre = nombre;
    p.cedula = cedula;
    p.motivoConsulta = "Test";
    p.urgencia= urg;
    p.numLlegada = llegada;
    return p;
}

int main() {
    // 1. Inicializar el árbol
    Nodo* raizNodo = NULL;

    cout << "=== INICIO DE PRUEBA DE TRIAJE (AVL) ===" << endl << endl;

    // 2. SIMULACION DE LLEGADA DE PACIENTES
    // Nota: El orden de inserción es cronológico (como van llegando al hospital)
    
    // Paciente 1: Llega alguien con dolor de cabeza (LEVE)
    cout << "1. Llega Juan (LEVE, Turno 1)" << endl;
    insertarPaciente(raizNodo, crearPacienteTest("Juan Perez", "111", LEVE, 1));

    // Paciente 2: Llega alguien con fiebre alta (MODERADO)
    cout << "2. Llega Maria (MODERADO, Turno 2)" << endl;
    insertarPaciente(raizNodo, crearPacienteTest("Maria Gomez", "222", MODERADO, 2));

    // Paciente 3: ¡ACCIDENTE! Llega alguien muriendo (CRITICO) - Llega de 3ro
    cout << "3. Llega PEDRO (CRITICO, Turno 3) - Deberia ser atendido primero" << endl;
    insertarPaciente(raizNodo, crearPacienteTest("PEDRO ROJAS", "333", CRITICO, 3));

    // Paciente 4: Otro paciente grave (CRITICO) - Llega de 4to
    cout << "4. Llega Ana (CRITICO, Turno 4)" << endl;
    insertarPaciente(raizNodo, crearPacienteTest("Ana Silva", "444", CRITICO, 4));

    cout << endl << "-----------------------------------" << endl;
    cout << "VISUALIZACION DEL ARBOL (In-Orden)" << endl;
    cout << "Deberia mostrarse ordenado por prioridad (Critico primero):" << endl;
    mostrarArbol(raizNodo, 1); // 1 = Inorden
    cout << "-----------------------------------" << endl << endl;

    // 3. SIMULACION DE ATENCION (Extracción)
    cout << "=== COMIENZA LA ATENCION MEDICA ===" << endl;

    // Primera atención: Debería ser PEDRO (Critico, llegó antes que Ana)
    cout << "\nLlamando al primer paciente..." << endl;
    eliminarPacienteMayorPrioridad(raizNodo);

    // Segunda atención: Debería ser ANA (Critico, llegó después de Pedro)
    cout << "\nLlamando al segundo paciente..." << endl;
    eliminarPacienteMayorPrioridad(raizNodo);

    // Tercera atención: Debería ser MARIA (Moderado)
    cout << "\nLlamando al tercer paciente..." << endl;
    eliminarPacienteMayorPrioridad(raizNodo);

    // Cuarta atención: Debería ser JUAN (Leve, aunque llegó de primero, espera al final)
    cout << "\nLlamando al cuarto paciente..." << endl;
    eliminarPacienteMayorPrioridad(raizNodo);

    // Intento extra (Arbol vacío)
    cout << "\nLlamando al quinto paciente..." << endl;
    eliminarPacienteMayorPrioridad(raizNodo);

    return 0;
}