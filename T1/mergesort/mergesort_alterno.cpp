#include "mergesort_alterno.hpp"


MergesortExterno::MergesortExterno(size_t tamano_bloque, size_t tamano_memoria, size_t aridad)
    : B(tamano_bloque), M(tamano_memoria), a(aridad){
    buffer = new int64_t[B / sizeof(int64_t)];
}

MergesortExterno::~MergesortExterno(){
    delete[] buffer;
}

void MergesortExterno::leerBloque(FILE* archivo, int64_t* bloque, size_t posicion) {
  fseek(archivo, posicion * B, SEEK_SET); 
  size_t ignorado = fread(bloque, sizeof(int64_t), B / sizeof(int64_t), archivo); 
  (void)ignorado; 
}

void MergesortExterno::escribirBloque(FILE* archivo, int64_t* bloque, size_t posicion) {
  fseek(archivo, posicion * B, SEEK_SET);
  fwrite(bloque, sizeof(int64_t), B / sizeof(int64_t), archivo);
}

void::mergesortIterativo(const std::string& archivo_entrada, const std::string& archivo_salida, size_t inicio, size_t fin){
  
}

void MergesortExterno::mergesort(const std::string& archivo_entrada, const std::string& archivo_salida, size_t N) {
  // Verificar que el archivo existe y obtener su tamaño real si no se especificó
  FILE* archivo = fopen(archivo_entrada.c_str(), "rb");
  
  // Calcular el número real de elementos (int64_t) en el archivo
  size_t num_elementos = N / sizeof(int64_t);
  
  // Llamar a la función recursiva para ordenar el archivo
  mergesortIterativo(archivo_entrada, archivo_salida, 0, num_elementos);
}

