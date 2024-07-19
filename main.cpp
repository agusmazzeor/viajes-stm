#include <mpi.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <unordered_map>
#include <map>
#include "types.h"
// #include "calcular_horarios.h"

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

void load_chunk_data(const string &filename, vector<DataViaje> &data, int start, int count, const LineaMap &horarios_teoricos)
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
                vector<string> tokens = split(line, ';');
                DataViaje viaje;

                viaje.fecha_evento = tokens[2];
                viaje.cantidad_pasajeros = tokens[10];
                viaje.codigo_parada_origen = tokens[11];
                viaje.cod_empresa = tokens[12];
                viaje.linea_codigo = tokens[14];
                viaje.dsc_linea = tokens[15];
                viaje.sevar_codigo = tokens[16];

                // Agregar solo si sevar_codigo está en lineas_a_evaluar
                if (horarios_teoricos.find(viaje.sevar_codigo) != horarios_teoricos.end())
                {
                    data.push_back(viaje);
                }
            }
        }
    }
}

// TODO
double calculate_delay(const DataViaje &data, const LineaMap &horarios_teoricos)
{
    // Aquí debes implementar la lógica para calcular el retraso basado en los horarios teóricos
    return 0.0; // Retorna 0 si no se encuentra información correspondiente
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
            schedule[tokens[0]][tokens[1]][tokens[2]][tokens[3]] = ht;
        }
    }
}

void helper_function_print_linea_map(LineaMap &lista_horarios_teoricos_parada)
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
        cout << "------------------------" << endl;
    }
}

int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    const string filename = "05_2024_viajes.csv";

    if (rank == 0)
    {
        // Master process
        LineaMap lista_horarios_teoricos_parada = procesar_horarios_teoricos();

        // Serializar el mapa
        string serialized_schedule;
        serialize_horarios_teoricos(lista_horarios_teoricos_parada, serialized_schedule);

        // Enviar el tamaño del mapa serializado
        int schedule_size = serialized_schedule.size();
        for (int i = 1; i < size; ++i)
        {
            MPI_Send(&schedule_size, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
        }

        // Enviar el mapa serializado
        for (int i = 1; i < size; ++i)
        {
            MPI_Send(serialized_schedule.data(), schedule_size, MPI_CHAR, i, 0, MPI_COMM_WORLD);
        }

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
        // Recibir el tamaño del mapa serializado
        int schedule_size;
        MPI_Recv(&schedule_size, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // Recibir el mapa serializado
        vector<char> buffer(schedule_size);
        MPI_Recv(buffer.data(), schedule_size, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // Deserializar el mapa
        LineaMap lista_horarios_teoricos_parada;
        string serialized_schedule(buffer.begin(), buffer.end());
        deserialize_horarios_teoricos(serialized_schedule, lista_horarios_teoricos_parada);

        int start, count;
        MPI_Recv(&start, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&count, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        vector<DataViaje> slave_data;
        load_chunk_data(filename, slave_data, start, count, lista_horarios_teoricos_parada);

        print_data_viaje(slave_data);

        unordered_map<string, double> slave_delay_map;
        for (const auto &viaje : slave_data)
        {
            double delay = calculate_delay(viaje, lista_horarios_teoricos_parada);
            slave_delay_map[viaje.sevar_codigo] += delay;
        }

        vector<char> buffer_out;
        serialize_map(slave_delay_map, buffer_out);

        int map_size = buffer_out.size();
        MPI_Send(&map_size, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        MPI_Send(buffer_out.data(), map_size, MPI_CHAR, 0, 0, MPI_COMM_WORLD);

        cout << "Termine slave con rank: " << rank << ", con slave_delay_map de size: " << slave_delay_map.size() << endl;
    }

    MPI_Finalize();
    return 0;
}
