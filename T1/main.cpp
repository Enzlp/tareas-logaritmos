#include <sys/statvfs.h>
#include <iostream>
#include "mergesort_externo.hpp"
#include <limits>

//Header
int busqueda_ternaria(int left, int right, size_t M, size_t B, std::string& archivo_entrada, size_t tamano_archivo);

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
  MergesortExterno mergesort_search(B, M , 0);
  std::string archivo_salida = "busqueda.bin";
  
  while(right-left>4){
    int mid1 = left + (right - left) / 3;
    int mid2 = right - (right - left) / 3;

    mergesort_search.updateAridad(static_cast<size_t>(mid1));
    mergesort_search.mergesort(archivo_entrada, archivo_salida , tamano_archivo);
    int sort_mid1 = mergesort_search.obtenerContadorIO();
    mergesort_search.resetContadorIO();

    mergesort_search.updateAridad(static_cast<size_t>(mid2));
    mergesort_search.mergesort(archivo_entrada, archivo_salida , tamano_archivo);
    int sort_mid2 = mergesort_search.obtenerContadorIO();
    mergesort_search.resetContadorIO();

    if (sort_mid1 < sort_mid2) {
        right = mid2;
    } else {
        left = mid1;
    }
  }

  for(int i = left; i <= right; i++){
    mergesort_search.updateAridad(static_cast<size_t>(i));
    mergesort_search.mergesort(archivo_entrada, archivo_salida , tamano_archivo);
    min = std::min(mergesort_search.obtenerContadorIO(), min);
    mergesort_search.resetContadorIO();
  }

  std::remove(archivo_salida.c_str());
  return min;
}

int main(){
  return 0;
}