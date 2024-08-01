#include "calcular_horarios.h"
#include <string>
#include <vector>
#include <unordered_set>
#include <iomanip>
#include <limits>
#include <unordered_map>
#include <algorithm> // Necesario para std::find
#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>
#include "types.h"
#include "funciones_auxiliares.h"

using namespace std;

unordered_map<string, vector<string>> obtener_lineas_y_variantes()
{
	string ruta_archivo_paradas = PARADAS;
	unordered_map<string, vector<string>> lineas_con_variantes;
	string line;

	ifstream archivo_paradas(ruta_archivo_paradas);
	if (!archivo_paradas.is_open())
	{
		cerr << "No se pudo abrir el archivo: " << ruta_archivo_paradas << endl;
		return lineas_con_variantes;
	};

	// Ignorar la primera línea del encabezado
	if (getline(archivo_paradas, line))
	{
		while (getline(archivo_paradas, line))
		{
			vector<string> tokens = split(line, ',');
			string desc_linea = tokens[1];	 // desc_linea
			string cod_variante = tokens[2]; // cod_varian
			lineas_con_variantes[desc_linea].push_back(cod_variante);
		};
	};

	archivo_paradas.close();
	return lineas_con_variantes;
};

unordered_map<string, pair<double, double>> obtener_paradas_y_coordenadas()
{
	string ruta_archivo_paradas = PARADAS;
	unordered_map<string, pair<double, double>> paradas_con_coordenadas;
	string line;

	ifstream archivo_paradas(ruta_archivo_paradas);
	if (!archivo_paradas.is_open())
	{
		cerr << "No se pudo abrir el archivo: " << ruta_archivo_paradas << endl;
		return paradas_con_coordenadas;
	};

	// Ignorar la primera línea del encabezado
	if (getline(archivo_paradas, line))
	{
		while (getline(archivo_paradas, line))
		{
			vector<string> tokens = split(line, ',');
			if (tokens.size() < 10)
			{
				cerr << "Línea mal formada: " << line << endl;
				continue; // Saltar líneas mal formateadas
			}

			string cod_parada = tokens[0];
			double coord_este, coord_norte;

			try
			{
				coord_este = stod(tokens[8]);
				coord_norte = stod(tokens[9]);
			}
			catch (const std::exception &e)
			{
				cerr << "Error convirtiendo coordenadas: " << e.what() << " en la línea: " << line << endl;
				continue; // Saltar esta línea si hay un error de conversión
			}

			paradas_con_coordenadas[cod_parada] = make_pair(coord_este, coord_norte);
		};
	};

	archivo_paradas.close();
	return paradas_con_coordenadas;
};

LineaMap procesar_horarios_teoricos()
{
	string ruta_archivo_horarios_teoricos = HORARIOS_POR_PARADA;
	LineaMap lista_horarios_teoricos_parada;

	// Obtener todas las líneas únicas con sus variantes
	unordered_map<string, vector<string>> lineas_con_variantes = obtener_lineas_y_variantes();
	// Obtener las coordenadas de las paradas
	unordered_map<string, pair<double, double>> paradas_con_coordenadas = obtener_paradas_y_coordenadas();

	ifstream archivo_horarios_teoricos(ruta_archivo_horarios_teoricos);
	if (!archivo_horarios_teoricos.is_open())
	{
		cerr << "No se pudo abrir el archivo: " << ruta_archivo_horarios_teoricos << endl;
		return lista_horarios_teoricos_parada;
	};

	string linea;
	while (getline(archivo_horarios_teoricos, linea))
	{
		vector<string> horario_teorico = split(linea, ';');
		if (horario_teorico.size() < 7)
		{
			continue; // Saltar líneas mal formateadas
		};

		string variante = horario_teorico[1]; // cod_variante

		// Buscar la línea correspondiente a esta variante
		for (const auto &linea_variante : lineas_con_variantes)
		{
			const string &linea_omnibus = linea_variante.first;
			const vector<string> &variantes = linea_variante.second;

			if (find(variantes.begin(), variantes.end(), variante) != variantes.end())
			{
				int id_tipo_dia = stoi(horario_teorico[0]);						 // tipo_dia
				string id_recorrido = horario_teorico[2];							 // frecuencia
				string id_parada = horario_teorico[3];								 // cod_ubic_parada
				int pos_recorrido = stoi(horario_teorico[4]);					 // ordinal
				string horario = horario_teorico[5];									 // hora
				bool arranco_dia_anterior = horario_teorico[6] == "1"; // dia_anterior

				HorarioTeorico ht;
				ht.delay = -1;
				ht.cantidad_boletos_vendidos = 0;
				ht.horario = horario;
				ht.arranco_dia_anterior = arranco_dia_anterior;
				ht.retraso_acumulado = -1;
				ht.cant_pasajeros_parada_anterior = 0;
				ht.distancia_parada_anterior = 0.0;

				// Buscar y asignar coordenadas de la parada
				auto coord_it = paradas_con_coordenadas.find(id_parada);
				if (coord_it != paradas_con_coordenadas.end())
				{
					ht.coord_este = coord_it->second.first;
					ht.coord_norte = coord_it->second.second;
				}
				else
				{
					cerr << "No se encontraron coordenadas para la parada: " << id_parada << endl;
					ht.coord_este = 0;
					ht.coord_norte = 0;
				}

				lista_horarios_teoricos_parada[linea_omnibus][variante][id_tipo_dia][id_parada][id_recorrido][pos_recorrido] = ht;
				break; // Una vez encontrada la línea, no es necesario seguir buscando
			};
		};
	};

	archivo_horarios_teoricos.close();
	return lista_horarios_teoricos_parada;
};

