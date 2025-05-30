#include "quicksort_externo.h"
#include <vector>
#include <string>
#include <algorithm> // Para std::sort, std::min, std::shuffle
#include <cstdio>    // Para FILE*, fopen, fclose, fread, fwrite, fseek, ftell, remove
#include <cstdlib>   // Para rand, srand
#include <ctime>     // Para time
#include <stdexcept> // Para std::runtime_error (opcional)
#include <random>    // Para std::random_device, std::mt19937

/**
 * Constructor de la clase QuicksortExterno.
 * @param block_size_bytes Tamaño del bloque de disco en bytes (B).
 * @param memory_size_bytes Tamaño de la memoria principal en bytes (M).
 * @param arity_a_val Aridad 'a', número de subarreglos en los que particionar.
 */
QuicksortExterno::QuicksortExterno(size_t block_size_bytes, size_t memory_size_bytes, size_t arity_a_val)
    : B_bytes(block_size_bytes), M_bytes(memory_size_bytes), arity_a(arity_a_val),
      contador_io(0), temp_file_id_counter(0) {
    this->num_pivots_to_select = (this->arity_a > 0) ? (this->arity_a - 1) : 0;

    // Sembrar el generador de números aleatorios una vez
    srand(static_cast<unsigned int>(time(nullptr)));
}
/**
 * Ordena un archivo binario de enteros de 64 bits usando Quicksort Externo.
 * @param archivo_entrada Ruta al archivo binario de entrada.
 * @param archivo_salida Ruta donde se guardará el archivo binario ordenado.
 */
void QuicksortExterno::ordenar(const std::string& archivo_entrada, const std::string& archivo_salida) {
    resetContadorIO();
    temp_file_id_counter = 0; // Reiniciar para nombres de temp únicos por cada llamada a ordenar

    size_t N_total_elements = get_num_elements_in_file(archivo_entrada);

    if (N_total_elements == 0) {
        // Si el archivo de entrada está vacío, crear un archivo de salida vacío.
        FILE* out_empty = fopen(archivo_salida.c_str(), "wb");
        if (out_empty) {
            fclose(out_empty);
        } else {
            // Manejar error si no se puede crear el archivo de salida
        }
        return;
    }
    quicksort_recursivo(archivo_entrada, N_total_elements, archivo_salida);
}

/**
 * Metodo para obtener contador de I/O
 * @return El número total de operaciones de E/S (lectura/escritura de bloques) realizadas.
 */
size_t QuicksortExterno::obtenerContadorIO() const {
    return contador_io;
}


/**
 * Reinicia el contador de operaciones de E/S a cero.
 */
void QuicksortExterno::resetContadorIO() {
    contador_io = 0;
}

/**
 * Obtiene el número de elementos int64_t en un archivo binario.
 * @param file_name Nombre del archivo.
 * @return Número de elementos de 64 bits en el archivo.
 */
size_t QuicksortExterno::get_num_elements_in_file(const std::string& file_name) {
    FILE* file = fopen(file_name.c_str(), "rb");
    if (!file) {
        // std::cerr << "Error abriendo archivo para obtener tamaño: " << file_name << std::endl;
        return 0; // O lanzar excepción
    }
    fseek(file, 0, SEEK_END);
    long file_size_bytes = ftell(file);
    fclose(file);

    if (file_size_bytes <= 0) { // Igual a 0 para archivo vacío, negativo para error en ftell
        return 0;
    }
    return static_cast<size_t>(file_size_bytes) / sizeof(int64_t);
}

/**
 * Genera un nombre único para un archivo temporal.
 * @return Nombre del archivo temporal.
 */
std::string QuicksortExterno::generar_nombre_temporal() {
    return "temp_qsort_" + std::to_string(temp_file_id_counter++) + ".bin";
}


/**
 * Ordena en memoria una partición que cabe completamente en la memoria principal.
 * Lee de 'input_filename', ordena y escribe en 'output_filename'.
 * @param input_filename nombre archivo a ordenar
 * @param num_elements numero de elementos en el archivo
 * @param output_filename nombre del archivo de salida
 */
