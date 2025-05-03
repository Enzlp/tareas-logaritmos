#ifndef MERGESORT_EXTERNO_H
#define MERGESORT_EXTERNO_H

#include <cstdint>
#include <string>
#include <cstdio> 
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <random>
#include <iostream>
using namespace std;


/** 
 * Header para la clase constructura del mergesort externo
 */
class MergesortExterno {
private:
    size_t B;   //Tamaño del bloque
    size_t M;   //Tamaño de memoria principal
    size_t a;   //Aridad de particiones          
    int contadorIO;   //Contador I/O
    int64_t* buffer; //buffer para lectura y escritura de bloques

public:
    // Constructor y destructor
    MergesortExterno(size_t tamano_bloque, size_t tamano_memoria,size_t aridad);
    ~MergesortExterno();

    // Método principal de mergesort
    void mergesort(const std::string& archivo_entrada, const std::string& archivo_salida, size_t N);

    // Método recursivo para el merge sort
    void mergesortRecursivo(const std::string& archivo_entrada, const std::string& archivo_salida, size_t inicio, size_t fin);

    // Funciones auxiliares para dividir y mezclar archivos
    std::vector<std::string> dividirArchivo(const std::string& archivo_entrada, const std::string& archivo_salida, size_t inicio, size_t fin);
    void mergeArchivos(const std::vector<std::string>& archivos_temp, const std::string& archivo_salida);

    // Lectura y escritura de bloques
    void leerBloque(FILE* archivo, int64_t* bloque, size_t posicion);
    void escribirBloque(FILE* archivo, int64_t* bloque, size_t posicion);

    // Obtener estadísticas
    int obtenerContadorIO();
    void resetContadorIO();

    //Cambio valores internos
    void updateAridad(size_t new_a);
    void updateMemoria(size_t new_M);
    void limpiarBuffer();

};

#endif
