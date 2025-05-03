#include "quicksort_externo.h"
#include <chrono>
#include <iostream>
#include <random>
#include <string>

// Función para generar un archivo con números aleatorios
void generarArchivo(const std::string& nombre_archivo, size_t num_elementos) {
    FILE* archivo = fopen(nombre_archivo.c_str(), "wb");
    if (!archivo) {
        std::cerr << "Error al crear el archivo: " << nombre_archivo << std::endl;
        return;
    }
    
    // Generador de números aleatorios
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<int64_t> dis;
    
    // Tamaño de buffer (1 MB)
    const size_t buffer_size = 1024 * 1024 / sizeof(int64_t);
    int64_t* buffer = new int64_t[buffer_size];
    
    // Generar números aleatorios por bloques
    for (size_t i = 0; i < num_elementos; i += buffer_size) {
        size_t elementos_a_generar = std::min(buffer_size, num_elementos - i);
        
        for (size_t j = 0; j < elementos_a_generar; j++) {
            buffer[j] = dis(gen);
        }
        
        fwrite(buffer, sizeof(int64_t), elementos_a_generar, archivo);
    }
    
    delete[] buffer;
    fclose(archivo);
    
    std::cout << "Archivo generado: " << nombre_archivo << " con " << num_elementos << " elementos" << std::endl;
}

// Función para verificar si un archivo está ordenado
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

int main(int argc, char* argv[]) {
    // Configuración
    size_t B = 4096;                // Tamaño del bloque (4 KB por defecto)
    size_t M = 50 * 1024 * 1024;    // 50 MB de memoria principal
    size_t a = 8;                   // Aridad (hay que cambiarlo segun el resultado de mergesort)
    
    // Procesar argumentos
    if (argc > 1) {
        B = std::stoul(argv[1]);
    }
    if (argc > 2) {
        M = std::stoul(argv[2]) * 1024 * 1024;  // Convertir MB a bytes
    }
    if (argc > 3) {
        a = std::stoul(argv[3]);
    }
    
    std::cout << "Configuración:" << std::endl;
    std::cout << "- Tamaño de bloque (B): " << B << " bytes" << std::endl;
    std::cout << "- Memoria principal (M): " << M / (1024 * 1024) << " MB" << std::endl;
    std::cout << "- Aridad (a): " << a << std::endl;
    
    // Nombres de archivos
    std::string archivo_entrada = "datos.bin";
    std::string archivo_salida = "datos_ordenados.bin";
    
    // Tamaño de prueba (hay que ajustarlo)
    size_t tamano_prueba = 4 * 1024 * 1024;  // 4M elementos (32 MB con enteros de 64 bits)
    
    // Generar archivo de prueba
    generarArchivo(archivo_entrada, tamano_prueba);
    
    // Crear instancia de QuicksortExterno
    QuicksortExterno quicksort(B, M, a);
    
    // Medir tiempo y ordenar
    auto inicio = std::chrono::high_resolution_clock::now();
    quicksort.ordenar(archivo_entrada, archivo_salida);
    auto fin = std::chrono::high_resolution_clock::now();
    
    // Calcular tiempo
    std::chrono::duration<double> tiempo = fin - inicio;
    
    // Verificar ordenamiento
    bool ordenado = verificarOrdenamiento(archivo_salida);
    
    // Mostrar resultados
    std::cout << "Resultados:" << std::endl;
    std::cout << "- Tiempo de ejecución: " << tiempo.count() << " segundos" << std::endl;
    std::cout << "- Operaciones I/O: " << quicksort.obtenerContadorIO() << std::endl;
    std::cout << "- Archivo ordenado: " << (ordenado ? "SÍ" : "NO") << std::endl;
    
    return 0;
}