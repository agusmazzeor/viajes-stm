#include "funciones_auxiliares.h"
#include <sstream>
#include <unistd.h>
#include <fstream>
#include <string>
#include <vector>
#include "types.h"

vector<string> split(const string &s, char delimiter)
{
	vector<string> tokens;
	string token;
	istringstream tokenStream(s);
	while (getline(tokenStream, token, delimiter))
	{
		tokens.push_back(token);
	}
	return tokens;
}

void guardar_linea_map_en_archivo(const LineaMap &linea_map, const string &filename)
{
	ofstream file(filename);
	if (!file.is_open())
	{
		cerr << "Error al abrir el archivo para escribir: " << filename << endl;
		return;
	}

	// Escribir los encabezados
	file << "linea,variante,tipo_dia,parada,recorrido,pos_recorrido,delay,cant_boletos,horario,arranco_dia_anterior,retraso_parada_anterior,cant_pasajeros_parada_anterior" << endl;

	for (const auto &linea : linea_map)
	{
		for (const auto &variante : linea.second)
		{
			for (const auto &dia : variante.second)
			{
				for (const auto &parada : dia.second)
				{
					for (const auto &recorrido : parada.second)
					{
						for (const auto &pos_recorrido : recorrido.second)
						{
							const HorarioTeorico &horario = pos_recorrido.second;
							file << linea.first << ","
								 << variante.first << ","
								 << dia.first << ","
								 << parada.first << ","
								 << recorrido.first << ","
								 << pos_recorrido.first << ","
								 << horario.delay << ","
								 << horario.cantidad_boletos_vendidos << ","
								 << horario.horario << ","
								 << horario.arranco_dia_anterior << ","
								 << horario.retraso_acumulado << ","
								 << horario.cant_pasajeros_parada_anterior << endl;
						}
					}
				}
			}
		}
	}

	file.close();
}

void guardar_linea_map_final_en_archivo(const LineaMapFinal &linea_map, const string &filename)
{
	ofstream file(filename);
	if (!file.is_open())
	{
		cerr << "Error al abrir el archivo para escribir: " << filename << endl;
		return;
	}

	// Escribir los encabezados
	file << "linea,variante,tipo_dia,parada,recorrido,pos_recorrido,delay,cant_boletos,horario,arranco_dia_anterior,retraso_parada_anterior,cant_pasajeros_parada_anterior" << endl;

	for (const auto &linea : linea_map)
	{
		for (const auto &variante : linea.second)
		{
			for (const auto &dia : variante.second)
			{
				for (const auto &recorrido : dia.second)
				{
					for (const auto &pos_recorrido : recorrido.second)
					{
						for (const auto &parada : pos_recorrido.second)
						{
							const HorarioTeorico &horario = parada.second;
							file << linea.first << ","
								 << variante.first << ","
								 << dia.first << ","
								 << parada.first << ","
								 << recorrido.first << ","
								 << pos_recorrido.first << ","
								 << horario.delay << ","
								 << horario.cantidad_boletos_vendidos << ","
								 << horario.horario << ","
								 << horario.arranco_dia_anterior << ","
								 << horario.retraso_acumulado << ","
								 << horario.cant_pasajeros_parada_anterior << endl;
						}
					}
				}
			}
		}
	}

	file.close();
}

void guardar_tiempo_de_ejecucion_en_archivo(const int &duration, const int &num_processes, const string &filename, const string &tipo_tiempo)
{
	ofstream file(filename, ios::app);
	if (!file.is_open())
	{
		cerr << "Error al abrir el archivo para escribir: " << filename << endl;
		return;
	}

	file << "Cant procesos: " << num_processes << ", tiempo de ejecucion (minutos): " << duration << ", tipo tiempo: " << tipo_tiempo << endl;
	file.close();
}

void print_data_linea(LineaMap &lista_horarios_teoricos_parada)
{
	// Imprimir algunos datos para verificar la estructura
	for (const auto &linea : lista_horarios_teoricos_parada)
	{
		cout << "Linea: " << linea.first << endl;
		for (const auto &variante : linea.second)
		{
			cout << "  Variante: " << variante.first << endl;
			for (const auto &tipo_dia : variante.second)
			{
				cout << "    Tipo de día: " << tipo_dia.first << endl;
				for (const auto &parada : tipo_dia.second)
				{
					cout << "      Parada: " << parada.first << endl;
					for (const auto &recorrido : parada.second)
					{
						cout << "        Recorrido: " << recorrido.first << endl;
						for (const auto &pos_recorrido : recorrido.second)
						{
							const HorarioTeorico &ht = pos_recorrido.second;
							cout << "          Pos recorrido: " << pos_recorrido.first << endl;
							cout << "            Delay: " << ht.delay << endl;
							cout << "            Boletos vendidos: " << ht.cantidad_boletos_vendidos << endl;
							cout << "            Horario: " << ht.horario << endl;
							cout << "            Arrancó día anterior: " << (ht.arranco_dia_anterior ? "Sí" : "No") << endl;
							cout << "            Retraso parada anterior: " << ht.retraso_acumulado << endl;
							cout << "            Cant. boletos parada anterior: " << ht.cant_pasajeros_parada_anterior << endl;
						}
					}
				}
			}
		}
	}
}

void print_data_viaje(const vector<DataViaje> &data_viajes)
{
	for (const auto &viaje : data_viajes)
	{
		cout << "Fecha del evento: " << viaje.fecha_evento << endl;
		cout << "Cantidad de pasajeros: " << viaje.cantidad_pasajeros << endl;
		cout << "Código de parada origen: " << viaje.codigo_parada_origen << endl;
		cout << "Código de la empresa: " << viaje.cod_empresa << endl;
		cout << "Código de la línea: " << viaje.linea_codigo << endl;
		cout << "Descripción de la línea: " << viaje.dsc_linea << endl;
		cout << "Código de variante (sevar_codigo): " << viaje.sevar_codigo << endl;
		cout << "Recorrido omnibus: " << viaje.recorrido << endl;
		cout << "Delay omnibus (minutos): " << viaje.delay << endl;
		cout << "Pos recorrido omnibus: " << viaje.pos_recorrido << endl;
		cout << "------------------------" << endl;
	}
}

void combinar_linea_maps(LineaMap &dest, const LineaMap &src)
{
	for (const auto &linea : src)
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
							const HorarioTeorico &ht_src = pos_recorrido.second;
							auto &ht_dest = dest[linea.first][variante.first][id_tipo_dia.first][id_parada.first][id_recorrido.first][pos_recorrido.first];

							if (ht_dest.delay == -1 || (ht_src.delay != -1 && ht_src.delay < ht_dest.delay))
							{
								ht_dest.delay = ht_src.delay;
							}
							ht_dest.cantidad_boletos_vendidos += ht_src.cantidad_boletos_vendidos;
						}
					}
				}
			}
		}
	}
};

string obtener_derectorio_actual()
{
	char cwd[1024];
	if (getcwd(cwd, sizeof(cwd)) != NULL)
	{
		return string(cwd);
	}
	else
	{
		cerr << "Error al obtener el directorio actual" << endl;
		return "";
	}
}