int obtener_dia_semana(const string &horario_real)
{
	// Definir el formato de la fecha
	const string formato_fecha = "%Y-%m-%dT%H:%M:%S";

	// Convertir la cadena de fecha a un objeto tm
	tm fecha_completa = {};
	istringstream ss(horario_real);
	ss >> get_time(&fecha_completa, formato_fecha.c_str());

	// Calcular el día de la semana (0 = domingo, 1 = lunes, ..., 6 = sábado)
	mktime(&fecha_completa);
	int dia_numero = fecha_completa.tm_wday;

	// Determinar el tipo de día
	if (dia_numero == 0)
	{
		return 3; // Domingo
	}
	else if (dia_numero == 6)
	{
		return 2; // Sábado
	}
	else
	{
		return 1; // Día hábil
	};
};

string convertir_fecha_a_hmm(const string &fecha_hora)
{
	const string formato_fecha = "%Y-%m-%d %H:%M:%S";
	tm fecha_completa = {};
	istringstream ss(fecha_hora);
	ss >> get_time(&fecha_completa, formato_fecha.c_str());

	int horas = fecha_completa.tm_hour;
	int minutos = fecha_completa.tm_min;
	ostringstream oss;
	oss << horas << setfill('0') << setw(2) << minutos;
	return oss.str();
};

string convertir_fecha_a_ymd(const string &fecha_hora)
{
	const string formato_fecha = "%Y-%m-%d %H:%M:%S";
	tm fecha_completa = {};
	istringstream ss(fecha_hora);
	ss >> get_time(&fecha_completa, formato_fecha.c_str());

	int ano = fecha_completa.tm_year + 1900;
	int mes = fecha_completa.tm_mon;
	int dia = fecha_completa.tm_mday;

	ostringstream oss;
	oss << ano << setfill('0') << setw(2) << mes << setfill('0') << setw(2) << dia;
	return oss.str();
};

int convertir_hmm_a_minutos(const string &horario_hmm)
{
	int horas = 0, minutos = 0;

	switch (horario_hmm.length())
	{
	case 1:
		// Ejemplo: "1" -> 00:01
		minutos = stoi(horario_hmm.substr(0, 1));
		break;
	case 2:
		// Ejemplo: "12" -> 00:12
		minutos = stoi(horario_hmm.substr(0, 2));
		break;
	case 3:
		// Ejemplo: "824" -> 08:24
		horas = stoi(horario_hmm.substr(0, 1));
		minutos = stoi(horario_hmm.substr(1, 2));
		break;
	case 4:
		// Ejemplo: "1324" -> 13:24
		horas = stoi(horario_hmm.substr(0, 2));
		minutos = stoi(horario_hmm.substr(2, 2));
		break;
	default:
		throw out_of_range("La cadena de entrada no tiene el formato esperado: " + horario_hmm);
	};

	return horas * 60 + minutos;
};

