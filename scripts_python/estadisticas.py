import pandas as pd
import matplotlib.pyplot as plt
import numpy as np # Import numpy for array manipulation

def porcentajes():
    # Leer el archivo CSV
    df = pd.read_csv('/resultado/retrasos_de_lineas.csv')

    # Filtrar los datos donde el delay es válido y el tipo de día es 1 (día de la semana)
    df = df[df['delay'] != -1]
    df = df[df['tipo_dia'] == 1]

    # Buses que se atrasaron entre 5 y 15 minutos (300-900 segundos)
    delay_entre_5_y_15 = ((df['delay'] >= 300) & (df['delay'] <= 900)).sum()
    total_buses = len(df)
    porcentaje_delay_5_15 = (delay_entre_5_y_15 / total_buses) * 100

    # Omnibus donde la cantidad de boletos vendidos en la parada anterior es mayor a 5
    boletos_mayor_5 = (df['cant_pasajeros_parada_anterior'] > 5).sum()
    porcentaje_boletos_mayor_5 = (boletos_mayor_5 / total_buses) * 100

    # Omnibus donde la distancia con la parada anterior es menor a 200 metros
    distancia_menor_200 = (df['distancia_parada_anterior'] < 200).sum()
    porcentaje_distancia_menor_200 = (distancia_menor_200 / total_buses) * 100

    # Graficar los resultados
    labels = [
        'Delay 5-15 min',
        'Boletos > 5',
        'Distancia < 200 m'
    ]
    porcentajes = [
        porcentaje_delay_5_15,
        porcentaje_boletos_mayor_5,
        porcentaje_distancia_menor_200
    ]

    plt.figure(figsize=(7, 6))
    plt.bar(labels, porcentajes, color=['blue', 'green', 'orange'])
    plt.xlabel('Categoría')
    plt.ylabel('Porcentaje (%)')
    plt.title('Porcentaje de Buses en Diferentes Categorías')
    plt.ylim(0, 15)
    plt.grid(axis='y', linestyle='--', alpha=0.7)
    plt.tight_layout()
    plt.savefig('/resultado/estadisticas/porcentajes.png')

def paradas_mas_vendidas_dia_habil():
    # Leer el archivo CSV
    df = pd.read_csv('/resultado/retrasos_de_lineas.csv')

    df = df[df['delay'] != -1]
    tipo_dia = 1
    df = df[df['tipo_dia'] == tipo_dia]
    # filtar sacar las paradas que tengan mas de 15 minutos de delay
    df = df[df['delay'] <= 15*60]
    # Encontrar los recorridos con más delay en una parada
    top_recorridos = df.nlargest(200, 'delay')
    # Si las variantes se repiten en top recorridos, filtrar
    top_recorridos = top_recorridos.drop_duplicates(subset='variante')

    # Ordenar los datos para los gráficos
    sorted_top_paradas = top_recorridos.sort_values('parada', ascending=False)

    # Gráfico 1: Cantidad de boletos vendidos en la parada anterior
    plt.figure(figsize=(7, 6))
    plt.bar(sorted_top_paradas['parada'].astype(str), sorted_top_paradas['cant_pasajeros_parada_anterior'], color='g')
    plt.title('Boletos vendidos en la parada anterior en los recorridos con mayor retraso días hábiles')
    plt.xlabel('Parada del recorrido específico (Línea - Recorrido - Fecha - Parada)')
    plt.ylabel('Boletos vendidos parada anterior')
    plt.xticks(rotation=45)
    plt.tight_layout()
    plt.savefig('/resultado/estadisticas/habil_boletos_parada_anterior.png')

    # Gráfico 2: Retrasos en la parada
    plt.figure(figsize=(7, 6))
    plt.bar(sorted_top_paradas['parada'].astype(str), sorted_top_paradas['delay'], color='r')
    plt.title('Retraso en la parada actual en los recorridos con mayor retraso días hábiles')
    plt.xlabel('Parada del recorrido específico (Línea - Recorrido - Fecha - Parada)')
    plt.ylabel('Retraso en parada actual')
    plt.xticks(rotation=45)
    plt.tight_layout()
    plt.savefig('/resultado/estadisticas/habil_retraso_parada.png')

    # Gráfico 3: Retrasos en la parada anterior
    plt.figure(figsize=(7, 6))
    plt.bar(sorted_top_paradas['parada'].astype(str), sorted_top_paradas['retraso_parada_anterior'], color='r')
    plt.title('Retrasos en la parada anterior días hábiles')
    plt.xlabel('Parada del recorrido específico (Línea - Recorrido - Fecha - Parada)')
    plt.ylabel('Retraso parada anterior')
    plt.xticks(rotation=45)
    plt.tight_layout()
    plt.savefig('/resultado/estadisticas/habil_retraso_parada_anterior.png')

    # Gráfico 4: Distancia a la parada anterior
    plt.figure(figsize=(7, 6))
    plt.bar(sorted_top_paradas['parada'].astype(str), sorted_top_paradas['distancia_parada_anterior'], color='orange')
    plt.title('Distancia a la Parada Anterior días hábiles')
    plt.xlabel('Parada del recorrido específico (Línea - Recorrido - Fecha - Parada)')
    plt.ylabel('Distancia a la parada anterior')
    plt.xticks(rotation=45)
    plt.tight_layout()
    plt.savefig('/resultado/estadisticas/habil_distancia_con_parada_anterior.png')

