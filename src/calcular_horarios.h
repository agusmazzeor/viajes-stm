#ifndef CALCULAR_HORARIOS_H
#define CALCULAR_HORARIOS_H

#include <string>
#include <vector>
#include <unordered_set>
#include "types.h"
#include "funciones_auxiliares.h"

using namespace std;

LineaMap procesar_horarios_teoricos();
int obtener_dia_semana(const string &horario_real);
void encontrar_recorrido_y_calcular_delay(DataViaje &v, const LineaMap &horarios_linea);
string convertir_fecha_a_ymd(const string &fecha_hora);
void procesar_viajes(const string &filename, vector<DataViaje> &data, int start, int count, const LineaMap &horarios_linea);
void calcular_distancia_parada_anterior(LineaMapFinal &linea_map);

#endif // CALCULAR_HORARIOS_H