void encontrar_recorrido_y_calcular_delay(DataViaje &v, const LineaMap &horarios_linea)
{
	auto linea_it = horarios_linea.find(v.dsc_linea);
	if (linea_it == horarios_linea.end())
		return;

	auto variante_it = linea_it->second.find(v.sevar_codigo);
	if (variante_it == linea_it->second.end())
		return;

	int dia_semana = obtener_dia_semana(v.fecha_evento);
	auto dia_it = variante_it->second.find(dia_semana);
	if (dia_it == variante_it->second.end())
		return;

	auto parada_it = dia_it->second.find(v.codigo_parada_origen);
	if (parada_it == dia_it->second.end())
		return;

	const auto &recorridos = parada_it->second;
	string horario_viaje_hmm = convertir_fecha_a_hmm(v.fecha_evento);
	string nearest_recorrido;
	int nearest_pos_recorrido = -1;
	int min_diff = numeric_limits<int>::max();

	for (const auto &recorrido : recorridos)
	{
		for (const auto &pos_recorrido : recorrido.second)
		{
			const HorarioTeorico &horario_teorico = pos_recorrido.second;

			try
			{
				int horario_viaje_minutos = convertir_hmm_a_minutos(horario_viaje_hmm);
				int horario_teorico_minutos = convertir_hmm_a_minutos(horario_teorico.horario);

				if (horario_teorico_minutos <= horario_viaje_minutos)
				{
					int diff = horario_viaje_minutos - horario_teorico_minutos;
					if (diff < min_diff)
					{
						min_diff = diff;
						nearest_recorrido = recorrido.first;
						nearest_pos_recorrido = pos_recorrido.first;
					};
				};
			}
			catch (const out_of_range &e)
			{
				cerr << "Error al convertir horario a minutos: " << e.what() << endl;
			};
		};
	};

	if (!nearest_recorrido.empty() && nearest_pos_recorrido != -1)
	{
		v.recorrido = nearest_recorrido;
		v.pos_recorrido = nearest_pos_recorrido;
		v.delay = min_diff;
	};
};

void encontrar_recorrido(DataViaje &v, const LineaMap &horarios_linea)
{
	auto linea_it = horarios_linea.find(v.dsc_linea);
	if (linea_it == horarios_linea.end())
		return;

	auto variante_it = linea_it->second.find(v.sevar_codigo);
	if (variante_it == linea_it->second.end())
		return;

	int dia_semana = obtener_dia_semana(v.fecha_evento);
	auto dia_it = variante_it->second.find(dia_semana);
	if (dia_it == variante_it->second.end())
		return;

	auto parada_it = dia_it->second.find(v.codigo_parada_origen);
	if (parada_it == dia_it->second.end())
		return;

	const auto &recorridos = parada_it->second;
	string horario_viaje_hmm = convertir_fecha_a_hmm(v.fecha_evento);
	string nearest_recorrido;
	int nearest_pos_recorrido = -1;
	int min_diff = numeric_limits<int>::max();

	for (const auto &recorrido : recorridos)
	{
		for (const auto &pos_recorrido : recorrido.second)
		{
			const HorarioTeorico &horario_teorico = pos_recorrido.second;

			try
			{
				int horario_viaje_minutos = convertir_hmm_a_minutos(horario_viaje_hmm);
				int horario_teorico_minutos = convertir_hmm_a_minutos(horario_teorico.horario);

				if (horario_teorico_minutos <= horario_viaje_minutos)
				{
					int diff = horario_viaje_minutos - horario_teorico_minutos;
					if (diff < min_diff)
					{
						min_diff = diff;
						nearest_recorrido = recorrido.first;
						nearest_pos_recorrido = pos_recorrido.first;
					};
				};
			}
			catch (const out_of_range &e)
			{
				cerr << "Error al convertir horario a minutos: " << e.what() << endl;
			};
		};
	};

	if (!nearest_recorrido.empty() && nearest_pos_recorrido != -1)
	{
		v.recorrido = nearest_recorrido;
		v.pos_recorrido = nearest_pos_recorrido;
		v.delay = min_diff;
	};
};