def paradas_mas_vendidas_sabado():
    # Leer el archivo CSV
    df = pd.read_csv('/resultado/retrasos_de_lineas.csv')

    df = df[df['delay'] != -1]
    tipo_dia = 2
    df = df[df['tipo_dia'] == tipo_dia]
    df = df[df['delay'] <= 15*60]
    top_recorridos = df.nlargest(200, 'delay')
    top_recorridos = top_recorridos.drop_duplicates(subset='variante')

    # Ordenar los datos para los gráficos
    sorted_top_paradas = top_recorridos.sort_values('parada', ascending=False)

    # Gráfico 1: Cantidad de boletos vendidos en la parada anterior
    plt.figure(figsize=(7, 6))
    plt.bar(sorted_top_paradas['parada'].astype(str), sorted_top_paradas['cant_pasajeros_parada_anterior'], color='g')
    plt.title('Boletos vendidos en la parada anterior en los recorridos con mayor retraso día Sábado')
    plt.xlabel('Parada del recorrido específico (Línea - Recorrido - Fecha - Parada)')
    plt.ylabel('Boletos vendidos parada anterior')
    plt.xticks(rotation=45)
    plt.tight_layout()
    plt.savefig('/resultado/estadisticas/sabado_boletos_parada_anterior.png')

    # Gráfico 2: Retrasos en la parada
    plt.figure(figsize=(7, 6))
    plt.bar(sorted_top_paradas['parada'].astype(str), sorted_top_paradas['delay'], color='r')
    plt.title('Retraso en la parada actual en los recorridos con mayor retraso día Sábado')
    plt.xlabel('Parada del recorrido específico (Línea - Recorrido - Fecha - Parada)')
    plt.ylabel('Retraso en parada actual')
    plt.xticks(rotation=45)
    plt.tight_layout()
    plt.savefig('/resultado/estadisticas/sabado_retraso_parada.png')

    # Gráfico 3: Retrasos en la parada anterior
    plt.figure(figsize=(7, 6))
    plt.bar(sorted_top_paradas['parada'].astype(str), sorted_top_paradas['retraso_parada_anterior'], color='r')
    plt.title('Retrasos en la parada anterior día Sábado')
    plt.xlabel('Parada del recorrido específico (Línea - Recorrido - Fecha - Parada)')
    plt.ylabel('Retraso parada anterior')
    plt.xticks(rotation=45)
    plt.tight_layout()
    plt.savefig('/resultado/estadisticas/sabado_retraso_parada_anterior.png')

    # Gráfico 4: Distancia a la parada anterior
    plt.figure(figsize=(7, 6))
    plt.bar(sorted_top_paradas['parada'].astype(str), sorted_top_paradas['distancia_parada_anterior'], color='orange')
    plt.title('Distancia a la Parada Anterior día Sábado')
    plt.xlabel('Parada del recorrido específico (Línea - Recorrido - Fecha - Parada)')
    plt.ylabel('Distancia a la parada anterior')
    plt.xticks(rotation=45)
    plt.tight_layout()
    plt.savefig('/resultado/estadisticas/sabado_distancia_con_parada_anterior.png')

if __name__ == "__main__":
    porcentajes()
    paradas_mas_vendidas_dia_habil()
    paradas_mas_vendidas_sabado()
