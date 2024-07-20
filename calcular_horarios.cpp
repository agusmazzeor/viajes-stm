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
// {"8870", "1111", "8872"}

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
			string id_tipo_dia = horario_teorico[0];							 // tipo_dia
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

			lista_horarios_teoricos_parada[variante][id_tipo_dia][id_recorrido][id_parada] = ht;
		}
	}

	archivo_horarios_teoricos.close();
	return lista_horarios_teoricos_parada;
};
