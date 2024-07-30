#ifndef FUNCIONES_AUXILIARES_H
#define FUNCIONES_AUXILIARES_H

#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include "types.h"

using namespace std;

vector<string> split(const string &s, char delimiter);
void guardar_linea_map_en_archivo(const LineaMap &linea_map, const string &filename);
void guardar_tiempo_de_ejecucion_en_archivo(const int &duration, const int &num_processes, const string &filename, const string &tipo_tiempo);
void print_data_linea(LineaMap &lista_horarios_teoricos_parada);
void print_data_viaje(const vector<DataViaje> &data_viajes);
void guardar_linea_map_final_en_archivo(const LineaMapFinal &linea_map, const string &filename);
void combinar_linea_maps(LineaMap &dest, const LineaMap &src);
string obtener_derectorio_actual();

#endif // FUNCIONES_AUXILIARES_H
