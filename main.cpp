#include <mpi.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <chrono>
#include "types.h"
#include "calcular_horarios.h"
#include "funciones_auxiliares.h"
#include "serializers.h"

using namespace std;
using namespace std::chrono;

int main(int argc, char *argv[])
{
  MPI_Init(&argc, &argv);

  int rank, size;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  if (rank == 0)
  {
    // Master process

    // Iniciar el temporizador
    auto start_time = high_resolution_clock::now();

    LineaMap lista_horarios_teoricos_parada = procesar_horarios_teoricos(LINEA_OMNIBUS);

    // Serializar el mapa
    string horarios_teoricos_serializados;
    serialize_horarios_teoricos(lista_horarios_teoricos_parada, horarios_teoricos_serializados);

    // Enviar el tamaño del mapa serializado
    int schedule_size = horarios_teoricos_serializados.size();
    for (int i = 1; i < size; ++i)
    {
      MPI_Send(&schedule_size, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
    }

    // Enviar el mapa serializado de los horarios teoricos
    for (int i = 1; i < size; ++i)
    {
      MPI_Send(horarios_teoricos_serializados.data(), schedule_size, MPI_CHAR, i, 0, MPI_COMM_WORLD);
    }

    // Calcular chunks para procesar los viajes en los procesos esclavos
    ifstream file(DATOS_VIAJES);
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
        end += remainder; // El último chunk incluye el remanente

      int count = end - start;
      MPI_Send(&start, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
      MPI_Send(&count, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
    }

    // Obtener los resultados de los esclavos
    vector<DataViaje> all_viajes;
    for (int i = 1; i < size; ++i)
    {
      int viajes_size;
      MPI_Recv(&viajes_size, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      vector<char> viajes_buffer(viajes_size);
      MPI_Recv(viajes_buffer.data(), viajes_size, MPI_CHAR, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

      string viajes_serializados(viajes_buffer.begin(), viajes_buffer.end());
      vector<DataViaje> viajes = deserialize_viajes(viajes_serializados);
      all_viajes.insert(all_viajes.end(), viajes.begin(), viajes.end());
    }

    // Actualizar `cantidad_boletos_vendidos` y `delay` en `lista_horarios_teoricos_parada`
    for (const auto &viaje : all_viajes)
    {
      int dia_semana = obtener_dia_semana(viaje.fecha_evento);

      auto linea_it = lista_horarios_teoricos_parada.find(viaje.dsc_linea);
      if (linea_it != lista_horarios_teoricos_parada.end())
      {
        auto variante_it = linea_it->second.find(viaje.sevar_codigo);
        if (variante_it != linea_it->second.end())
        {
          auto dia_it = variante_it->second.find(dia_semana);
          if (dia_it != variante_it->second.end())
          {
            auto parada_it = dia_it->second.find(viaje.codigo_parada_origen);
            if (parada_it != dia_it->second.end())
            {
              auto recorrido_it = parada_it->second.find(viaje.recorrido);
              if (recorrido_it != parada_it->second.end())
              {
                auto &pos_recorrido_map = recorrido_it->second;
                auto pos_recorrido_it = pos_recorrido_map.find(viaje.pos_recorrido);
                if (pos_recorrido_it != pos_recorrido_map.end())
                {
                  HorarioTeorico &horario_teorico = pos_recorrido_it->second;
                  horario_teorico.cantidad_boletos_vendidos++;
                  if (horario_teorico.delay == -1 || viaje.delay < horario_teorico.delay)
                  {
                    horario_teorico.delay = viaje.delay;
                  }
                }
              }
            }
          }
        }
      }
    }

    // TODO: Recorrer todos los horarios teóricos y actualizar los nuevos campos

    // cout << "----------------------------" << endl;
    // cout << "Todos los viajes procesados:" << endl;
    // cout << "----------------------------" << endl;
    // print_data_viaje(all_viajes);

    // cout << "----------------------------" << endl;
    // cout << "Todas las lineas procesadas:" << endl;
    // cout << "----------------------------" << endl;
    // print_data_linea(lista_horarios_teoricos_parada);

    // Guardar `lista_horarios_teoricos_parada` en un archivo
    guardar_linea_map_en_archivo(lista_horarios_teoricos_parada, "resultado/retrasos_de_lineas.csv");

    // Parar el temporizador
    auto end_time = high_resolution_clock::now();
    auto duration = duration_cast<seconds>(end_time - start_time);
    cout << "Tiempo de ejecución total: " << duration.count() << " segundos" << endl;
    guardar_tiempo_de_ejecucion_en_archivo(duration.count(), "resultado/tiempo_de_ejecucion.txt");
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
    string horarios_teoricos_serializados(buffer.begin(), buffer.end());
    deserialize_horarios_teoricos(horarios_teoricos_serializados, lista_horarios_teoricos_parada);

    int start, count;
    MPI_Recv(&start, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Recv(&count, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    vector<DataViaje> viajes;
    procesar_viajes(DATOS_VIAJES, viajes, start, count, lista_horarios_teoricos_parada);

    // Serializar los datos de los viajes
    string viajes_serializados = serialize_viajes(viajes);
    int viajes_size = viajes_serializados.size();

    MPI_Send(&viajes_size, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    MPI_Send(viajes_serializados.data(), viajes_size, MPI_CHAR, 0, 0, MPI_COMM_WORLD);

    // cout << "---> Termine proceso con rank: " << rank << ", con cantidad de viajes: " << viajes.size() << endl;
  }

  MPI_Finalize();
  return 0;
}
