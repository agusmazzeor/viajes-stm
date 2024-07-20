#ifndef CALCULAR_HORARIOS_H
#define CALCULAR_HORARIOS_H

#include <string>
#include <vector>
#include "types.h"
#include "funciones_auxiliares.h"

using namespace std;

LineaMap procesar_horarios_teoricos(string linea_omnibus);
int obtener_dia_semana(const string &horario_real);

#endif // CALCULAR_HORARIOS_H
