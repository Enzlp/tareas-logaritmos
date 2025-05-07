// quicksort_externo.h
// Archivo de cabecera para Quicksort Externo que incluye definiciones y funciones necesarias

#ifndef QUICKSORT_EXTERNO_H
#define QUICKSORT_EXTERNO_H

#include <cstdint>
#include <string>
#include <cstdio> 

/**
 * @brief Implementa el algoritmo de ordenamiento Quicksort diseñado para operar en memoria secundaria (disco).
 * Esta clase maneja la lectura, escritura y partición de datos en archivos para ordenar grandes volúmenes
 * de información que no caben en la memoria principal en su totalidad.
 */
class QuicksortExterno {
private:
    /**
     * @brief Tamaño de bloque (en bytes o número de elementos) que se utilizará para las
     * operaciones de lectura y escritura en disco.
     */
    size_t B;
    /**
     * @brief Tamaño máximo de memoria principal (en bytes) que se puede utilizar
     * para el algoritmo.
     */ 
    size_t M;
    /**
     * @brief Aridad del algoritmo, número de subarreglos en los que se particionará el archivo
     * durante cada paso de Quicksort.
     */
    size_t a;
    /**
     * @brief Lleva un registro de la cantidad de operaciones (I/O) realizadas en disco durante
     * el proceso de ordenamiento.
     */
    size_t contador_io; 

    /**
     * @brief Puntero a un bloque de memoria (array de int64_t) que se utiliza como un buffer
     * temporal para leer datos desde el disco o para escribir datos hacia el disco.
     */
    int64_t* buffer;
    
    // Funciones auxiliares

    /**
     * @brief Esta función se encarga de leer el archivo de entrada (o una porción de él),
     * distribuir sus elementos en 'a' archivos de partición temporales basándose en los pivotes proporcionados.
     * Los elementos menores que el primer pivote van al primer archivo de partición, los elementos
     * entre el primer y segundo pivote van al segundo archivo, y así sucesivamente.
     * También calcula y devuelve los tamaños de estas particiones.
     * @param archivo_entrada Nombre del archivo que contiene los datos a particionar.
     * @param archivos_particiones Un arreglo de strings donde se almacenarán los nombres de los archivos
     * temporales creados para cada partición.
     * @param inicio Posición inicial (índice del primer elemento) dentro del archivo_entrada
     * desde donde se comenzará a leer para la partición actual.
     * @param fin Posición final (índice del último elemento) dentro del archivo_entrada
     * hasta donde se leerá para la partición actual.
     * @param pivotes Un arreglo con los a-1 valores pivote utilizados para distribuir los elementos.
     * @param num_pivotes Número de pivotes en el arreglo pivotes (debería ser a-1).
     * @param tamanos_particiones (Salida) Un arreglo donde se guardarán los tamaños (cantidad de elementos)
     * de cada una de las 'a' particiones generadas.
     */
    void particionarArchivo(const std::string& archivo_entrada, 
        std::string* archivos_particiones,
        size_t inicio, size_t fin, 
        int64_t* pivotes, size_t num_pivotes,
        size_t* tamanos_particiones);
    
    /**
     * @brief Implementa la lógica recursiva principal del algoritmo Quicksort Externo.
     * Si el segmento del archivo a ordenar (fin - inicio) es menor o igual al tamaño de la memoria principal M,
     * lo ordena en memoria. De lo contrario, selecciona pivotes, particiona el archivo usando particionarArchivo,
     * y luego se llama recursivamente a sí misma para cada una de las particiones generadas.
     * Finalmente, concatena los subarreglos ordenados.
     * @param archivo_entrada Nombre del archivo (o partición) que se va a ordenar.
     * @param archivo_salida Nombre del archivo donde se escribirá el resultado ordenado de archivo_entrada.
     * @param inicio Posición inicial (índice del primer elemento) del segmento a ordenar en archivo_entrada.
     * @param fin Posición final (índice del último elemento) del segmento a ordenar en archivo_entrada.
     */    
    void quicksortRecursivo(const std::string& archivo_entrada, 
                           const std::string& archivo_salida,
                           size_t inicio, size_t fin);
    
