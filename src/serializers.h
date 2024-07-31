#ifndef SERIALIZERS_H
#define SERIALIZERS_H

#include <sstream>
#include <vector>
#include <unordered_map>
#include "types.h"
#include "funciones_auxiliares.h"

using namespace std;

void serialize_horarios_teoricos(const LineaMap &schedule, string &output);
void deserialize_horarios_teoricos(const string &input, LineaMap &schedule);
void serialize_horarios_teoricos_final(const LineaMapFinal &schedule, string &output);
void deserialize_horarios_teoricos_final(const string &input, LineaMapFinal &schedule);
string serialize_viajes(const vector<DataViaje> &viajes);
vector<DataViaje> deserialize_viajes(const string &str);
// void convertir_linea_map_a_map_linea(const LineaMap &schedule, LineaMapFinal &linea_map_final);

#endif // SERIALIZERS_H
