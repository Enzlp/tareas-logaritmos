import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import sys
import os

"""
Genera graficos de experimentos hechos con mergesort externo y quicksort externo
@params archivo_csv ruta archivo resultados
"""
def generar_grafico(archivo_csv, y_label, x_label, title):
    # Cargar datos del archivo CSV
    df = pd.read_csv(archivo_csv)

    # Verificar que las columnas esperadas estén presentes
    if df.columns.size < 2:
        print(f"Archivo {archivo_csv} no tiene suficientes columnas.")
        return

    y = df.iloc[:, 0]  # Primera columna (tiempo o IO)
    x = df.iloc[:, 1]  # Segunda columna (N)

    # Configurar estilo del gráfico
    plt.style.use('ggplot')
    plt.figure(figsize=(12, 6))
    plt.plot(x, y, marker='o', linestyle='-', color='blue')

    # Etiquetas y título
    plt.xlabel(x_label)
    plt.ylabel(y_label)
    plt.title(title)
    plt.grid(True, linestyle='--', alpha=0.7)
    plt.legend()
    plt.tight_layout()

    # Guardar gráfico como imagen
    nombre_salida = f"{archivo_csv.replace('.csv', '.png')}"
    plt.savefig(nombre_salida, dpi=300)
    plt.close()
    
    print(f"Gráfico guardado en {nombre_salida}")

if __name__ == "__main__":
    archivos = [
        ("io_ms.csv", "Accesos a disco I/O", "Tamaño de N", "Accesos I/O para mergesort"),
        ("time_ms.csv", "Tiempo de Ejecución [s]", "Tamaño de N", "Tiempo de ejecucion para mergesort"),
        ("io_qs.csv", "Accesos a disco I/O", "Tamaño de N", "Accesos I/O para quicksort"),
        ("time_qs.csv", "Tiempo de Ejecución [s]", "Tamaño de N", "Tiempo de ejecucion para quicksort"),
    ]

    for archivo, y_label, x_label, title in archivos:
        if os.path.exists(archivo):
            generar_grafico(archivo, y_label, x_label, title)
        else:
            print(f"[ERROR] No se encontró el archivo {archivo}")