    // Funciones para manipulación de archivos

    /**
     * @brief Lee un bloque de datos de tamaño B desde una posición específica de un archivo abierto
     * y lo carga en el 'bloque' de memoria proporcionado. Incrementa el contador de operaciones I/O.
     * @param archivo Puntero al descriptor del archivo abierto desde donde se leerá.
     * @param bloque (Salida) Puntero al buffer de memoria donde se almacenarán los datos leídos del bloque.
     * @param posicion Posición (offset en número de bloques o elementos) dentro del archivo
     * desde donde comenzará la lectura del bloque.
     */
    void leerBloque(FILE* archivo, int64_t* bloque, size_t posicion);

    /**
     * @brief Escribe un bloque de datos de tamaño B desde el 'bloque' de memoria proporcionado
     * a una posición específica de un archivo abierto. Incrementa el contador de operaciones I/O.
     * @param archivo Puntero al descriptor del archivo abierto donde se escribirá.
     * @param bloque Puntero al buffer de memoria que contiene los datos a escribir en el bloque.
     * @param posicion Posición (offset en número de bloques o elementos) dentro del archivo
     * donde comenzará la escritura del bloque.
     */
    void escribirBloque(FILE* archivo, int64_t* bloque, size_t posicion);

    /**
     * @brief Lee una porción aleatoria (o un bloque) del archivo de entrada, selecciona 'a-1' elementos
     * de esta porción al azar, los ordena, y estos elementos ordenados se devuelven como los pivotes
     * para la operación de partición.
     * @param archivo Nombre del archivo desde el cual se seleccionarán los pivotes.
     * @param inicio Posición inicial del rango dentro del archivo del cual se pueden seleccionar los pivotes.
     * @param fin Posición final del rango dentro del archivo del cual se pueden seleccionar los pivotes.
     * @return Un puntero a un arreglo de 'a-1' elementos que son los pivotes seleccionados y ordenados.
     */
    int64_t* seleccionarPivotes(const std::string& archivo, 
                               size_t inicio, size_t fin);

public:
    /**
     * @brief Constructor de la clase QuicksortExterno.
     * Inicializa una nueva instancia. Configura el tamaño de bloque B, el tamaño máximo de memoria M,
     * y la aridad (número de subarreglos) a. También podría inicializar el buffer de memoria
     * y el contador de I/O.
     * @param tamano_bloque El tamaño del bloque (B) para operaciones de disco.
     * @param memoria_maxima El tamaño máximo de memoria principal (M) disponible.
     * @param aridad El número de subarreglos (a) a utilizar, lo que implica a-1 pivotes.
     */
     QuicksortExterno(size_t tamano_bloque, size_t memoria_maxima, size_t aridad);

     /**
      * @brief Destructor de la clase QuicksortExterno.
      * Libera los recursos utilizados por la instancia, principalmente el buffer de memoria.
      */
     ~QuicksortExterno();
 
     /**
      * @brief Función principal que el usuario llama para ordenar un archivo.
      * Inicia el proceso de Quicksort Externo llamando a quicksortRecursivo con los parámetros
      * iniciales correspondientes al archivo de entrada completo.
      * @param archivo_entrada Nombre del archivo que contiene los datos desordenados.
      * @param archivo_salida Nombre del archivo donde se guardará el resultado ordenado.
      */
     void ordenar(const std::string& archivo_entrada, const std::string& archivo_salida);
 
     /**
      * @brief Devuelve el número total de operaciones de entrada/salida I/O que se han
      * realizado en disco desde la última vez que se reseteó el contador (o desde la creación del objeto).
      * @return El valor actual del contador de operaciones I/O.
      */
     size_t obtenerContadorIO();
 
     /**
      * @brief Reinicia el contador de operaciones I/O a cero.
      */
     void resetContadorIO();
};

#endif // QUICKSORT_EXTERNO_H