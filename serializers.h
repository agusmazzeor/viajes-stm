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
string serialize_viajes(const vector<DataViaje> &viajes);
vector<DataViaje> deserialize_viajes(const string &str);

#endif // SERIALIZERS_H
