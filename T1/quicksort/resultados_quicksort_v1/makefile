CC=g++
CFLAGS=-std=c++11 -Wall -O2
LIBS=-pthread

all: quicksort_externo experimentos_quicksort 

# Compilar el programa principal de Quicksort
quicksort_externo: main.cpp quicksort_externo.cpp quicksort_externo.h
	$(CC) $(CFLAGS) -o quicksort_externo main.cpp quicksort_externo.cpp $(LIBS)

# Compilar el programa de experimentos
experimentos_quicksort: experimentos_quicksort.cpp quicksort_externo.cpp quicksort_externo.h
	$(CC) $(CFLAGS) -o experimentos_quicksort experimentos_quicksort.cpp quicksort_externo.cpp $(LIBS)


# Limpiar archivos compilados y temporales
clean:
	rm -f quicksort_externo experimentos_quicksort encontrar_mejor_a *.bin *.tmp *.part* *.o

# Limpiar solo archivos temporales
clean-temp:
	rm -f *.bin *.tmp *.part*

# Ejecutar el programa principal con parámetros por defecto
run: quicksort_externo
	./quicksort_externo

# Ejecutar experimentos
experimentos: experimentos_quicksort
	./experimentos_quicksort

# Generar gráficos (requiere Python con pandas y matplotlib)
graficos:
	python generar_graficos.py resultados_quicksort.csv