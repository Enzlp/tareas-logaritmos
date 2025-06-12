import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import sys
import os

"""
Genera gráficos tiempo vs N usando CSVs que contienen solo una columna con los tiempos.
Los valores de N se asumen conocidos (vector hardcodeado).
"""

def generar_grafico(archivo_csv, y_label, x_label, title):
    # Valores de N usados en el experimento
    N = [32, 64, 128, 256, 512, 1024, 2048, 4096]

    # Cargar datos del archivo CSV sin encabezado
    try:
        df = pd.read_csv(archivo_csv, header=None)
    except Exception as e:
        print(f"[ERROR] No se pudo leer el archivo {archivo_csv}: {e}")
        return

    if len(df) != len(N):
        print(f"[ERROR] El archivo {archivo_csv} contiene {len(df)} filas, pero se esperaban {len(N)}.")
        return

    x = N
    y = df.iloc[:, 0]  # Tiempos

    # Configurar estilo del gráfico
    plt.style.use('ggplot')
    plt.figure(figsize=(12, 6))
    plt.plot(x, y, marker='o', linestyle='-', color='blue')

    # Etiquetas y título
    plt.xlabel(x_label)
    plt.ylabel(y_label)
    plt.title(title)
    plt.grid(True, linestyle='--', alpha=0.7)
    plt.tight_layout()

    # Guardar gráfico como imagen
    nombre_salida = archivo_csv.replace('.csv', '.png')
    plt.savefig(nombre_salida, dpi=300)
    plt.close()

    print(f"Gráfico guardado en {nombre_salida}")


if __name__ == "__main__":
    archivos = [
        ("time_avg_arreglo_no_opti.csv", "Tiempo [s]", "Tamaño de N", "Kruskal con arreglo (sin optimización)"),
        ("time_avg_heap_no_opti.csv", "Tiempo [s]", "Tamaño de N", "Kruskal con heap (sin optimización)"),
        ("time_avg_arreglo_opti.csv", "Tiempo [s]", "Tamaño de N", "Kruskal con arreglo (con optimización)"),
        ("time_avg_heap_opti.csv", "Tiempo [s]", "Tamaño de N", "Kruskal con heap (con optimización)"),
    ]

    for archivo, y_label, x_label, title in archivos:
        if os.path.exists(archivo):
            generar_grafico(archivo, y_label, x_label, title)
        else:
            print(f"[ERROR] No se encontró el archivo {archivo}")

