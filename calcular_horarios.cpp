#include "calcular_horarios.h"
#include <string>
#include <vector>
#include <unordered_set>
#include <iomanip>
#include <limits>
#include "types.h"
#include "funciones_auxiliares.h"

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
	}

	// Ignorar la primera línea del encabezado
	if (getline(archivo_paradas, line))
	{
		while (getline(archivo_paradas, line))
		{
			vector<string> tokens = split(line, ',');
			string desc_linea = tokens[1];	 // desc_linea
			string cod_variante = tokens[2]; // cod_varian
			lineas_con_variantes[desc_linea].push_back(cod_variante);
		}
	}

	archivo_paradas.close();
	return lineas_con_variantes;
}

LineaMap procesar_horarios_teoricos()
{
	string ruta_archivo_horarios_teoricos = HORARIOS_POR_PARADA;
	LineaMap lista_horarios_teoricos_parada;

	// Obtener todas las líneas únicas con sus variantes
	unordered_map<string, vector<string>> lineas_con_variantes = obtener_lineas_y_variantes();

	ifstream archivo_horarios_teoricos(ruta_archivo_horarios_teoricos);
	if (!archivo_horarios_teoricos.is_open())
	{
		cerr << "No se pudo abrir el archivo: " << ruta_archivo_horarios_teoricos << endl;
		return lista_horarios_teoricos_parada;
	}

	string linea;
	while (getline(archivo_horarios_teoricos, linea))
	{
		vector<string> horario_teorico = split(linea, ';');
		if (horario_teorico.size() < 7)
		{
			continue; // Saltar líneas mal formateadas
		}

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

				lista_horarios_teoricos_parada[linea_omnibus][variante][id_tipo_dia][id_parada][id_recorrido][pos_recorrido] = ht;
				break; // Una vez encontrada la línea, no es necesario seguir buscando
			}
		}
	}

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
	}
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
}

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
	}

	return horas * 60 + minutos;
}

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
					}
				}
			}
			catch (const out_of_range &e)
			{
				cerr << "Error al convertir horario a minutos: " << e.what() << endl;
			}
		}
	}

	if (!nearest_recorrido.empty() && nearest_pos_recorrido != -1)
	{
		v.recorrido = nearest_recorrido;
		v.pos_recorrido = nearest_pos_recorrido;
		v.delay = min_diff;
	}
}

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
		}

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
					encontrar_recorrido_y_calcular_delay(viaje, horarios_linea);
					if (!viaje.recorrido.empty())
					{
						data.push_back(viaje);
					}
				}
			}
		}
	}
}