void QuicksortExterno::sort_in_memory_and_write(const std::string& input_filename, size_t num_elements, const std::string& output_filename) {
    if (num_elements == 0) {
        FILE* out_empty = fopen(output_filename.c_str(), "wb");
        if (out_empty) fclose(out_empty);
        return;
    }

    std::vector<int64_t> data_to_sort;
    data_to_sort.reserve(num_elements);

    FILE* in_file = fopen(input_filename.c_str(), "rb");
    if (!in_file) { /* Manejar error */ return; }

    size_t elements_per_B_block = B_bytes / sizeof(int64_t);
    if (elements_per_B_block == 0) elements_per_B_block = 1; // Evitar división por cero si B es muy pequeño

    std::vector<int64_t> read_buffer_vec(elements_per_B_block);
    size_t elements_read_total = 0;

    while (elements_read_total < num_elements) {
        size_t elements_to_read_this_round = std::min(num_elements - elements_read_total, elements_per_B_block);
        size_t actual_read = fread(read_buffer_vec.data(), sizeof(int64_t), elements_to_read_this_round, in_file);
        contador_io++;
        
        if (actual_read > 0) {
            data_to_sort.insert(data_to_sort.end(), read_buffer_vec.begin(), read_buffer_vec.begin() + actual_read);
            elements_read_total += actual_read;
        } else { // EOF o error
            break;
        }
    }
    fclose(in_file);

    std::sort(data_to_sort.begin(), data_to_sort.end());

    FILE* out_file = fopen(output_filename.c_str(), "wb");
    if (!out_file) { /* Manejar error */ return; }
    
    size_t elements_written_total = 0;
    while (elements_written_total < num_elements) {
        size_t elements_to_write_this_round = std::min(num_elements - elements_written_total, elements_per_B_block);
        fwrite(data_to_sort.data() + elements_written_total, sizeof(int64_t), elements_to_write_this_round, out_file);
        contador_io++;
        elements_written_total += elements_to_write_this_round;
    }
    fclose(out_file);
}

/**
 * Selecciona 'num_pivots_to_select' pivotes de un bloque aleatorio del archivo 'input_filename'.
 * @param input_filename Archivo del cual seleccionar pivotes.
 * @param num_elements_in_file Número total de elementos en 'input_filename'.
 * @return Vector con los pivotes seleccionados y ordenados.
 */
std::vector<int64_t> QuicksortExterno::seleccionar_pivotes(const std::string& input_filename, size_t num_elements_in_file) {
    if (num_pivots_to_select == 0 || num_elements_in_file == 0) {
        return {};
    }

    FILE* file = fopen(input_filename.c_str(), "rb");
    if (!file) return {}; // Manejar error

    size_t elements_per_B_block = B_bytes / sizeof(int64_t);
    if (elements_per_B_block == 0) elements_per_B_block = 1;

    size_t num_total_blocks_in_file = (num_elements_in_file * sizeof(int64_t) + B_bytes - 1) / B_bytes;
    if (num_total_blocks_in_file == 0 && num_elements_in_file > 0) num_total_blocks_in_file = 1; // Si es más pequeño que un bloque

    size_t random_block_idx = 0;
    if (num_total_blocks_in_file > 1) { // rand() % 1 can be problematic
        random_block_idx = static_cast<size_t>(rand()) % num_total_blocks_in_file;
    }
    
    fseek(file, random_block_idx * B_bytes, SEEK_SET);

    std::vector<int64_t> block_elements_buffer(elements_per_B_block);
    size_t elements_read_from_block = fread(block_elements_buffer.data(), sizeof(int64_t), elements_per_B_block, file);
    contador_io++;
    fclose(file);

    if (elements_read_from_block == 0) {
        return {}; // No se pudo leer nada
    }
    
    // Redimensionar al tamaño real leído
    block_elements_buffer.resize(elements_read_from_block);

    std::vector<int64_t> pivots;
    // Usar C++11 <random> para mejor aleatoriedad y selección
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(block_elements_buffer.begin(), block_elements_buffer.end(), g);

    size_t count_to_pick = std::min(num_pivots_to_select, elements_read_from_block);
    for (size_t i = 0; i < count_to_pick; ++i) {
        pivots.push_back(block_elements_buffer[i]);
    }

    std::sort(pivots.begin(), pivots.end());
    return pivots;
}

