#include <mpi.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include "types.h"
#include "calcular_horarios.h"
#include "funciones_auxiliares.h"
#include "serializers.h"

using namespace std;

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

int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    const string filename = "datos_dummy/05_2024_viajes.csv";

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
