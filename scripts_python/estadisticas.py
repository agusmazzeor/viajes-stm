import pandas as pd
import matplotlib.pyplot as plt
import numpy as np # Import numpy for array manipulation

def generar_plot():
    # Leer el archivo CSV
    df = pd.read_csv('/resultado/retrasos_de_lineas.csv')

    # Agrupar por recorrido y sumar los delays
    grouped = df.groupby(['linea', 'variante', 'tipo_dia', 'recorrido']).agg({'delay': 'sum'}).reset_index()

    # Obtener los 3 recorridos con más delay
    largest_3 = grouped.nlargest(3, 'delay')

    top_recorridos = largest_3['recorrido'].to_list()
    top_variantes = largest_3['variante'].to_list()
    tipos_dia = list(set(largest_3['tipo_dia'].to_list()))

    # Filtrar el DataFrame original para mantener solo las 3 top variantes
    df_top = df[df['variante'].isin(top_variantes)]
    df_top = df_top[df_top['recorrido'].isin(top_recorridos)]

    # # Obtener los tipos de día únicos y los recorridos únicos
    tipos_dia = df['tipo_dia'].unique()
    # recorridos = df_top['recorrido'].unique()
    # print(tipos_dia)
    # print(recorridos)

    # Crear una figura y ejes para los gráficos
    fig, axes = plt.subplots(len(tipos_dia), len(top_recorridos), figsize=(15, 10), sharey=True, sharex=True)
    fig.suptitle('Delay por Posición del Recorrido para cada Tipo de Día y Recorrido', fontsize=16)

    # Convert axes to a NumPy array for multi-dimensional indexing
    axes = np.array(axes) 

    # Reshape axes to 2D if necessary
    if axes.ndim == 1:
        axes = axes.reshape(len(tipos_dia), len(top_recorridos))

    # Crear un gráfico para cada tipo de día y cada recorrido
    for i, tipo_dia in enumerate(tipos_dia):
        for j, recorrido in enumerate(top_recorridos):
            ax = axes[i, j]  # Now you can index with a tuple
            
            subset = df[(df['tipo_dia'] == tipo_dia) & (df['recorrido'] == recorrido) & (df['variante'].isin(top_variantes))]
            subset = subset[subset['delay'] != -1]

            if not subset.empty: 
                ax.plot(subset['pos_recorrido'], subset['delay'], marker='o', linestyle='-')
            linea = subset['linea'].unique()
            if len(linea) > 0:
                linea = linea[0]
            else:
                linea = ''

            variante = subset['variante'].unique()
            if len(variante) > 0:
                variante = variante[0]
            else:
                variante = ''

            ax.set_title(f'Tipo de Día {tipo_dia}, Recorrido {recorrido}, Linea {linea}, Variante {variante}')
            ax.set_xlabel('Posición del Recorrido')
            ax.set_ylabel('Delay')

    # Ajustar el layout
    plt.tight_layout(rect=[0, 0.03, 1, 0.95])
    plt.savefig('/resultado/estadisticas/delay_por_posicion.png')
    plt.show()


if __name__ == "__main__":
    generar_plot()