/**
 * Particiona 'input_filename' en 'arity_a' archivos temporales basado en los 'pivots'.
 * @param input_filename Archivo a particionar.
 * @param num_elements_total Número de elementos en 'input_filename'.
 * @param pivots Vector de pivotes ordenados.
 * @return Vector de pares, donde cada par contiene el nombre del archivo de partición temporal y el número de elementos que contiene.
 */
std::vector<std::pair<std::string, size_t>> QuicksortExterno::particionar_archivo(
    const std::string& input_filename,
    size_t num_elements_total,
    const std::vector<int64_t>& pivots) {

    std::vector<std::pair<std::string, size_t>> partition_files_info;
    std::vector<FILE*> out_files_ptr(arity_a, nullptr);
    std::vector<std::string> temp_filenames(arity_a);
    std::vector<size_t> elements_in_partition_count(arity_a, 0);
    
    // Búferes en memoria para cada partición antes de escribir al disco
    std::vector<std::vector<int64_t>> partition_write_buffers(arity_a);
    size_t elements_per_B_block_for_write = B_bytes / sizeof(int64_t);
    if (elements_per_B_block_for_write == 0) elements_per_B_block_for_write = 1;


    for (size_t i = 0; i < arity_a; ++i) {
        temp_filenames[i] = generar_nombre_temporal();
        out_files_ptr[i] = fopen(temp_filenames[i].c_str(), "wb");
        if (!out_files_ptr[i]) { /* Manejar error: cerrar abiertos y limpiar */ }
        partition_write_buffers[i].reserve(elements_per_B_block_for_write);
    }

    FILE* in_file = fopen(input_filename.c_str(), "rb");
    if (!in_file) { /* Manejar error */ }

    size_t elements_per_B_block_for_read = B_bytes / sizeof(int64_t);
    if (elements_per_B_block_for_read == 0) elements_per_B_block_for_read = 1;
    std::vector<int64_t> read_buffer_vec(elements_per_B_block_for_read);
    size_t elements_processed = 0;

    while (elements_processed < num_elements_total) {
        size_t elements_to_read_this_block = std::min(elements_per_B_block_for_read, num_elements_total - elements_processed);
        if (elements_to_read_this_block == 0) break;

        size_t actual_read = fread(read_buffer_vec.data(), sizeof(int64_t), elements_to_read_this_block, in_file);
        contador_io++;

        if (actual_read == 0) { // EOF o error
            break;
        }

        for (size_t i = 0; i < actual_read; ++i) {
            int64_t current_element = read_buffer_vec[i];
            size_t partition_idx = 0;
            
            // Determinar a qué partición pertenece el elemento
            // pivots está ordenado: p_0, p_1, ..., p_{k-1} donde k = num_pivots_to_select
            // Partición 0: elem < p_0
            // Partición j: p_{j-1} <= elem < p_j
            // Partición arity_a-1 (última): elem >= p_{k-1} (último pivote)
            while (partition_idx < pivots.size() && current_element >= pivots[partition_idx]) {
                partition_idx++;
            }

            partition_write_buffers[partition_idx].push_back(current_element);
            elements_in_partition_count[partition_idx]++;

            if (partition_write_buffers[partition_idx].size() == elements_per_B_block_for_write) {
                fwrite(partition_write_buffers[partition_idx].data(), sizeof(int64_t), elements_per_B_block_for_write, out_files_ptr[partition_idx]);
                contador_io++;
                partition_write_buffers[partition_idx].clear();
            }
        }
        elements_processed += actual_read;
    }
    fclose(in_file);

    // Escribir los datos restantes en los búferes de partición
    for (size_t i = 0; i < arity_a; ++i) {
        if (!partition_write_buffers[i].empty()) {
            fwrite(partition_write_buffers[i].data(), sizeof(int64_t), partition_write_buffers[i].size(), out_files_ptr[i]);
            contador_io++; // Se cuenta como una E/S aunque sea parcial
        }
        fclose(out_files_ptr[i]);
        partition_files_info.emplace_back(temp_filenames[i], elements_in_partition_count[i]);
    }
    return partition_files_info;
}

/**
 * Concatena una lista de archivos de entrada (ordenados) en un único archivo de salida.
 * @param nombres_archivos_entrada Vector de nombres de archivos a concatenar.
 * @param archivo_salida_final Nombre del archivo resultante de la concatenación.
 */
