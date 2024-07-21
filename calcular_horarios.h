#ifndef CALCULAR_HORARIOS_H
#define CALCULAR_HORARIOS_H

#include <string>
#include <vector>
#include "types.h"
#include "funciones_auxiliares.h"

using namespace std;

LineaMap procesar_horarios_teoricos(string linea_omnibus);
int obtener_dia_semana(const string &horario_real);
string encontrar_recorrido_del_viaje(const DataViaje &v, const LineaMap &horarios_linea);
string calcular_delay(const DataViaje &v, const LineaMap &horarios_linea);

#endif // CALCULAR_HORARIOS_H
