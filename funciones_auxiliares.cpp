#include "funciones_auxiliares.h"

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

void print_data_linea(LineaMap &lista_horarios_teoricos_parada)
{
	// Imprimir algunos datos para verificar la estructura
	for (const auto &linea : lista_horarios_teoricos_parada)
	{
		cout << "Linea: " << linea.first << endl;
		for (const auto &tipo_dia : linea.second)
		{
			cout << "  Tipo de día: " << tipo_dia.first << endl;
			for (const auto &parada : tipo_dia.second)
			{
				cout << "    Parada: " << parada.first << endl;
				for (const auto &recorrido : parada.second)
				{
					const HorarioTeorico &ht = recorrido.second;
					cout << "      Recorrido: " << recorrido.first << endl;
					cout << "        Delay: " << ht.delay << endl;
					cout << "        Boletos vendidos: " << ht.cantidad_boletos_vendidos << endl;
					cout << "        Pos recorrido: " << ht.pos_recorrido << endl;
					cout << "        Horario: " << ht.horario << endl;
					cout << "        Arrancó día anterior: " << (ht.arranco_dia_anterior ? "Sí" : "No") << endl;
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
		cout << "------------------------" << endl;
	}
}
