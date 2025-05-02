#include "quicksort_externo.h"
#include <chrono>
#include <iostream>
#include <random>
#include <fstream>
#include <vector>
#include <iomanip>
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
    
    // Tamaño de buffer (8 MB para generación más rápida)
    const size_t buffer_size = 8 * 1024 * 1024 / sizeof(int64_t);
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

// Estructura para almacenar resultados
struct Resultado {
    double tiempo_segundos;
    size_t accesos_io;
    
    Resultado() : tiempo_segundos(0), accesos_io(0) {}
    
    Resultado(double t, size_t io) : tiempo_segundos(t), accesos_io(io) {}
};

// Función principal para ejecutar experimentos
int main(int argc, char* argv[]) {
    // Configuración
    size_t B = 4096;                    // Tamaño del bloque (4 KB por defecto)
    size_t M = 50 * 1024 * 1024;       // 50 MB de memoria principal
    size_t a = 8;                       // Aridad (definida por ahora como 8)
    
    // Número de repeticiones por tamaño
    const int num_repeticiones = 5;
    
   // Tamaños a probar: 4M, 8M, 12M, ... 48M (enteros de 64 bits)
    std::vector<size_t> tamanos;
    for (size_t i = 4; i <= 48; i += 4) {
        tamanos.push_back(i * 1024 * 1024); // i millones de elementos
    }

    // Archivo de resultados
    std::ofstream archivo_resultados("resultados_quicksort.csv");
    archivo_resultados << "Tamaño,TiempoPromedio(s),AccesosIOPromedio" << std::endl;
    
    // Para cada tamaño
    for (size_t tamano : tamanos) {
        std::cout << "=====================================" << std::endl;
        std::cout << "Probando tamaño: " << tamano / (1024 * 1024) << "M elementos" << std::endl;
        
        double tiempo_total = 0.0;
        size_t io_total = 0;
        
        // Repetir el experimento varias veces
        for (int rep = 1; rep <= num_repeticiones; rep++) {
            std::cout << "Repetición " << rep << "/" << num_repeticiones << std::endl;
            
            // Nombres de archivos para esta repetición
            std::string archivo_entrada = "datos_" + std::to_string(tamano / (1024 * 1024)) + "M_" + std::to_string(rep) + ".bin";
            std::string archivo_salida = "ordenados_" + std::to_string(tamano / (1024 * 1024)) + "M_" + std::to_string(rep) + ".bin";
            
            // Generar archivo de prueba
            std::cout << "Generando archivo de prueba..." << std::endl;
            generarArchivo(archivo_entrada, tamano / sizeof(int64_t));
            
            // Crear instancia de QuicksortExterno
            QuicksortExterno quicksort(B, M, a);
            
            // Medir tiempo y ordenar
            std::cout << "Ordenando..." << std::endl;
            auto inicio = std::chrono::high_resolution_clock::now();
            quicksort.ordenar(archivo_entrada, archivo_salida);
            auto fin = std::chrono::high_resolution_clock::now();
            
            // Calcular tiempo
            std::chrono::duration<double> tiempo = fin - inicio;
            double tiempo_segundos = tiempo.count();
            size_t accesos_io = quicksort.obtenerContadorIO();
            
            // Verificar ordenamiento
            bool ordenado = verificarOrdenamiento(archivo_salida);
            if (!ordenado) {
                std::cerr << "¡ADVERTENCIA! El archivo no está correctamente ordenado." << std::endl;
            }
            
            // Mostrar resultados de esta repetición
            std::cout << "- Tiempo: " << tiempo_segundos << " segundos" << std::endl;
            std::cout << "- Accesos I/O: " << accesos_io << std::endl;
            std::cout << "- Ordenado: " << (ordenado ? "SÍ" : "NO") << std::endl;
            
            // Acumular resultados
            tiempo_total += tiempo_segundos;
            io_total += accesos_io;
            
            // Eliminar archivos para liberar espacio
            std::cout << "Eliminando archivos temporales..." << std::endl;
            remove(archivo_entrada.c_str());
            remove(archivo_salida.c_str());
        }
        
        // Calcular promedios
        double tiempo_promedio = tiempo_total / num_repeticiones;
        size_t io_promedio = io_total / num_repeticiones;
        
        // Mostrar resultados promedio
        std::cout << "RESULTADOS PROMEDIO PARA " << tamano / (1024 * 1024) << "M:" << std::endl;
        std::cout << "- Tiempo promedio: " << tiempo_promedio << " segundos" << std::endl;
        std::cout << "- Accesos I/O promedio: " << io_promedio << std::endl;
        
        // Guardar en archivo
        archivo_resultados << tamano / (1024 * 1024) << "," << std::fixed << std::setprecision(6) << tiempo_promedio << "," << io_promedio << std::endl;
    }
    
    archivo_resultados.close();
    std::cout << "Experimentos completados. Resultados guardados en 'resultados_quicksort.csv'" << std::endl;
    
    return 0;
}