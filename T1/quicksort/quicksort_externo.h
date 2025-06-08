#ifndef QUICKSORT_EXTERNO_H
#define QUICKSORT_EXTERNO_H

#include <string>
#include <vector>
#include <cstdint> // Para int64_t

class QuicksortExterno {
public:
    //Headers metodos publicos
    QuicksortExterno(size_t block_size_bytes, size_t memory_size_bytes, size_t arity_a_val);

    void ordenar(const std::string& archivo_entrada, const std::string& archivo_salida);

    size_t obtenerContadorIO() const;

    void resetContadorIO();

private:
    //Metodos y variables privadas
    size_t B_bytes;              // Tamaño del bloque de disco en bytes
    size_t M_bytes;              // Tamaño de la memoria principal en bytes
    size_t arity_a;              // Número de sub-arreglos para particionar (parámetro 'a')
    size_t num_pivots_to_select; // arity_a - 1

    size_t contador_io;          // Contador de operaciones de E/S
    int temp_file_id_counter;    // Contador para generar nombres de archivos temporales únicos


    void quicksort_recursivo(const std::string& input_filename, size_t num_elements, const std::string& output_filename);

    void sort_in_memory_and_write(const std::string& input_filename, size_t num_elements, const std::string& output_filename);

    std::vector<int64_t> seleccionar_pivotes(const std::string& input_filename, size_t num_elements_in_file);

    std::vector<std::pair<std::string, size_t>> particionar_archivo(
        const std::string& input_filename,
        size_t num_elements_total,
        const std::vector<int64_t>& pivots
    );

    void concatenar_archivos(const std::vector<std::string>& nombres_archivos_entrada, const std::string& archivo_salida_final);

    size_t get_num_elements_in_file(const std::string& file_name);

    std::string generar_nombre_temporal();
};

#endif // QUICKSORT_EXTERNO_H