void calcular_delay(DataViaje &v, const LineaMap &horarios_linea)
{
	auto linea_it = horarios_linea.find(v.dsc_linea);
	if (linea_it == horarios_linea.end())
		return;

	auto variante_it = linea_it->second.find(v.sevar_codigo);
	if (variante_it == linea_it->second.end())
		return;

	int dia_semana = obtener_dia_semana(v.fecha_evento);
	auto dia_it = variante_it->second.find(dia_semana);
	if (dia_it == variante_it->second.end())
		return;

	auto parada_it = dia_it->second.find(v.codigo_parada_origen);
	if (parada_it == dia_it->second.end())
		return;

	const auto &recorridos = parada_it->second;
	string horario_viaje_hmm = convertir_fecha_a_hmm(v.fecha_evento);
	int min_diff = numeric_limits<int>::max();

	for (const auto &recorrido : recorridos)
	{
		for (const auto &pos_recorrido : recorrido.second)
		{
			const HorarioTeorico &horario_teorico = pos_recorrido.second;

			try
			{
				int horario_viaje_minutos = convertir_hmm_a_minutos(horario_viaje_hmm);
				int horario_teorico_minutos = convertir_hmm_a_minutos(horario_teorico.horario);

				if (horario_teorico_minutos <= horario_viaje_minutos)
				{
					int diff = horario_viaje_minutos - horario_teorico_minutos;
					if (diff < min_diff)
					{
						min_diff = diff;
					};
				};
			}
			catch (const out_of_range &e)
			{
				cerr << "Error al convertir horario a minutos: " << e.what() << endl;
			};
		};
	};

	if (min_diff != numeric_limits<int>::max())
	{
		v.delay = min_diff;
	};
};

void procesar_viajes(const string &filename, vector<DataViaje> &data, int start, int count, const LineaMap &horarios_linea)
{
	ifstream file(filename);
	string line;

	// Ignorar la primera línea del encabezado
	if (getline(file, line))
	{
		// Avanzar hasta el inicio del chunk
		for (int i = 0; i < start; ++i)
		{
			getline(file, line);
		};

		// Leer las líneas del chunk
		for (int i = 0; i < count; ++i)
		{
			if (getline(file, line))
			{
				vector<string> tokens = split(line, ',');
				DataViaje viaje;

				viaje.dsc_linea = tokens[15];
				// Agregar solo si dsc_linea está en lineas_a_evaluar
				if (horarios_linea.find(viaje.dsc_linea) != horarios_linea.end())
				{
					viaje.fecha_evento = tokens[2];
					viaje.cantidad_pasajeros = tokens[10];
					viaje.codigo_parada_origen = tokens[11];
					viaje.cod_empresa = tokens[12];
					viaje.linea_codigo = tokens[14];
					viaje.sevar_codigo = tokens[16];
					encontrar_recorrido(viaje, horarios_linea);
					calcular_delay(viaje, horarios_linea);
					if (!viaje.recorrido.empty())
					{
						data.push_back(viaje);
					};
				};
			};
		};
	};
};

// Función para calcular la distancia entre dos puntos UTM
double calcular_distancia_utm(double x1, double y1, double x2, double y2)
{
	return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}

void calcular_distancia_parada_anterior(LineaMapFinal &linea_map)
{
	for (auto &linea : linea_map)
	{
		for (auto &variante : linea.second)
		{
			for (auto &tipo_dia : variante.second)
			{
				for (auto &fecha : tipo_dia.second)
				{
					for (auto &recorrido : fecha.second)
					{
						HorarioTeorico *parada_anterior = nullptr;

						for (auto &pos_recorrido : recorrido.second)
						{
							for (auto &parada : pos_recorrido.second)
							{
								HorarioTeorico &ht = parada.second;
								if (parada_anterior)
								{
									ht.distancia_parada_anterior = calcular_distancia_utm(
											parada_anterior->coord_este, parada_anterior->coord_norte,
											ht.coord_este, ht.coord_norte);
								}
								else
								{
									ht.distancia_parada_anterior = 0; // Primera parada no tiene parada anterior
								}
								parada_anterior = &ht;
							}
						}
					}
				}
			}
		}
	}
}