void QuicksortExterno::concatenar_archivos(const std::vector<std::string>& nombres_archivos_entrada, const std::string& archivo_salida_final) {
    FILE* out_final_file = fopen(archivo_salida_final.c_str(), "wb");
    if (!out_final_file) { /* Manejar error */ return; }

    size_t elements_per_B_block = B_bytes / sizeof(int64_t);
    if (elements_per_B_block == 0) elements_per_B_block = 1;
    std::vector<int64_t> buffer_vec(elements_per_B_block);

    for (const std::string& nombre_entrada : nombres_archivos_entrada) {
        FILE* in_sub_file = fopen(nombre_entrada.c_str(), "rb");
        if (!in_sub_file) { /* Manejar error, quizás continuar con los demás? */ continue; }

        while (true) {
            size_t read_count = fread(buffer_vec.data(), sizeof(int64_t), elements_per_B_block, in_sub_file);
            if (read_count > 0) {
                contador_io++; // Contar lectura
                fwrite(buffer_vec.data(), sizeof(int64_t), read_count, out_final_file);
                contador_io++; // Contar escritura
            }
            if (read_count < elements_per_B_block) { // EOF o error
                break;
            }
        }
        fclose(in_sub_file);
    }
    fclose(out_final_file);
}

/**
 * Función principal recursiva de Quicksort Externo.
 * Ordena el archivo 'input_filename' que contiene 'num_elements' y escribe el resultado en 'output_filename'.
 * @param current_input_file nombre del archivo actual que se esta ordenando
 * @param num_elements_in_partition numero de elementos en esta particion
 * @param final_output_file_for_this_recursion nombre del archivo de salida final
 */
void QuicksortExterno::quicksort_recursivo(const std::string& current_input_file, size_t num_elements_in_partition, const std::string& final_output_file_for_this_recursion) {
    if (num_elements_in_partition == 0) {
        FILE* out_empty = fopen(final_output_file_for_this_recursion.c_str(), "wb");
        if (out_empty) fclose(out_empty);
        // No es necesario eliminar current_input_file aquí si es el original, solo si es temp.
        return;
    }
    
    // Caso base: si la partición cabe en memoria principal (M_bytes)
    size_t M_elements_capacity = M_bytes / sizeof(int64_t);
    if (num_elements_in_partition <= M_elements_capacity) {
        sort_in_memory_and_write(current_input_file, num_elements_in_partition, final_output_file_for_this_recursion);
        return;
    }

    // Paso recursivo
    // 1. Seleccionar pivotes
    std::vector<int64_t> pivots = seleccionar_pivotes(current_input_file, num_elements_in_partition);

    // 2. Particionar archivo
    std::vector<std::pair<std::string, size_t>> partitions_info =
        particionar_archivo(current_input_file, num_elements_in_partition, pivots);

    std::vector<std::string> sorted_partition_files_temp_names;

    // 3. Llamadas recursivas para cada partición
    for (const auto& partition_pair : partitions_info) {
        const std::string& temp_partition_file_raw = partition_pair.first;
        size_t num_elements_in_temp_partition = partition_pair.second;

        std::string temp_sorted_output_for_sub_problem = generar_nombre_temporal();
        
        if (num_elements_in_temp_partition > 0) {
            quicksort_recursivo(temp_partition_file_raw, num_elements_in_temp_partition, temp_sorted_output_for_sub_problem);
        } else { // Partición vacía
            FILE* ef = fopen(temp_sorted_output_for_sub_problem.c_str(), "wb");
            if (ef) fclose(ef);
        }
        sorted_partition_files_temp_names.push_back(temp_sorted_output_for_sub_problem);
        remove(temp_partition_file_raw.c_str()); // Eliminar partición cruda (no ordenada)
    }

    // 4. Concatenar particiones ordenadas
    concatenar_archivos(sorted_partition_files_temp_names, final_output_file_for_this_recursion);

    // 5. Limpiar archivos temporales de particiones ordenadas
    for (const std::string& sorted_temp_file : sorted_partition_files_temp_names) {
        remove(sorted_temp_file.c_str());
    }
}