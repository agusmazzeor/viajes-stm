#include "serializers.h"

void serialize_horarios_teoricos(const LineaMap &schedule, string &output)
{
	stringstream ss;
	for (const auto &linea : schedule)
	{
		for (const auto &id_tipo_dia : linea.second)
		{
			for (const auto &id_parada : id_tipo_dia.second)
			{
				for (const auto &id_recorrido : id_parada.second)
				{
					for (const auto &pos_recorrido : id_recorrido.second)
					{
						const HorarioTeorico &ht = pos_recorrido.second;
						ss << linea.first << "," << id_tipo_dia.first << "," << id_parada.first << "," << id_recorrido.first << "," << pos_recorrido.first << ","
							 << ht.delay << "," << ht.cantidad_boletos_vendidos << "," << ht.horario << "," << ht.arranco_dia_anterior << "\n";
					}
				}
			}
		}
	}
	output = ss.str();
}

void deserialize_horarios_teoricos(const string &input, LineaMap &schedule)
{
	stringstream ss(input);
	string line;
	while (getline(ss, line))
	{
		vector<string> tokens = split(line, ',');
		if (tokens.size() == 9)
		{
			HorarioTeorico ht;
			ht.delay = stoi(tokens[5]);
			ht.cantidad_boletos_vendidos = stoi(tokens[6]);
			ht.horario = tokens[7];
			ht.arranco_dia_anterior = tokens[8] == "1";
			schedule[tokens[0]][stoi(tokens[1])][tokens[2]][tokens[3]][stoi(tokens[4])] = ht;
		}
	}
}

string serialize_viajes(const vector<DataViaje> &viajes)
{
	stringstream ss;
	for (const auto &viaje : viajes)
	{
		ss << viaje.fecha_evento << ","
			 << viaje.cantidad_pasajeros << ","
			 << viaje.codigo_parada_origen << ","
			 << viaje.cod_empresa << ","
			 << viaje.linea_codigo << ","
			 << viaje.dsc_linea << ","
			 << viaje.sevar_codigo << ","
			 << viaje.recorrido << ","
			 << viaje.pos_recorrido << ","
			 << viaje.delay << "\n";
	}
	return ss.str();
}

vector<DataViaje> deserialize_viajes(const string &str)
{
	vector<DataViaje> viajes;
	stringstream ss(str);
	string line;
	while (getline(ss, line))
	{
		stringstream line_ss(line);
		string token;
		DataViaje viaje;

		getline(line_ss, viaje.fecha_evento, ',');
		getline(line_ss, viaje.cantidad_pasajeros, ',');
		getline(line_ss, viaje.codigo_parada_origen, ',');
		getline(line_ss, viaje.cod_empresa, ',');
		getline(line_ss, viaje.linea_codigo, ',');
		getline(line_ss, viaje.dsc_linea, ',');
		getline(line_ss, viaje.sevar_codigo, ',');
		getline(line_ss, viaje.recorrido, ',');
		line_ss >> viaje.pos_recorrido;
		line_ss.ignore(1);
		line_ss >> viaje.delay;

		viajes.push_back(viaje);
	}
	return viajes;
}
