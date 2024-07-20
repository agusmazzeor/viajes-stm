#ifndef SERIALIZERS_H
#define SERIALIZERS_H

#include <sstream>
#include <vector>
#include <unordered_map>
#include "types.h"
#include "funciones_auxiliares.h"

using namespace std;

void serialize_map(const unordered_map<string, double> &map, vector<char> &buffer);
void deserialize_map(const vector<char> &buffer, unordered_map<string, double> &map);
void serialize_horarios_teoricos(const LineaMap &schedule, string &output);
void deserialize_horarios_teoricos(const string &input, LineaMap &schedule);

#endif // SERIALIZERS_H
