#include <cstdint>
#include <sys/statvfs.h>
#include <iostream>
#include <limits>
#include <fstream>                
#include <tuple>                  
#include <ctime>                  
#include "mergesort/mergesort_alterno.hpp"
#include "quicksort/quicksort_externo.h"
#include "file_generator/input_generator.h"       
#include "misc/block_size.h"      


//Header
int busqueda_ternaria(int left, int right, size_t M, size_t B, std::string& archivo_entrada, size_t tamano_archivo);
void exportToCsv(const std::string& filename, const std::vector<std::tuple<double, size_t>>& data);
bool verificarOrdenamiento(const std::string& nombre_archivo);

/** 
 * busqueda ternaria, divide el conjunto en tres partes y va achicando el intervalo hasta llegar a un intervalo de 4 valores
 * donde prueba cada uno de los valores por separado y retorna el minimo de operaciones IO. En este caso es un arreglo imaginario
 * entre [2, b] al que le vamos a aplicar el algoritmo de mergesort
 * @param left valor inicial del "arreglo"
 * @param right valor final del "arreglo"
 * @param M memoria principal o RAM definida
 * @param B tamaño de bloque
 * @param archivo_entrada nombre del archivo a ordenar
 * @param tamano_archivo tamaño del archivo a ordenar
 * @returns minimo de operaciones IO
 */
int busqueda_ternaria(int left, int right, size_t M, size_t B, std::string& archivo_entrada, size_t tamano_archivo){
    int min = std::numeric_limits<int>::max();
    int a = 0;
    MergesortExterno mergesort_search(B, M , 0);
    std::string archivo_salida = "busqueda.bin";
  
    while(right-left>4){
        int mid1 = left + (right - left) / 3;
        int mid2 = right - (right - left) / 3;

        mergesort_search.updateAridad(static_cast<size_t>(mid1));
        mergesort_search.mergesort(archivo_entrada, archivo_salida , tamano_archivo);
        int sort_mid1 = mergesort_search.obtenerContadorIO();
        mergesort_search.resetContadorIO();
        mergesort_search.limpiarBuffer();

        mergesort_search.updateAridad(static_cast<size_t>(mid2));
        mergesort_search.mergesort(archivo_entrada, archivo_salida , tamano_archivo);
        int sort_mid2 = mergesort_search.obtenerContadorIO();
        mergesort_search.resetContadorIO();
        mergesort_search.limpiarBuffer();

        if (sort_mid1 < sort_mid2) {
            right = mid2;
        } else {
            left = mid1;
        }
    }

    for(int i = left; i <= right; i++){
        mergesort_search.updateAridad(static_cast<size_t>(i));
        mergesort_search.mergesort(archivo_entrada, archivo_salida , tamano_archivo);
        int currentIO = mergesort_search.obtenerContadorIO();
        if (currentIO < min) {
            min = currentIO;
            a = i;
        }
        mergesort_search.resetContadorIO();
    }

    std::remove(archivo_salida.c_str());
    return a;
}

/**
 * exporta los datos obtenidos a formato csv
 * @param filename nombre del archivo a exportar
 * @param data datos a exportar
 */
void exportToCsv(const std::string& filename, const std::vector<std::tuple<double, size_t>>& data){
    std::string archivo = "graphs/"+filename;
    std::ofstream file(archivo.c_str());
    for (const auto& [value, group] : data) {
        file << group << "," << value << "\n";
    }
    file.close();
}

bool verificarOrdenamiento(const std::string& nombre_archivo) {
    FILE* archivo = fopen(nombre_archivo.c_str(), "rb");
    if (!archivo) {
        std::cerr << "Error al abrir el archivo: " << nombre_archivo << std::endl;
        return false;
    }
    
    // Tamaño de buffer (1 MB)
    const size_t buffer_size = 1024 * 1024 / sizeof(int64_t);
    int64_t* buffer = new int64_t[buffer_size];
    
    // Valor anterior para comparar
    int64_t valor_anterior = INT64_MIN;
    bool ordenado = true;
    
    // Leer archivo por bloques
    size_t elementos_leidos;
    do {
        elementos_leidos = fread(buffer, sizeof(int64_t), buffer_size, archivo);
        
        for (size_t i = 0; i < elementos_leidos; i++) {
            if (buffer[i] < valor_anterior) {
                ordenado = false;
                break;
            }
            valor_anterior = buffer[i];
        }
        
        if (!ordenado) break;
    } while (elementos_leidos == buffer_size);
    
    delete[] buffer;
    fclose(archivo);
    
    return ordenado;
}

int main(int argc, char* argv[]){
    
    // Configuración por defecto
    size_t M = 50 * 1024 * 1024;  // 50 MB de memoria principal
    std::string filename = "pruebas";

    // Intentamos obtener el tamaño del bloque
    size_t B = get_block_size();
    
    if (B < 0) {
        std::cout << "Error al obtener el tamaño de bloque." << std::endl;
        std::cout << "Se usará un valor por defecto de 4096" << std::endl;
        B = 4096; 
    }

    if (argc < 2){
        std::cout << "Utilizando un valor de memoria por defecto de 50MB" << std::endl;
        std::cout << "Para cambiarlo ejecutar " << argv[0] << " <memoria_en_MB>" << std::endl;
    }

    // Procesar argumentos
    if (argc == 2) {
        M = std::stoul(argv[1]) * 1024 * 1024;  // Convertir MB a bytes
    }

    if (argc > 2){
        std::cerr << "Uso:\n"
            << "  " << argv[0] << " <memoria_en_MB> \n";
        return 1;
    }
    
    std::cout << "Configuración:" << std::endl;
    std::cout << "- Nombre de archivo de entrada: " << filename << ".bin" << std::endl;
    std::cout << "- Memoria principal (M): " << M / (1024 * 1024) << " MB" << " (" << M << " bytes)" << std::endl;

    // Paso 1: Calculo de a
    std::string archivo_entrada = filename + ".bin";
    generate_binary_file(archivo_entrada, M, 60);

    size_t tamano_archivo = 60 * M;
    size_t b = B / sizeof(int64_t);

    //int a = busqueda_ternaria(2, b, M, B, archivo_entrada, tamano_archivo);
    int a = 96;
    MergesortExterno mergesort(B, M, a);
    
    mergesort.mergesort(archivo_entrada, "salida.bin", tamano_archivo);
    std::cout<<"Bool obtenida: "<< verificarOrdenamiento("salida.bin") <<std::endl;
}

 