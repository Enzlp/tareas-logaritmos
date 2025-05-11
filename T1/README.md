# TAREA 1

# Explicacion del codigo

En esta carpeta de encuentra el codigo para la tarea 1. La estructura de este proyecto contiene varias carpetas y archivos que se usaron para toda la experimentacion requerida. Estos son los componentes:

- *file_generator*: esta carpeta contiene el archivo input_generator.cpp, aqui esta la funcion usada para generar un archivo de un tamano determinado con secuencia aleatoria

- *graphs*: esta carpeta contiene un archivo auxiliar en python. Este codigo es solo auxiliar, y se uso para obtener los graficos a partir de los datos obtenidos por la experimentacion.

- *mergesort*: carpeta con el archivo con la implementacion de mergesort externo. Aqui se puede ver el codigo y header del mergesort externo

- *misc*: carpeta con miscelaneos. Contiene el codigo usado para poder obtener el tamano de un bloque en memoria.

- *quicksort*: carpeta con los archivos de implementacion del algoritmo de quicksort externo.

- *main.cpp*: funcion main, usada para poder ejecutar todo el codigo en conjunto. Contiene no solo la experimentacion sino que tambien la funcion de busqueda para la aridad. Y una funcion auxiliar para exportar los datos como csv, para generar los graficos.


# Como ejecutar esta tarea

Para ejecutar la tarea es necesario primero compilar el programa fuera de el docker, ya que las limitaciones de memoria pueden dificultar los tiempos de compilacion. Para compilar se puede usar el siguiente comando:

```
g++ -O2 -o main_tests main.cpp mergesort/mergesort_externo.cpp file_generator/input_generator.cpp quicksort/quicksort_externo.cpp
```

Luego dentro del docker se puede usar el siguiente comando:

```
./main_tests 50
```

El segundo parametro indica la cantidad de memoria usada en este caso 50mb. Pero tambien se puede dejar el primer parametro solo y se asumira los 50mb por defecto:
```
./main_tests 
```

