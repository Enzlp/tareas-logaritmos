#ifndef QUICKSORT_EXTERNO_H
#define QUICKSORT_EXTERNO_H

#include <string>
#include <vector>
#include <cstdint> // Para int64_t

class QuicksortExterno {
public:
    /**
     * Constructor de la clase QuicksortExterno.
     * @param block_size_bytes Tamaño del bloque de disco en bytes (B).
     * @param memory_size_bytes Tamaño de la memoria principal en bytes (M).
     * @param arity_a_val Aridad 'a', número de subarreglos en los que particionar.
     */
    QuicksortExterno(size_t block_size_bytes, size_t memory_size_bytes, size_t arity_a_val);

    /**
     * Ordena un archivo binario de enteros de 64 bits usando Quicksort Externo.
     * @param archivo_entrada Ruta al archivo binario de entrada.
     * @param archivo_salida Ruta donde se guardará el archivo binario ordenado.
     */
    void ordenar(const std::string& archivo_entrada, const std::string& archivo_salida);

    /**
     * @return El número total de operaciones de E/S (lectura/escritura de bloques) realizadas.
     */
    size_t obtenerContadorIO() const;

    /**
     * Reinicia el contador de operaciones de E/S a cero.
     */
    void resetContadorIO();

private:
    size_t B_bytes;              // Tamaño del bloque de disco en bytes
    size_t M_bytes;              // Tamaño de la memoria principal en bytes
    size_t arity_a;              // Número de sub-arreglos para particionar (parámetro 'a')
    size_t num_pivots_to_select; // arity_a - 1

    size_t contador_io;          // Contador de operaciones de E/S
    int temp_file_id_counter;    // Contador para generar nombres de archivos temporales únicos

    /**
     * Función principal recursiva de Quicksort Externo.
     * Ordena el archivo 'input_filename' que contiene 'num_elements' y escribe el resultado en 'output_filename'.
     */
    void quicksort_recursivo(const std::string& input_filename, size_t num_elements, const std::string& output_filename);

    /**
     * Ordena en memoria una partición que cabe completamente en la memoria principal.
     * Lee de 'input_filename', ordena y escribe en 'output_filename'.
     */
    void sort_in_memory_and_write(const std::string& input_filename, size_t num_elements, const std::string& output_filename);

    /**
     * Selecciona 'num_pivots_to_select' pivotes de un bloque aleatorio del archivo 'input_filename'.
     * @param input_filename Archivo del cual seleccionar pivotes.
     * @param num_elements_in_file Número total de elementos en 'input_filename'.
     * @return Vector con los pivotes seleccionados y ordenados.
     */
    std::vector<int64_t> seleccionar_pivotes(const std::string& input_filename, size_t num_elements_in_file);

    /**
     * Particiona 'input_filename' en 'arity_a' archivos temporales basado en los 'pivots'.
     * @param input_filename Archivo a particionar.
     * @param num_elements_total Número de elementos en 'input_filename'.
     * @param pivots Vector de pivotes ordenados.
     * @return Vector de pares, donde cada par contiene el nombre del archivo de partición temporal y el número de elementos que contiene.
     */
    std::vector<std::pair<std::string, size_t>> particionar_archivo(
        const std::string& input_filename,
        size_t num_elements_total,
        const std::vector<int64_t>& pivots
    );

    /**
     * Concatena una lista de archivos de entrada (ordenados) en un único archivo de salida.
     * @param nombres_archivos_entrada Vector de nombres de archivos a concatenar.
     * @param archivo_salida_final Nombre del archivo resultante de la concatenación.
     */
    void concatenar_archivos(const std::vector<std::string>& nombres_archivos_entrada, const std::string& archivo_salida_final);

    /**
     * Obtiene el número de elementos int64_t en un archivo binario.
     * @param file_name Nombre del archivo.
     * @return Número de elementos de 64 bits en el archivo.
     */
    size_t get_num_elements_in_file(const std::string& file_name);

    /**
     * Genera un nombre único para un archivo temporal.
     * @return Nombre del archivo temporal.
     */
    std::string generar_nombre_temporal();
};

#endif // QUICKSORT_EXTERNO_H