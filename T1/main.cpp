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
    size_t B = 4096;                    // Tamaño del bloque (512 B por defecto)
    size_t M = 50 * 1024 * 1024;       // 50 MB de memoria principal
    
    if (argc < 2 || argc > 3){
        std::cerr << "Usage:\n"
            << "  " << argv[0] << " <filename> <memory_size_in_MB> \n";
        return 1;
    }

    // Procesar argumentos
    std::string filename = argv[1];

    if (argc > 2) {
        M = std::stoul(argv[2]) * 1024 * 1024;  // Convertir MB a bytes
    }
    
    std::cout << "Configuración:" << std::endl;
    std::cout << "- Nombre de archivo de entrada: " << filename << ".bin" << std::endl;
    std::cout << "- Memoria principal (M): " << M / (1024 * 1024) << " MB" << " (" << M << " bytes)" << std::endl;

    // Paso 1: Calculo de a
    std::string archivo_entrada = filename + ".bin";
    generate_binary_file(archivo_entrada, M, 60);

    size_t tamano_archivo = 60 * M;
    size_t b = B / sizeof(int64_t);

    int a = busqueda_ternaria(2, b, M, B, archivo_entrada, tamano_archivo);
    
    cout<<"Aridad obtenida: "<< a <<std::endl;

    // Paso 2: Ordenamiento de archivos
    cout<<""<<std::endl;
    std::vector<size_t> N = {4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52, 56, 60};
    std::vector<std::tuple<std::string, size_t>> nombres_archivos;

    for (size_t i = 0; i < N.size(); i++) {
        for (int j = 1; j <= 5; j++) {
            std::string entrada = filename + "_" + std::to_string(i) + "_" + std::to_string(j) + ".bin";
            generate_binary_file(entrada, M, N[i]*M);
            nombres_archivos.push_back(std::make_tuple(entrada, N[i]));
        }
    }

    cout<< "Ordenamiento por mergesort: "<<std::endl;
    std::vector<std::tuple<int, size_t>> io_merge;
    std::vector<std::tuple<double, size_t>> time_merge;
    MergesortExterno mergesort(B,M,a); //Calculo de mergesort para cada archivo
    for (const auto& archivo : nombres_archivos) {
        std::string name = std::get<0>(archivo);  
        size_t size = std::get<1>(archivo);       

        time_t start = time(nullptr);   
        mergesort.mergesort(name, "salida.bin", size); 
        time_t end = time(nullptr);

        time_merge.push_back(std::make_tuple(difftime(end, start), size));
        io_merge.push_back(std::make_tuple(mergesort.obtenerContadorIO(), size));
        mergesort.resetContadorIO();
        mergesort.limpiarBuffer();
    }
    std::remove("salida.bin");

    std::vector<std::tuple<double, size_t>> time_merge_avg;
    for (size_t i = 0; i < time_merge.size(); i += 5) {
        double suma = 0.0;
        size_t grupo = std::get<1>(time_merge[i]); // misma N para los 5
        for (size_t j = 0; j < 5; ++j) {
            suma += std::get<0>(time_merge[i + j]);
        }
        double promedio = suma / 5.0;
        time_merge_avg.emplace_back(promedio, grupo);
    }

    std::vector<std::tuple<double, size_t>> io_merge_avg;
    for (size_t i = 0; i < io_merge.size(); i += 5) {
        double suma = 0.0;
        size_t grupo = std::get<1>(io_merge[i]); // misma N para los 5
        for (size_t j = 0; j < 5; ++j) {
            suma += std::get<0>(io_merge[i + j]);
        }
        double promedio = suma / 5.0;
        io_merge_avg.emplace_back(promedio, grupo);
    }
    cout<<"- Valores de tiempo promedio calculados "<<std::endl;
    cout<<"- Valores de IO promedio calculados"<<std::endl;


    cout<< "Ordenamiento por quicksort: "<<std::endl;
    std::vector<std::tuple<int, size_t>> io_quick;
    std::vector<std::tuple<double, size_t>> time_quick;
    
    QuicksortExterno quicksort(M, B, a);
    
    for (const auto& archivo : nombres_archivos) {
        std::string name = std::get<0>(archivo);      
        size_t size = std::get<1>(archivo);  // 🔧 Esto faltaba
    
        time_t start = time(nullptr);   
        quicksort.ordenar(name, "salida.bin"); 
        time_t end = time(nullptr);
    
        time_quick.push_back(std::make_tuple(difftime(end, start), size));
        io_quick.push_back(std::make_tuple(quicksort.obtenerContadorIO(), size));
        quicksort.resetContadorIO();
        // quicksort.limpiarBuffer(); // ← solo si implementaste esa función
    }
    
    std::remove("salida.bin");

    std::vector<std::tuple<double, size_t>> time_quick_avg;
    for (size_t i = 0; i < time_quick.size(); i += 5) {
        double suma = 0.0;
        size_t grupo = std::get<1>(time_quick[i]); // misma N para los 5
        for (size_t j = 0; j < 5; ++j) {
            suma += std::get<0>(time_quick[i + j]);
        }
        double promedio = suma / 5.0;
        time_quick_avg.emplace_back(promedio, grupo);
    }
    
    std::vector<std::tuple<double, size_t>> io_quick_avg;
    for (size_t i = 0; i < io_quick.size(); i += 5) {
        double suma = 0.0;
        size_t grupo = std::get<1>(io_quick[i]); // misma N para los 5
        for (size_t j = 0; j < 5; ++j) {
            suma += std::get<0>(io_quick[i + j]);
        }
        double promedio = suma / 5.0;
        io_quick_avg.emplace_back(promedio, grupo);
    }
    cout<<"- Valores de tiempo promedio calculados "<<std::endl;
    cout<<"- Valores de IO promedio calculados"<<std::endl;


    //Paso 3: Limpieza de archivos
    for (const auto& archivo : nombres_archivos) {
        std::string name = std::get<0>(archivo);  
        std::remove(name.c_str());
    }

    //Paso 4: generar grafico (Revisar si N devuelve en int o byte)
    exportToCsv("io_ms.csv", io_merge_avg);
    exportToCsv("time_ms.csv", time_merge_avg);
    exportToCsv("io_qs.csv", io_quick_avg);
    exportToCsv("time_qs.csv", time_quick_avg);

    return 0;
}