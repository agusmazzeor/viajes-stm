#ifndef TYPES_H
#define TYPES_H

#include <map>
#include <iostream>

using namespace std;

// const string DATOS_VIAJES = "datos_dummy/05_2024_viajes.csv";
// const string HORARIOS_POR_PARADA = "datos_dummy/horarios_por_parada.csv";
// const string PARADAS = "datos_dummy/paradas.csv";
const string DATOS_VIAJES = "datos/viajes_stm.csv";
const string HORARIOS_POR_PARADA = "datos/horarios_por_parada.csv";
const string PARADAS = "datos/paradas.csv";

// Definición de la estructura para la información de los horarios teóricos de una parada
struct HorarioTeorico
{
  int delay;
  int cantidad_boletos_vendidos;
  string horario; // horario teorico
  bool arranco_dia_anterior;
  int retraso_acumulado;
  int cant_pasajeros_parada_anterior;
  double coord_este;
  double coord_norte;
  double distancia_parada_anterior;
};

// Definición de los tipos de datos anidados
using PosRecorridoMap = map<int, HorarioTeorico>;  // clave: ordinal
using RecorridoMap = map<string, PosRecorridoMap>; // clave: id_recorrido
using ParadaMap = map<string, RecorridoMap>;       // clave: id_parada
using TipoDiaMap = map<int, ParadaMap>;            // clave: id_tipo_dia
using VarianteMap = map<string, TipoDiaMap>;       // clave: variante de la linea
using LineaMap = map<string, VarianteMap>;         // clave: linea
// algo[linea][variante][tipo_dia][parada][recorrido][pos_recorrido]

// algo[linea][variante][tipo_dia][recorrido][pos_recorrido][parada] VIEJO
// algo[linea][variante][tipo_dia][fecha][recorrido][pos_recorrido][parada] NUEVO
using ParadaMapFinal = map<string, HorarioTeorico>;          // clave: id_parada
using PosRecorridoMapFinal = map<int, ParadaMapFinal>;       // clave: ordinal
using RecorridoMapFinal = map<string, PosRecorridoMapFinal>; // clave: id_recorrido
using FechaMapFinal = map<string, RecorridoMapFinal>;        // clave: fecha
using TipoDiaMapFinal = map<string, FechaMapFinal>;          // clave: id_tipo_dia
using VarianteMapFinal = map<string, TipoDiaMapFinal>;       // clave: variante de la linea
using LineaMapFinal = map<string, VarianteMapFinal>;         // clave: linea

// Definición de la estructura para la información de los viajes
struct DataViaje
{
  string fecha_evento; // horario
  string cantidad_pasajeros;
  string codigo_parada_origen; // codigo de la parada de ascenso
  string cod_empresa;          // codigo empresa a la cual pertenece el omnibus
  string linea_codigo;         // codigo de la linea
  string dsc_linea;            // nombre publico de la linea
  string sevar_codigo;         // codigo de la variante
  string recorrido;            // recorrido del omnibus (calculado)
  int pos_recorrido;           // pos_recorrido del omnibus (calculado)
  int delay;                   // delay del omnibus (calculado)
};

#endif // TYPES_H
