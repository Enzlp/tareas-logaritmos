import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import sys

def generar_graficos(archivo_qs, archivo_ms=None):
    """
    Genera gráficos comparativos entre Quicksort y Mergesort externo.
    
    Parámetros:
    - archivo_qs: Ruta al archivo CSV con resultados de Quicksort
    - archivo_ms: Ruta al archivo CSV con resultados de Mergesort (opcional)
    """
    # Cargar datos de Quicksort
    df_qs = pd.read_csv(archivo_qs)
    
    # Configurar estilo de los gráficos
    plt.style.use('ggplot')
    
    # Si tenemos datos de Mergesort
    if archivo_ms:
        df_ms = pd.read_csv(archivo_ms)
        
        # Gráfico de tiempo de ejecución
        plt.figure(figsize=(12, 6))
        plt.plot(df_qs['Tamaño'], df_qs['TiempoPromedio(s)'], 'o-', color='#FF5733', label='Quicksort Externo')
        plt.plot(df_ms['Tamaño'], df_ms['TiempoPromedio(s)'], 's-', color='#33A8FF', label='Mergesort Externo')
        plt.xlabel('Tamaño (millones de elementos)')
        plt.ylabel('Tiempo de ejecución (segundos)')
        plt.title('Comparación de tiempo de ejecución: Quicksort vs Mergesort')
        plt.grid(True, linestyle='--', alpha=0.7)
        plt.legend()
        plt.tight_layout()
        plt.savefig('grafico_tiempo_comparacion.png', dpi=300)
        
        # Gráfico de accesos a disco
        plt.figure(figsize=(12, 6))
        plt.plot(df_qs['Tamaño'], df_qs['AccesosIOPromedio'], 'o-', color='#FF5733', label='Quicksort Externo')
        plt.plot(df_ms['Tamaño'], df_ms['AccesosIOPromedio'], 's-', color='#33A8FF', label='Mergesort Externo')
        plt.xlabel('Tamaño (millones de elementos)')
        plt.ylabel('Número de accesos a disco')
        plt.title('Comparación de accesos a disco: Quicksort vs Mergesort')
        plt.grid(True, linestyle='--', alpha=0.7)
        plt.legend()
        plt.tight_layout()
        plt.savefig('grafico_io_comparacion.png', dpi=300)
        
        # Crear tabla comparativa
        resultados = pd.merge(
            df_qs.rename(columns={'TiempoPromedio(s)': 'Tiempo_QS', 'AccesosIOPromedio': 'IO_QS'}),
            df_ms.rename(columns={'TiempoPromedio(s)': 'Tiempo_MS', 'AccesosIOPromedio': 'IO_MS'}),
            on='Tamaño'
        )
        
        resultados['Ratio_Tiempo'] = resultados['Tiempo_QS'] / resultados['Tiempo_MS']
        resultados['Ratio_IO'] = resultados['IO_QS'] / resultados['IO_MS']
        
        resultados.to_csv('comparacion_algoritmos.csv', index=False)
        print("Comparación guardada en 'comparacion_algoritmos.csv'")
        
    else:
        # Solo Quicksort
        # Gráfico de tiempo de ejecución
        plt.figure(figsize=(12, 6))
        plt.plot(df_qs['Tamaño'], df_qs['TiempoPromedio(s)'], 'o-', color='#FF5733')
        plt.xlabel('Tamaño (millones de elementos)')
        plt.ylabel('Tiempo de ejecución (segundos)')
        plt.title('Tiempo de ejecución: Quicksort Externo')
        plt.grid(True, linestyle='--', alpha=0.7)
        plt.tight_layout()
        plt.savefig('grafico_tiempo_quicksort.png', dpi=300)
        
        # Gráfico de accesos a disco
        plt.figure(figsize=(12, 6))
        plt.plot(df_qs['Tamaño'], df_qs['AccesosIOPromedio'], 'o-', color='#FF5733')
        plt.xlabel('Tamaño (millones de elementos)')
        plt.ylabel('Número de accesos a disco')
        plt.title('Accesos a disco: Quicksort Externo')
        plt.grid(True, linestyle='--', alpha=0.7)
        plt.tight_layout()
        plt.savefig('grafico_io_quicksort.png', dpi=300)
    
    print("Gráficos generados exitosamente.")

if __name__ == "__main__":
    # Verificar argumentos
    if len(sys.argv) < 2:
        print("Uso: python generar_graficos.py archivo_quicksort.csv [archivo_mergesort.csv]")
        sys.exit(1)
    
    # Llamar a la función principal
    if len(sys.argv) == 3:
        generar_graficos(sys.argv[1], sys.argv[2])
    else:
        generar_graficos(sys.argv[1])