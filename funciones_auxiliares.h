#ifndef FUNCIONES_AUXILIARES_H
#define FUNCIONES_AUXILIARES_H

#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include "types.h"

using namespace std;

vector<string> split(const string &s, char delimiter);
void print_data_linea(LineaMap &lista_horarios_teoricos_parada);
void print_data_viaje(const vector<DataViaje> &data_viajes);

#endif // FUNCIONES_AUXILIARES_H
