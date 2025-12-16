// Definiciones globales.

#ifndef ESTRUCTURAS_H
#define ESTRUCTURAS_H

#include <iostream>
#include <string>

using namespace std;

enum NivelUrgencia {
  CRITICO,
  URGENTE,
  MODERADO,
  LEVE
};

enum TipoHabitacion {
  INDIVIDUAL,
  AISLAMIENTO,
  CUIDADOS_ESPECIALES,
  DOBLE,
};

enum TipoExamen {
  HEMATOLOGIA,
  QUIMICA,
  IMAGENOLOGIA,
  ORINA,
  HECES
};

struct Fecha {
  int dia;
  int mes;
  int anio;
};

struct Hora {
  int hora;
  int minuto;
};

struct Paciente {
  
  // identificacion
  string cedula;
  string nombre;

  // Datos Clinicos
  string motivoConsulta;
  NivelUrgencia urgencia;

  // Control administrativo
  int numLlegada;

};

inline string obtenerNombreUrgencia(NivelUrgencia urgencia) {
  switch(urgencia) {
    case CRITICO: return "CRITICO";
    case URGENTE: return "URGENTE";
    case MODERADO: return "MODERADO";
    case LEVE: return "LEVE";
    default: return "DESCONOCIDO";
  }
}


// funciones auxiliares

// Sobrecarga simple para comparar fechas
inline bool fechaEsMenor(Fecha a, Fecha b) {
  if ( a.anio != b.anio) return a.anio < b.anio;
  if ( a.mes != b.mes) return a.mes < b.mes;
  return a.dia < b.dia;
}

#endif