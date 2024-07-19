#include <calcular_horarios.h>
#include <iostream>
#include <mpi.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <unordered_map>
#include <map>

using namespace std;

const string HORARIOS_POR_PARADA = "horarios_por_parada.csv";

// Función para dividir una cadena en partes, dado un delimitador
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

// Función para procesar los horarios teóricos
LineaMap procesar_horarios_teoricos()
{
  string ruta_archivo_horarios_teoricos = HORARIOS_POR_PARADA;
  LineaMap lista_horarios_teoricos_parada;
  vector<string> lineas_a_evaluar = {"8870", "1111", "8872"};

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

    string linea = horario_teorico[1]; // cod_variante
    if (find(lineas_a_evaluar.begin(), lineas_a_evaluar.end(), linea) != lineas_a_evaluar.end())
    {
      string id_tipo_dia = horario_teorico[0];               // tipo_dia
      string id_recorrido = horario_teorico[2];              // frecuencia
      string id_parada = horario_teorico[3];                 // cod_ubic_parada
      int pos_recorrido = stoi(horario_teorico[4]);          // ordinal
      string horario = horario_teorico[5];                   // hora
      bool arranco_dia_anterior = horario_teorico[6] == "1"; // dia_anterior

      HorarioTeorico ht;
      ht.delay = -1;
      ht.cantidad_boletos_vendidos = 0;
      ht.pos_recorrido = pos_recorrido;
      ht.horario = horario;
      ht.arranco_dia_anterior = arranco_dia_anterior;

      lista_horarios_teoricos_parada[linea][id_tipo_dia][id_recorrido][id_parada] = ht;
    }
  }

  archivo_horarios_teoricos.close();
  return lista_horarios_teoricos_parada;
}
