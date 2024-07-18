#include <mpi.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <unordered_map>

using namespace std;

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
