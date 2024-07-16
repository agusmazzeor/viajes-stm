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

void load_data(const string &filename, vector<StopData> &data)
{
    ifstream file(filename);
    string line;

    while (getline(file, line))
    {
        stringstream ss(line);
        StopData stop_data;
        ss >> stop_data.stop_id >> stop_data.theoretical_time >> stop_data.actual_time >> stop_data.avg_speed;
        data.push_back(stop_data);
    }
}

double calculate_delay(const StopData &data)
{
    return data.actual_time - data.theoretical_time;
}

int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    vector<StopData> data;
    const string filename = "stop_data.csv";

    if (rank == 0)
    {
        // Master process
        load_data(filename, data);

        int chunk_size = data.size() / size;
        for (int i = 1; i < size; ++i)
        {
            MPI_Send(&chunk_size, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(data.data() + i * chunk_size, chunk_size * sizeof(StopData), MPI_BYTE, i, 0, MPI_COMM_WORLD);
        }

        // Process master's chunk
        unordered_map<string, double> delay_map;
        for (int i = 0; i < chunk_size; ++i)
        {
            double delay = calculate_delay(data[i]);
            delay_map[data[i].stop_id] += delay;
        }

        // Collect results from slaves
        for (int i = 1; i < size; ++i)
        {
            unordered_map<string, double> slave_delay_map;
            MPI_Recv(&slave_delay_map, sizeof(slave_delay_map), MPI_BYTE, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            for (const auto &pair : slave_delay_map)
            {
                delay_map[pair.first] += pair.second;
            }
        }

        // Output results
        for (const auto &pair : delay_map)
        {
            cout << "Stop ID: " << pair.first << ", Total Delay: " << pair.second << " minutes" << endl;
        }
    }
    else
    {
        // Slave process
        int chunk_size;
        MPI_Recv(&chunk_size, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        vector<StopData> slave_data(chunk_size);
        MPI_Recv(slave_data.data(), chunk_size * sizeof(StopData), MPI_BYTE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        unordered_map<string, double> slave_delay_map;
        for (const auto &stop : slave_data)
        {
            double delay = calculate_delay(stop);
            slave_delay_map[stop.stop_id] += delay;
        }

        MPI_Send(&slave_delay_map, sizeof(slave_delay_map), MPI_BYTE, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}
