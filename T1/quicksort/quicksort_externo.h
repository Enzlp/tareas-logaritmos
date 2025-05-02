// quicksort_externo.h
// Archivo de cabecera para Quicksort Externo que incluye definiciones y funciones necesarias

#ifndef QUICKSORT_EXTERNO_H
#define QUICKSORT_EXTERNO_H

#include <cstdint>
#include <string>

class QuicksortExterno {
private:
    size_t B; // Tamaño de bloque
    size_t M; // Tamaño máximo en memoria principal
    size_t a; // Número de subarreglos (pivotes+1)
    size_t contador_io; // Contador de operaciones I/O

    // Buffer para lectura/escritura
    int64_t* buffer;
    
    // Funciones auxiliares
    void particionarArchivo(const std::string& archivo_entrada, 
                           const std::string& archivo_salida,
                           size_t inicio, size_t fin, int64_t* pivotes, size_t num_pivotes);
    
    void quicksortRecursivo(const std::string& archivo_entrada, 
                           const std::string& archivo_salida,
                           size_t inicio, size_t fin);
    
    // Funciones para manipulación de archivos
    void leerBloque(FILE* archivo, int64_t* bloque, size_t posicion);
    void escribirBloque(FILE* archivo, int64_t* bloque, size_t posicion);
    int64_t* seleccionarPivotes(const std::string& archivo, 
                               size_t inicio, size_t fin);

public:
    QuicksortExterno(size_t tamano_bloque, size_t memoria_maxima, size_t aridad);
    ~QuicksortExterno();
    
    // Función principal
    void ordenar(const std::string& archivo_entrada, const std::string& archivo_salida);
    
    // Obtener estadísticas
    size_t obtenerContadorIO();
    void resetContadorIO();
};

#endif // QUICKSORT_EXTERNO_H