#include "calcular_horarios.h"

const string HORARIOS_POR_PARADA = "datos_dummy/horarios_por_parada.csv";
const string PARADAS = "datos_dummy/paradas.csv";

vector<string> obtener_variantes_de_linea(string linea)
{
	string ruta_archivo_paradas = PARADAS;
	vector<string> variantes_de_linea;
	string line;

	ifstream archivo_paradas(ruta_archivo_paradas);
	if (!archivo_paradas.is_open())
	{
		cerr << "No se pudo abrir el archivo: " << ruta_archivo_paradas << endl;
		return variantes_de_linea;
	}

	// Ignorar la primera línea del encabezado
	if (getline(archivo_paradas, line))
	{
		while (getline(archivo_paradas, line))
		{
			vector<string> tokens = split(line, ',');
			if (tokens[1] == linea) // desc_linea
			{
				variantes_de_linea.push_back(tokens[2]); // cod_varian
			};
		};
	};

	archivo_paradas.close();
	return variantes_de_linea;
};

// Función para procesar los horarios teóricos
LineaMap procesar_horarios_teoricos(string linea_omnibus)
{
	string ruta_archivo_horarios_teoricos = HORARIOS_POR_PARADA;
	LineaMap lista_horarios_teoricos_parada;
	vector<string> vars_de_linea_a_evaluar = obtener_variantes_de_linea(linea_omnibus);

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
		if (find(vars_de_linea_a_evaluar.begin(), vars_de_linea_a_evaluar.end(), variante) != vars_de_linea_a_evaluar.end())
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
			ht.pos_recorrido = pos_recorrido;
			ht.horario = horario;
			ht.arranco_dia_anterior = arranco_dia_anterior;

			lista_horarios_teoricos_parada[variante][id_tipo_dia][id_parada][id_recorrido] = ht;
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
	cout << "  convertir fecha: " << fecha_hora << endl;
	const string formato_fecha = "%Y-%m-%d %H:%M:%S";
	tm fecha_completa = {};
	istringstream ss(fecha_hora);
	ss >> get_time(&fecha_completa, formato_fecha.c_str());

	int horas = fecha_completa.tm_hour;
	int minutos = fecha_completa.tm_min;
	cout << "    horas: " << horas << endl;
	cout << "    minutos: " << minutos << endl;
	ostringstream oss;
	oss << horas << setfill('0') << setw(2) << minutos;
	cout << "    resultado: " << oss.str() << endl;
	return oss.str();
}

string encontrar_recorrido_del_viaje(const DataViaje &v, const LineaMap &horarios_teoricos)
{
	int dia_semana = obtener_dia_semana(v.fecha_evento);

	auto linea_it = horarios_teoricos.find(v.sevar_codigo);
	if (linea_it == horarios_teoricos.end())
		return "";

	auto dia_it = linea_it->second.find(dia_semana);
	if (dia_it == linea_it->second.end())
		return "";

	auto parada_it = dia_it->second.find(v.codigo_parada_origen);
	if (parada_it == dia_it->second.end())
		return "";

	const auto &recorridos = parada_it->second;
	string horario_viaje_hmm = convertir_fecha_a_hmm(v.fecha_evento);
	string nearest_recorrido;
	int min_diff = numeric_limits<int>::max();

	for (const auto &it : recorridos)
	{
		const string &id_recorrido = it.first;
		const HorarioTeorico &horario_teorico = it.second;

		cout << "Evaluando recorrido: " << id_recorrido << endl;

		int horario_teorico_hmm = stoi(horario_teorico.horario);
		int horario_viaje_int = stoi(horario_viaje_hmm);

		cout << "Horario teorico: " << horario_teorico.horario << endl;
		cout << "Horario viaje:   " << horario_viaje_hmm << endl;

		if (horario_teorico_hmm <= horario_viaje_int)
		{
			int diff = horario_viaje_int - horario_teorico_hmm;
			if (diff < min_diff)
			{
				min_diff = diff;
				nearest_recorrido = id_recorrido;
			}
		}
		else
		{
			cout << "El horario del recorrido es mayor" << endl;
		}
	}

	cout << "Nearest recorrido: " << nearest_recorrido << endl;
	cout << "------------------------" << endl;
	return nearest_recorrido;
}
