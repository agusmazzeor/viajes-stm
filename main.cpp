#include <mpi.h>
#include <iostream>
#include <unistd.h>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <chrono>
#include "src/types.h"
#include "src/calcular_horarios.h"
#include "src/funciones_auxiliares.h"
#include "src/serializers.h"

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
    cout << "Master      ---> Arranco" << endl;
    // Iniciar el temporizador
    auto start_time = high_resolution_clock::now();

    LineaMap lista_horarios_teoricos_parada = procesar_horarios_teoricos();

    // Serializar el mapa
    string horarios_teoricos_serializados;
    serialize_horarios_teoricos(lista_horarios_teoricos_parada, horarios_teoricos_serializados);

    auto end_time_pre = high_resolution_clock::now();
    auto duration_pre = duration_cast<minutes>(end_time_pre - start_time);
    cout << "Tiempo de preprocesamiento: " << duration_pre.count() << " minutos" << endl;
    guardar_tiempo_de_ejecucion_en_archivo(duration_pre.count(), size, "resultado/tiempo_de_ejecucion.txt", "preprocesamiento");
    cout << "Master      ---> Envio horarios teoricos a los esclavos" << endl;
    auto start_time_procesamiento = high_resolution_clock::now();
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
    cout << "Master      ---> Termino de enviar horarios teoricos a los esclavos" << endl;
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

    vector<int> v;

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
    cout << "Master      ---> Termino de enviar cuantos viajes le toca procesar a cada esclavos" << endl;
    LineaMapFinal resultado_linea_map_final;

    cout << "Master      ---> Espero recibir nuevos horarios de los esclavos" << endl;
    // Obtener los resultados de los esclavos
    for (int i = 1; i < size; ++i)
    {
      int viajes_size;
      MPI_Recv(&viajes_size, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      vector<char> buffer(viajes_size);
      MPI_Recv(buffer.data(), viajes_size, MPI_CHAR, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      cout << "Master      ---> Recibi horarios de un esclavo" << endl;
      // Deserializar el mapa
      LineaMapFinal linea_map_esclavo;
      string horarios_teoricos_serializados(buffer.begin(), buffer.end());
      deserialize_horarios_teoricos_final(horarios_teoricos_serializados, linea_map_esclavo);
      cout << "Master      ---> Combino horarios de un esclavo con los resultados finales" << endl;
      combinar_linea_maps(resultado_linea_map_final, linea_map_esclavo);
      cout << "Master      ---> Termine de combinar con los resultados finales" << endl;
    }
    cout << "Master      ---> Termine de recibir nuevos horarios de los esclavos" << endl;
    auto end_time_procesamiento = high_resolution_clock::now();
    auto duration_procesamiento = duration_cast<minutes>(end_time_procesamiento - start_time_procesamiento);
    cout << "Tiempo de procesamiento: " << duration_procesamiento.count() << " minutos" << endl;
    guardar_tiempo_de_ejecucion_en_archivo(duration_procesamiento.count(), size, "resultado/tiempo_de_ejecucion.txt", "procesamiento");

    // // Recorrer todos los horarios teóricos y actualizar los nuevos campos
    // LineaMapFinal linea_map_final;
    // convertir_linea_map_a_map_linea(resultado_linea_map_final, linea_map_final);

    cout << "Master      ---> Recorro horarios teoricos segun pos_recorrido" << endl;
    // variante, parada, recorrido, pos_recorrido, delay, retraso_parada_anterior
    for (auto &linea : resultado_linea_map_final)
    {
      for (auto &variante : linea.second)
      {
        for (auto &tipo_dia : variante.second)
        {
          for (auto &fecha : tipo_dia.second)
          {
            for (auto &recorrido : fecha.second)
            {
              int retraso_acumulado_anterior = -1;
              int cant_pasajeros_parada_anterior = 0;

              for (auto &pos_recorrido : recorrido.second)
              {
                for (auto &parada : pos_recorrido.second)
                {
                  HorarioTeorico &ht = parada.second;
                  ht.retraso_acumulado = retraso_acumulado_anterior;
                  ht.cant_pasajeros_parada_anterior = cant_pasajeros_parada_anterior;

                  // Actualizar los valores acumulados para la siguiente parada
                  retraso_acumulado_anterior = ht.delay;
                  cant_pasajeros_parada_anterior = ht.cantidad_boletos_vendidos;
                }
              }
            }
          }
        }
      }
    };

    // cout << "----------------------------" << endl;
    // cout << "Todos los viajes procesados:" << endl;
    // cout << "----------------------------" << endl;
    // print_data_viaje(all_viajes);

    // cout << "----------------------------" << endl;
    // cout << "Todas las lineas procesadas:" << endl;
    // cout << "----------------------------" << endl;
    // print_data_linea(lista_horarios_teoricos_parada);

    cout << "Master      ---> Guardo horarios teoricos finales" << endl;
    // Guardar `lista_horarios_teoricos_parada` en un archivo
    guardar_linea_map_final_en_archivo(resultado_linea_map_final, "resultado/retrasos_de_lineas.csv");

    // Aca correr estadisticas
    // string current_working_dir = obtener_derectorio_actual();
    // string python_script = current_working_dir + "/scripts_python/estadisticas.py";
    // string command = "python3 " + python_script;
    // system(command.c_str());

    // Parar el temporizador
    auto end_time = high_resolution_clock::now();
    auto duration = duration_cast<minutes>(end_time - start_time);
    cout << "Tiempo de ejecución total: " << duration.count() << " minutos, cant procesos: " << size << endl;
    guardar_tiempo_de_ejecucion_en_archivo(duration.count(), size, "resultado/tiempo_de_ejecucion.txt", "total");
  }
  else
  {
    // Proceso esclavo
    cout << "Esclavo (" << rank << ") ---> Arranco" << endl;

    cout << "Esclavo (" << rank << ") ---> Espero recibir horarios teoricos" << endl;
    // Recibir el tamaño del mapa serializado
    int schedule_size;
    MPI_Recv(&schedule_size, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    // Recibir el mapa serializado
    vector<char> buffer(schedule_size);
    MPI_Recv(buffer.data(), schedule_size, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    cout << "Esclavo (" << rank << ") ---> Termine de recibir horarios teoricos" << endl;

    // Deserializar el mapa
    LineaMap lista_horarios_teoricos_parada;
    string horarios_teoricos_serializados(buffer.begin(), buffer.end());
    deserialize_horarios_teoricos(horarios_teoricos_serializados, lista_horarios_teoricos_parada);

    cout << "Esclavo (" << rank << ") ---> Espero recibir rango de viajes a evaluar" << endl;
    int start, count;
    MPI_Recv(&start, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Recv(&count, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    cout << "Esclavo (" << rank << ") ---> Recibo start: " << start << ", count: " << count << endl;

    cout << "Esclavo (" << rank << ") ---> Proceso viajes" << endl;
    vector<DataViaje> viajes;
    procesar_viajes(DATOS_VIAJES, viajes, start, count, lista_horarios_teoricos_parada);
    cout << "Esclavo (" << rank << ") ---> Termino de procesar viajes" << endl;
    // Pasar viajes a linea map
    // Actualizar `cantidad_boletos_vendidos` y `delay` en `lista_horarios_teoricos_parada`

    cout << "Esclavo (" << rank << ") ---> Creo horarios_final en base a viajes" << endl;
    LineaMapFinal horarios_final;
    for (const auto &viaje : viajes)
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

                  string parsed_fecha = convertir_fecha_a_ymd(viaje.fecha_evento);
                  // en vez de sumar todo, creamos la linea map final y le agregamos el horario teorico
                  if (existe_entrada(horarios_final, viaje.dsc_linea, viaje.sevar_codigo, to_string(dia_semana), parsed_fecha, viaje.recorrido, viaje.pos_recorrido, viaje.codigo_parada_origen))
                  {
                    HorarioTeorico &horario_teorico_existente = horarios_final[viaje.dsc_linea][viaje.sevar_codigo][to_string(dia_semana)][parsed_fecha][viaje.recorrido][viaje.pos_recorrido][viaje.codigo_parada_origen];
                    //  agregar cantidad de boletos vendidos, promediamos el delay

                    horario_teorico_existente.cantidad_boletos_vendidos++;
                    if (horario_teorico_existente.delay == -1 || viaje.delay < horario_teorico_existente.delay)
                    {
                      horario_teorico_existente.delay = viaje.delay;
                    }
                  }
                  else
                  {
                    HorarioTeorico horario_teorico_final;
                    horario_teorico_final.delay = viaje.delay;
                    horario_teorico_final.cantidad_boletos_vendidos = 1;
                    horario_teorico_final.horario = horario_teorico.horario;
                    horario_teorico_final.arranco_dia_anterior = horario_teorico.arranco_dia_anterior;
                    horario_teorico_final.retraso_acumulado = horario_teorico.retraso_acumulado;
                    horario_teorico_final.cant_pasajeros_parada_anterior = horario_teorico.cant_pasajeros_parada_anterior;
                    horario_teorico_final.coord_este = horario_teorico.coord_este;
                    horario_teorico_final.coord_norte = horario_teorico.coord_norte;
                    horario_teorico_final.distancia_parada_anterior = horario_teorico.distancia_parada_anterior;
                    horarios_final[viaje.dsc_linea][viaje.sevar_codigo][to_string(dia_semana)][parsed_fecha][viaje.recorrido][viaje.pos_recorrido][viaje.codigo_parada_origen] = horario_teorico_final;
                  }
                }
              }
            }
          }
        }
      }
    }
    calcular_distancia_parada_anterior(horarios_final);

    cout << "Esclavo (" << rank << ") ---> horarios_final size: " << horarios_final.size() << endl;
    // Serializar los horarios teoricos
    string horarios_teoricos_con_viajes_serializados;
    serialize_horarios_teoricos_final(horarios_final, horarios_teoricos_con_viajes_serializados);

    int size = horarios_teoricos_con_viajes_serializados.size();
    cout << "Esclavo (" << rank << ") ---> horarios_final serializados size: " << size << endl;
    MPI_Send(&size, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    MPI_Send(horarios_teoricos_con_viajes_serializados.data(), size, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
    cout << "Esclavo (" << rank << ") ---> Termino de enviar horarios teoricos nuevos" << endl;
  }

  MPI_Finalize();
  return 0;
}
