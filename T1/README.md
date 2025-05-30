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

Luego dentro del docker se puede usar el siguiente comando, que toma el tamaño de M como párametro:

```
./main_tests 50
```

El segundo parametro indica la cantidad de memoria usada en este caso 50mb. Pero tambien se puede dejar el primer parametro solo y se asumira los 50mb por defecto:
```
./main_tests 
```

# Consideraciones

El desarrollo y la ejecución del trabajo se realizaron utilizando discos de estado sólido (SSD), lo cual mejoro en parte el rendimiento general del sistema. El uso de discos SSD permite una mayor velocidad de lectura y escritura en comparación con discos duros tradicionales (HDD), lo que resulta en que ejecutar esta tarea con discos HDD pueda tomarse mas tiempo que con discos SSD. Bajo estas condiciones, el programa completo en SSD tomó un poco mas de 4 horas en ejecutarse, dependiendo de la carga del sistema y las variaciones entre ejecuciones.

Cabe destacar que el procedimiento de búsqueda ternaria se ejecuta utilizando un límite de 50 MB de memoria principal como parametro, conforme a lo requerido por el enunciado. Esta restricción de memoria influye directamente en el tiempo de ejecución, haciendo que el proceso de búsqueda del valor óptimo de aridad sea considerablemente lento. Por esta razón, si bien la búsqueda se ejecuta automáticamente, se ha dejado comentada en el código la aridad óptima obtenida tras múltiples pruebas realizadas en nuestros sistemas, para facilitar la ejecución directa si no se quiere repetir el proceso completo.

Por ultimo, para ejecutar la tarea se uso una limitacion de memoria un poco mas holgada (90m en vez de 50m), ya que permitio que se puediera utilizar llamadas recursivas en el algoritmo de quicksort. Par esto se uso el siguiente comando de Docker:

```
docker run --rm -it -m 90m -v "$PWD":/workspace pabloskewes/cc4102-cpp-env bash
```