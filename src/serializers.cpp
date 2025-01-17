#include "serializers.h"
#include <sstream>
#include <vector>
#include <unordered_map>
#include "types.h"
#include "funciones_auxiliares.h"

void serialize_horarios_teoricos(const LineaMap &schedule, string &output)
{
	stringstream ss;
	for (const auto &linea : schedule)
	{
		for (const auto &variante : linea.second)
		{
			for (const auto &id_tipo_dia : variante.second)
			{
				for (const auto &id_parada : id_tipo_dia.second)
				{
					for (const auto &id_recorrido : id_parada.second)
					{
						for (const auto &pos_recorrido : id_recorrido.second)
						{
							const HorarioTeorico &ht = pos_recorrido.second;
							ss << linea.first << "," << variante.first << "," << id_tipo_dia.first << "," << id_parada.first << "," << id_recorrido.first << "," << pos_recorrido.first << ","
								 << ht.delay << "," << ht.cantidad_boletos_vendidos << "," << ht.horario << "," << ht.arranco_dia_anterior << ","
								 << ht.retraso_acumulado << "," << ht.cant_pasajeros_parada_anterior << ","
								 << ht.coord_este << "," << ht.coord_norte << "," << ht.distancia_parada_anterior << "\n";
						}
					}
				}
			}
		}
	}
	output = ss.str();
}

// algo[linea][variante][tipo_dia][fecha][recorrido][pos_recorrido][parada]
void serialize_horarios_teoricos_final(const LineaMapFinal &schedule, string &output)
{
	stringstream ss;
	for (const auto &linea : schedule)
	{
		for (const auto &variante : linea.second)
		{
			for (const auto &id_tipo_dia : variante.second)
			{
				for (const auto &fecha : id_tipo_dia.second)
				{
					for (const auto &id_recorrido : fecha.second)
					{
						for (const auto &pos_recorrido : id_recorrido.second)
						{
							for (const auto &id_parada : pos_recorrido.second)
							{
								const HorarioTeorico &ht = id_parada.second;
								ss << linea.first << "," << variante.first << "," << id_tipo_dia.first << "," << fecha.first << "," << id_recorrido.first << "," << pos_recorrido.first << ","
									 << id_parada.first << "," << ht.delay << "," << ht.cantidad_boletos_vendidos << "," << ht.horario << "," << ht.arranco_dia_anterior << ","
									 << ht.retraso_acumulado << "," << ht.cant_pasajeros_parada_anterior << ","
									 << ht.coord_este << "," << ht.coord_norte << "," << ht.distancia_parada_anterior << "\n";
							}
						}
					}
				}
			}
		}
	}
	output = ss.str();
}

void deserialize_horarios_teoricos_final(const string &input, LineaMapFinal &schedule)
{
	stringstream ss(input);
	string line;
	while (getline(ss, line))
	{
		vector<string> tokens = split(line, ',');
		if (tokens.size() == 16)
		{
			HorarioTeorico ht;
			ht.delay = stoi(tokens[7]);
			ht.cantidad_boletos_vendidos = stoi(tokens[8]);
			ht.horario = tokens[9];
			ht.arranco_dia_anterior = tokens[10] == "1";
			ht.retraso_acumulado = stoi(tokens[11]);
			ht.cant_pasajeros_parada_anterior = stoi(tokens[12]);
			ht.coord_este = stod(tokens[13]);
			ht.coord_norte = stod(tokens[14]);
			ht.distancia_parada_anterior = stod(tokens[15]);
			schedule[tokens[0]][tokens[1]][tokens[2]][tokens[3]][tokens[4]][stoi(tokens[5])][tokens[6]] = ht;
		}
	}
}

void deserialize_horarios_teoricos(const string &input, LineaMap &schedule)
{
	stringstream ss(input);
	string line;
	while (getline(ss, line))
	{
		vector<string> tokens = split(line, ',');
		if (tokens.size() == 15)
		{
			HorarioTeorico ht;
			ht.delay = stoi(tokens[6]);
			ht.cantidad_boletos_vendidos = stoi(tokens[7]);
			ht.horario = tokens[8];
			ht.arranco_dia_anterior = tokens[9] == "1";
			ht.retraso_acumulado = stoi(tokens[10]);
			ht.cant_pasajeros_parada_anterior = stoi(tokens[11]);
			ht.coord_este = stod(tokens[12]);
			ht.coord_norte = stod(tokens[13]);
			ht.distancia_parada_anterior = stod(tokens[14]);
			schedule[tokens[0]][tokens[1]][stoi(tokens[2])][tokens[3]][tokens[4]][stoi(tokens[5])] = ht;
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
};
