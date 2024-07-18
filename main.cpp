#include <mpi.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <unordered_map>
#include <map>

using namespace std;

// Definición de la estructura para la información de los horarios teóricos de una parada
struct HorarioTeorico
{
    int delay;
    int cantidad_boletos_vendidos;
    int pos_recorrido;
    string horario;
    bool arranco_dia_anterior;
};

// Definición de los tipos de datos anidados
using ParadaMap = map<string, HorarioTeorico>; // clave: id_parada
using RecorridoMap = map<string, ParadaMap>;   // clave: id_recorrido
using TipoDiaMap = map<string, RecorridoMap>;  // clave: id_tipo_dia
using LineaMap = map<string, TipoDiaMap>;      // clave: linea
// lista_horarios_teoricos_parada[linea][id_tipo_dia][id_recorrido][id_parada] = [delay,cantidad_boletos_vendidos,pos_recorrido,horario,arranco_dia_aterior]

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
    string ruta_archivo_horarios_teoricos = "horarios_por_parada.csv";
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

struct StopData
{
    string stop_id;
    double theoretical_time;
    double actual_time;
    double avg_speed;
};

void load_chunk_data(const string &filename, vector<StopData> &data, int start, int count)
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
                stringstream ss(line);
                StopData stop_data;
                string temp;

                // Leer los campos de cada línea
                getline(ss, stop_data.stop_id, ',');
                getline(ss, temp, ',');
                stop_data.theoretical_time = stod(temp);
                getline(ss, temp, ',');
                stop_data.actual_time = stod(temp);
                getline(ss, temp, ',');
                stop_data.avg_speed = stod(temp);

                // Agregar los datos al vector
                data.push_back(stop_data);
            }
        }
    }
}

double calculate_delay(const StopData &data)
{
    return data.actual_time - data.theoretical_time;
}

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

void helper_function(LineaMap &lista_horarios_teoricos_parada)
{
    // Imprimir algunos datos para verificar la estructura
    for (const auto &linea : lista_horarios_teoricos_parada)
    {
        cout << "Linea: " << linea.first << endl;
        for (const auto &tipo_dia : linea.second)
        {
            cout << "  Tipo de día: " << tipo_dia.first << endl;
            for (const auto &recorrido : tipo_dia.second)
            {
                cout << "    Recorrido: " << recorrido.first << endl;
                for (const auto &parada : recorrido.second)
                {
                    const HorarioTeorico &ht = parada.second;
                    cout << "      Parada: " << parada.first << endl;
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

int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    const string filename = "stop_data.csv";

    if (rank == 0)
    {
        // Master process

        LineaMap lista_horarios_teoricos_parada = procesar_horarios_teoricos();

        ifstream file(filename);
        string line;
        int total_lines = 0;

        // Contar el número total de líneas (excluyendo el encabezado)
        if (getline(file, line))
        {
            while (getline(file, line))
            {
                ++total_lines;
            }
        }

        int chunk_size = total_lines / (size - 1);
        int remainder = total_lines % (size - 1);

        for (int i = 1; i < size; ++i)
        {
            int start = (i - 1) * chunk_size;
            int end = start + chunk_size;
            if (i == size - 1)
                end += remainder; // El ultimo chunk incluye el remanente

            int count = end - start;
            MPI_Send(&start, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(&count, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
        }

        // Obtener los resultados de los esclavos
        unordered_map<string, double> delay_map;
        for (int i = 1; i < size; ++i)
        {
            int map_size;
            MPI_Recv(&map_size, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            vector<char> buffer(map_size);
            MPI_Recv(buffer.data(), map_size, MPI_CHAR, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            unordered_map<string, double> slave_delay_map;
            deserialize_map(buffer, slave_delay_map);
            for (const auto &pair : slave_delay_map)
            {
                delay_map[pair.first] += pair.second;
            }
        }

        // Imprimir resultados
        for (const auto &pair : delay_map)
        {
            cout << "Stop ID: " << pair.first << ", Total Delay: " << pair.second << " minutes" << endl;
        }
        cout << "Termine master" << endl;
    }
    else
    {
        // Proceso esclavo
        int start, count;
        MPI_Recv(&start, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&count, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        vector<StopData> slave_data;
        load_chunk_data(filename, slave_data, start, count);

        unordered_map<string, double> slave_delay_map;
        for (const auto &stop : slave_data)
        {
            double delay = calculate_delay(stop);
            slave_delay_map[stop.stop_id] += delay;
        }

        vector<char> buffer;
        serialize_map(slave_delay_map, buffer);

        int map_size = buffer.size();
        MPI_Send(&map_size, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        MPI_Send(buffer.data(), map_size, MPI_CHAR, 0, 0, MPI_COMM_WORLD);

        cout << "Termine slave con rank: " << rank << ", con slave_delay_map de size: " << slave_delay_map.size() << endl;
    }

    MPI_Finalize();
    return 0;
}
