#include <cstdint>
#include <sys/statvfs.h>
#include <iostream>
#include <limits>
#include <fstream>                
#include <tuple>                  
#include <ctime>                  
#include "mergesort/mergesort_externo.hpp"
#include "quicksort/quicksort_externo.h"
#include "file_generator/input_generator.h"       
#include "misc/block_size.h"      
using namespace std;


//Header
int busqueda_ternaria(int left, int right, size_t M, size_t B, std::string& archivo_entrada, size_t tamano_archivo);
void exportToCsv(const std::string& filename, const std::vector<std::tuple<double, size_t>>& data);

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
    std::cout << "- Memoria principal (M): " << M / (1024 * 1024) << " MB" << " (" << M << " bytes)" << std::endl;
    std::cout << "- Tamaño del bloque de disco: " << B << std::endl;

    // Paso 1: Calculo de a
    std::string archivo_entrada = filename + ".bin";
    generate_binary_file(archivo_entrada, M, 60);

    size_t tamano_archivo = 60 * M;
    size_t b = B / sizeof(int64_t);

    //int a = busqueda_ternaria(2, b, M, B, archivo_entrada, tamano_archivo);
    int a = 200;
    
    cout<< "Aridad obtenida: " << a << std::endl;

    std::remove(archivo_entrada.c_str());

    // Paso 2: Ordenamiento de archivos
    cout << std::endl;
    std::vector<size_t> N = {4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52, 56, 60};

    // Vectores para almacenar resultados
    std::vector<std::tuple<int, size_t>> io_merge;
    std::vector<std::tuple<double, size_t>> time_merge;
    std::vector<std::tuple<int, size_t>> io_quick;
    std::vector<std::tuple<double, size_t>> time_quick;

    // Inicializar estructuras para algoritmos de ordenamiento
    MergesortExterno mergesort(B, M, a);
    QuicksortExterno quicksort(B, M, a); 

    // Procesar un archivo a la vez
    for (size_t i = 0; i < N.size(); i++) {
        for (int j = 1; j <= 5; j++) {
            // Generar nombre y archivo
            std::string archivo_nombre = filename + "_" + std::to_string(i) + "_" + std::to_string(j) + ".bin";
            size_t tamano = N[i];
            
            cout << "Procesando archivo: " << archivo_nombre << " (tamaño: " << tamano << "M)" << endl;
            
            // Generar archivo de prueba
            generate_binary_file(archivo_nombre, M, tamano);
            
            // Archivo temporal para resultados
            std::string archivo_salida = "temp_salida.bin";
            
            // 1. Procesar con Mergesort
            cout << "  - Aplicando Mergesort..." << endl;
            time_t start_merge = time(nullptr);
            mergesort.mergesort(archivo_nombre, archivo_salida, tamano*M);
            time_t end_merge = time(nullptr);
            
            // Guardar resultados de Mergesort
            time_merge.push_back(std::make_tuple(difftime(end_merge, start_merge), tamano));
            io_merge.push_back(std::make_tuple(mergesort.obtenerContadorIO(), tamano));
        
            
            // Limpiar
            mergesort.resetContadorIO();
            mergesort.limpiarBuffer();
            std::remove(archivo_salida.c_str());
            
            // 2. Procesar con Quicksort
            cout << "  - Aplicando Quicksort..." << endl;
            time_t start_quick = time(nullptr);
            quicksort.ordenar(archivo_nombre, archivo_salida);
            time_t end_quick = time(nullptr);
            
            // Guardar resultados de Quicksort
            time_quick.push_back(std::make_tuple(difftime(end_quick, start_quick), tamano));
            io_quick.push_back(std::make_tuple(quicksort.obtenerContadorIO(), tamano));
            
            // Limpiar
            quicksort.resetContadorIO();
            std::remove(archivo_salida.c_str());
            
            // Eliminar archivo de entrada para liberar espacio
            std::remove(archivo_nombre.c_str());
            
            cout << "  - Completado" << endl;
        }
    }

    // Calcular promedios para Mergesort
    cout << "Calculando promedios para Mergesort..." << endl;
    std::vector<std::tuple<double, size_t>> time_merge_avg;
    std::vector<std::tuple<double, size_t>> io_merge_avg;

    for (size_t i = 0; i < N.size(); i++) {
        double suma_tiempo = 0.0;
        double suma_io = 0.0;
        size_t tamano = N[i];
        
        for (int j = 0; j < 5; j++) {
            size_t idx = i * 5 + j;
            suma_tiempo += std::get<0>(time_merge[idx]);
            suma_io += std::get<0>(io_merge[idx]);
        }
        
        time_merge_avg.emplace_back(suma_tiempo / 5.0, tamano);
        io_merge_avg.emplace_back(suma_io / 5.0, tamano);
    }

    // Calcular promedios para Quicksort
    cout << "Calculando promedios para Quicksort..." << endl;
    std::vector<std::tuple<double, size_t>> time_quick_avg;
    std::vector<std::tuple<double, size_t>> io_quick_avg;

    for (size_t i = 0; i < N.size(); i++) {
        double suma_tiempo = 0.0;
        double suma_io = 0.0;
        size_t tamano = N[i];
        
        for (int j = 0; j < 5; j++) {
            size_t idx = i * 5 + j;
            suma_tiempo += std::get<0>(time_quick[idx]);
            suma_io += std::get<0>(io_quick[idx]);
        }
        
        time_quick_avg.emplace_back(suma_tiempo / 5.0, tamano);
        io_quick_avg.emplace_back(suma_io / 5.0, tamano);
    }

    cout << "- Valores de tiempo promedio calculados" << endl;
    cout << "- Valores de IO promedio calculados" << endl;

    //Paso 4: generar grafico (Revisar si N devuelve en int o byte)
    exportToCsv("io_ms.csv", io_merge_avg);
    exportToCsv("time_ms.csv", time_merge_avg);
    exportToCsv("io_qs.csv", io_quick_avg);
    exportToCsv("time_qs.csv", time_quick_avg);

    return 0;
}