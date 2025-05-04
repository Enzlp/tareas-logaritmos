#include <cstdint>
#include <sys/statvfs.h>
#include <iostream>
#include "mergesort/mergesort_externo.hpp"
#include "quicksort/quicksort_externo.h"
#include "file_generator/input_generator.h"
#include <limits>

//Header
int busqueda_ternaria(int left, int right, size_t M, size_t B, std::string& archivo_entrada, size_t tamano_archivo);
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

//Hay que sacar esta funcion antes de entregar
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
            nombres_archivos.push_back(std::make_tuple(entrada, i));
        }
    }

    cout<< "Ordenamiento por mergesort: "<<std::endl;
    std::vector<int> io_merge;
    std::vector<double> time_merge;
    MergesortExterno mergesort(B,M,a); //Calculo de mergesort para cada archivo
    for (const auto& archivo : nombres_archivos) {
        std::string name = std::get<0>(archivo);  
        size_t size = std::get<1>(archivo);       

        time_t start = time(nullptr);   
        mergesort.mergesort(name, "salida.bin", size); 
        time_t end = time(nullptr);

        time_merge.push_back(difftime(end, start));
        io_merge.push_back(mergesort.obtenerContadorIO());
        mergesort.resetContadorIO();
        mergesort.limpiarBuffer();
    }

    double avg_io_merge = std::accumulate(io_merge.begin(), io_merge.end(), 0.0) / io_merge.size();
    double avg_time_merge = std::accumulate(time_merge.begin(), time_merge.end(), 0.0) / time_merge.size();
    cout<<"- Promedio de tiempo : "<<avg_time_merge<< std::endl;
    cout<<"- Promedio de IO :"<<avg_io_merge<<std::endl;
    std::remove("salida.bin");


    cout<< "Ordenamiento por quicksort: "<<std::endl;
    std::vector<int> io_quick;
    std::vector<double> time_quick;
    QuicksortExterno quicksort(M,B,a);
    for (const auto& archivo : nombres_archivos) {
        std::string name = std::get<0>(archivo);      

        time_t start = time(nullptr);   
        quicksort.ordenar(name, "salida.bin"); 
        time_t end = time(nullptr);

        time_quick.push_back(difftime(end, start));
        io_quick.push_back(quicksort.obtenerContadorIO());
        quicksort.resetContadorIO();
        //verificar si hay que implementar un reset al buffer
    }

    double avg_io_quick = std::accumulate(io_quick.begin(), io_quick.end(), 0.0) / io_quick.size();
    double avg_time_quick = std::accumulate(time_quick.begin(), time_quick.end(), 0.0) / time_quick.size();
    cout<<"- Promedio de tiempo : "<<avg_time_quick<< std::endl;
    cout<<"- Promedio de IO :"<<avg_io_quick<<std::endl;
    std::remove("salida.bin");

    //Paso 3: Limpieza de archivos
    for (const auto& archivo : nombres_archivos) {
        std::string name = std::get<0>(archivo);  
        std::remove(name.c_str());
    }

    //Paso 5: generar graficos

    return 0;
}