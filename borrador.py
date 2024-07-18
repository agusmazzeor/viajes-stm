RUTA_ARCHIVO_HORARIOS_TEORICOS = "asd.csv"
RUTA_ARCHIVO_VELOCIDADES_PROMEDIO_EN_AVENIDAS = "asd.csv"
# Linea de omnibus -> decision nuestra
# ids paradas y lista avenidas: Identificar las paradas y avenidas de su recorrido -> de dataset paradas.csv
# ids paradas y lista avenidas: Filtrar por avenida ---> cruzar con las avenidas del dataset de velocidades promedio (velocidad.csv)
# lista viajes -> filtrar por linea e id paradas (viajes.csv)

# VIAJES
# id_viaje,con_tarjeta,fecha_evento,tipo_viaje,descripcion_tipo_viaje,grupo_usuario,descripcion_grupo_usuario,
# grupo_usuario_especifico,descripcion_grupo_usuario_espe,ordinal_de_tramo,cantidad_pasajeros,codigo_parada_origen,
# cod_empresa,descrip_empresa,linea_codigo,dsc_linea,sevar_codigo

# HORARIOS TEORICOS
# tipo_dia;cod_variante;frecuencia;cod_ubic_parada;ordinal;hora;dia_anterior

# procesar viajes (en procesos esclavos)
def procesar_horarios_teoricos():
  lineas_a_evaluar = ["104"]
  lista_horarios_teoricos_parada = {}
  delay = -1
  cantidad_boletos_vendidos = 0
  # avenidas_de_interes = obtener_avenidas() # por ahora no
  # paradas_de_interes = obtener_paradas(avenidas_de_interes) # por ahora no

  # Recorrer los horarios teóricos y quedarnos solo con aquellos que pertenezcan a las paradas relevantes
  archivo_horarios_teoricos = open(RUTA_ARCHIVO_HORARIOS_TEORICOS)
  horarios_teoricos = archivo_horarios_teoricos.readlines()
  archivo_horarios_teoricos.close()

  for _horario_teorico in horarios_teoricos:
    horario_teorico = _horario_teorico.split(";")
    linea = horario_teorico[1] # cod_variante
    if linea in lineas_a_evaluar:
      id_tipo_dia = horario_teorico[0] # tipo_dia
      id_recorrido = horario_teorico[2] # frecuencia
      id_parada = horario_teorico[3] # cod_ubic_parada
      pos_recorrido = horario_teorico[4] # ordinal
      horario = horario_teorico[5] # hora
      arranco_dia_aterior = horario_teorico[6] # dia_anterior

      lista_horarios_teoricos_parada[linea][id_tipo_dia][id_recorrido][id_parada] = [
        delay,
        cantidad_boletos_vendidos,
        pos_recorrido,
        horario,
        arranco_dia_aterior
      ]

  return lista_horarios_teoricos_parada

# # Para setear delay
# lista_horarios_teoricos_parada[linea][id_tipo_dia][id_recorrido][id_parada][0] = min(actual, delay_boleto)
# # Para setear cantidad_boletos_vendidos
# lista_horarios_teoricos_parada[linea][id_tipo_dia][id_recorrido][id_parada][1] ++

def obtener_avenidas():
  lista_avenidas = {}
  archivo_velocidades_promedio_en_avenidas = open(RUTA_ARCHIVO_VELOCIDADES_PROMEDIO_EN_AVENIDAS)
  velocidades_promedio_en_avenidas = archivo_velocidades_promedio_en_avenidas.readlines()
  archivo_velocidades_promedio_en_avenidas.close()

  for fila in velocidades_promedio_en_avenidas:
    codigo_avenida = fila[1]
    lista_avenidas.append(codigo_avenida)

  return lista_avenidas


def obtener_paradas(avenidas_de_interes):
  "hello"





# C++
# //  VIAJES
# //  id_viaje,con_tarjeta,fecha_evento,tipo_viaje,descripcion_tipo_viaje,grupo_usuario,descripcion_grupo_usuario,
# //  grupo_usuario_especifico,descripcion_grupo_usuario_espe,ordinal_de_tramo,cantidad_pasajeros,codigo_parada_origen,
# //  cod_empresa,descrip_empresa,linea_codigo,dsc_linea,sevar_codigo

# struct DataViaje
# {
#     string fecha_evento; // horario
#     string cantidad_pasajeros;
#     string codigo_parada_origen; // codigo de la parada de ascenso
#     string cod_empresa;          // codigo empresa a la cual pertenece el omnibus
#     string linea_codigo;         // codigo de la linea
#     string dsc_linea;            // nombre publico de la linea
#     string sevar_codigo;         // codigo de la variante
# };