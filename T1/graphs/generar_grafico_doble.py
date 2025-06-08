import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import sys
import os

"""
Genera graficos de experimentos hechos con mergesort externo y quicksort externo
@param archivo_csv ruta archivo resultados
@param y_label nombre de eje y
@param x_label nombre de eje x
@param title titulo del grafico
"""
def generar_grafico_doble(arch_1, arch_2, y_label, x_label, title, legend_1, legend_2):
    df1 = pd.read_csv(arch_1, header=None)
    df2 = pd.read_csv(arch_2, header=None)

    if df1.columns.size < 2:
        print(f"Archivo {arch_1} no tiene suficientes columnas.")
        return

    if df2.columns.size < 2:
        print(f"Archivo {arch_2} no tiene suficientes columnas.")
        return

    x1 = df1.iloc[:, 0]  # Primera columna
    y1 = df1.iloc[:, 1]  # Segunda columna
    x2 = df2.iloc[:, 0]  # Primera columna
    y2 = df2.iloc[:, 1]  # Segunda columna

    # Configurar estilo del gráfico
    plt.style.use('ggplot')
    plt.figure(figsize=(12, 6))

    # Plotear el primer dataset
    plt.plot(x1, y1, marker='o', linestyle='-', color='blue', label=legend_1)

    # Plotear el segundo dataset
    plt.plot(x2, y2, marker='s', linestyle='--', color='red', label=legend_2)

    # Etiquetas y título
    plt.xlabel(x_label)
    plt.ylabel(y_label)
    plt.title(title)
    plt.grid(True, linestyle='--', alpha=0.7)
    plt.legend()  # Mostrar leyenda
    plt.tight_layout()

    # Guardar gráfico como imagen
    nombre_salida = f"{title.replace(' ', '_')}.png" # Usar el título para el nombre del archivo
    plt.savefig(nombre_salida, dpi=300)
    plt.close()

    print(f"Gráfico comparativo guardado en {nombre_salida}")


if __name__ == "__main__":
    archivos = [
        ("io_ms.csv", "io_qs.csv", "Accesos a disco I/O", "Tamaño de N", "Accesos IO", "MergeSort", "QuickSort"),
        ("time_ms.csv", "time_qs.csv", "Tiempo de Ejecución [s]", "Tamaño de N", "Tiempo de ejecucion", "MergeSort", "QuickSort"),
    ]

    for arch_1, arch_2, y_label, x_label, title, legend_1, legend_2 in archivos:
        if os.path.exists(arch_1) and os.path.exists(arch_2):
            generar_grafico_doble(arch_1, arch_2, y_label, x_label, title, legend_1, legend_2)
        else:
            print(f"[ERROR] No se encontró un archivo")

