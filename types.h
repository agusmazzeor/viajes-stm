#ifndef TYPES_H
#define TYPES_H

#include <map>
#include <iostream>

using namespace std;

// Definición de la estructura para la información de los horarios teóricos de una parada
struct HorarioTeorico
{
  int delay;
  int cantidad_boletos_vendidos;
  int pos_recorrido;
  string horario;
  bool arranco_dia_anterior;
};

// Definición de los tipos de datos anidados
using ParadaMap = map<string, HorarioTeorico>; // clave: id_parada
using RecorridoMap = map<string, ParadaMap>;   // clave: id_recorrido
using TipoDiaMap = map<string, RecorridoMap>;  // clave: id_tipo_dia
using LineaMap = map<string, TipoDiaMap>;      // clave: linea
// lista_horarios_teoricos_parada[linea][id_tipo_dia][id_recorrido][id_parada] = [delay,cantidad_boletos_vendidos,pos_recorrido,horario,arranco_dia_aterior]

struct DataViaje
{
  string fecha_evento; // horario
  string cantidad_pasajeros;
  string codigo_parada_origen; // codigo de la parada de ascenso
  string cod_empresa;          // codigo empresa a la cual pertenece el omnibus
  string linea_codigo;         // codigo de la linea
  string dsc_linea;            // nombre publico de la linea
  string sevar_codigo;         // codigo de la variante
};

#endif // TYPES_H