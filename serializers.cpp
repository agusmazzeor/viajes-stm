#include "serializers.h"

void serialize_map(const unordered_map<string, double> &map, vector<char> &buffer)
{
	stringstream ss;
	for (const auto &pair : map)
	{
		ss << pair.first << "," << pair.second << "\n";
	}
	string s = ss.str();
	buffer.assign(s.begin(), s.end());
}

void deserialize_map(const vector<char> &buffer, unordered_map<string, double> &map)
{
	stringstream ss(string(buffer.begin(), buffer.end()));
	string line;
	while (getline(ss, line))
	{
		stringstream line_ss(line);
		string key;
		double value;
		getline(line_ss, key, ',');
		line_ss >> value;
		map[key] = value;
	}
}

void serialize_horarios_teoricos(const LineaMap &schedule, string &output)
{
	stringstream ss;
	for (const auto &linea : schedule)
	{
		for (const auto &id_tipo_dia : linea.second)
		{
			for (const auto &id_recorrido : id_tipo_dia.second)
			{
				for (const auto &id_parada : id_recorrido.second)
				{
					const HorarioTeorico &ht = id_parada.second;
					ss << linea.first << "," << id_tipo_dia.first << "," << id_recorrido.first << "," << id_parada.first << ","
						 << ht.delay << "," << ht.cantidad_boletos_vendidos << "," << ht.pos_recorrido << ","
						 << ht.horario << "," << ht.arranco_dia_anterior << "\n";
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
			ht.delay = stoi(tokens[4]);
			ht.cantidad_boletos_vendidos = stoi(tokens[5]);
			ht.pos_recorrido = stoi(tokens[6]);
			ht.horario = tokens[7];
			ht.arranco_dia_anterior = tokens[8] == "1";
			schedule[tokens[0]][stoi(tokens[1])][tokens[2]][tokens[3]] = ht;
		}
	}
}
