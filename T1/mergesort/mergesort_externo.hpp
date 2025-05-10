#ifndef MERGESORT_EXTERNO_HPP
#define MERGESORT_EXTERNO_HPP

#include <iostream>
#include <cstdint>
#include <vector>
#include <string>
#include <algorithm>
#include <cstring>

class MergesortExterno {
private:
    size_t B;           // Tamaño de bloque en bytes
    size_t M;           // Tamaño de memoria principal en bytes
    size_t a;           // Aridad del mergesort
    int contadorIO;     // Contador de operaciones I/O
    int64_t* buffer;    // Buffer de lectura/escritura

    // Métodos auxiliares
    void leerBloque(FILE* archivo, int64_t* bloque, size_t posicion);
    void escribirBloque(FILE* archivo, int64_t* bloque, size_t posicion);
    
    std::vector<std::string> dividirArchivo(const std::string& archivo_entrada, const std::string& archivo_salida, size_t inicio, size_t fin);
    void mergeArchivos(const std::vector<std::string>& archivos_temp, const std::string& archivo_salida);
    
    // Nuevo método para ordenar fragmentos que caben en memoria
    void ordenarEnMemoria(const std::string& archivo_entrada, const std::string& archivo_salida, size_t inicio, size_t fin);

public:
    MergesortExterno(size_t tamano_bloque, size_t tamano_memoria, size_t aridad);
    ~MergesortExterno();
    
    // Método principal de ordenamiento (ahora iterativo)
    void mergesort(const std::string& archivo_entrada, const std::string& archivo_salida, size_t N);
    
    // Métodos auxiliares
    int obtenerContadorIO();
    void resetContadorIO();
    void updateAridad(size_t new_a);
    void limpiarBuffer();
};

#endif // MERGESORT_EXTERNO_HPP