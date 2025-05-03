// Quicksort Externo V2 - mas optimizado
// Particionamiento más directo
// Mejor manejo de archivos temporales con nombres más simples
// Simplificación de la recursión
// Reducción de código redundante

// quicksort_externo.cpp
#include "quicksort_externo.h"
#include <algorithm>
#include <cstring>
#include <random>
#include <iostream>

QuicksortExterno::QuicksortExterno(size_t tamano_bloque, size_t memoria_maxima, size_t aridad) 
    : B(tamano_bloque), M(memoria_maxima), a(aridad), contador_io(0) {
    buffer = new int64_t[B / sizeof(int64_t)];
}

QuicksortExterno::~QuicksortExterno() {
    delete[] buffer;
}

void QuicksortExterno::leerBloque(FILE* archivo, int64_t* bloque, size_t posicion) {
    fseek(archivo, posicion * B, SEEK_SET);
    fread(bloque, sizeof(int64_t), B / sizeof(int64_t), archivo);
    contador_io++;
}

void QuicksortExterno::escribirBloque(FILE* archivo, int64_t* bloque, size_t posicion) {
    fseek(archivo, posicion * B, SEEK_SET);
    fwrite(bloque, sizeof(int64_t), B / sizeof(int64_t), archivo);
    contador_io++;
}

int64_t* QuicksortExterno::seleccionarPivotes(const std::string& archivo, size_t inicio, size_t fin) {
    if (fin - inicio <= a) {
        return nullptr;  // No hay suficientes elementos para seleccionar pivotes
    }
    
    // Seleccionar bloque aleatorio
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<size_t> dis(inicio, fin - 1);
    size_t elementos_por_bloque = B / sizeof(int64_t);
    size_t bloque_aleatorio = dis(gen) / elementos_por_bloque;
    
    // Leer el bloque
    FILE* archivo_ptr = fopen(archivo.c_str(), "rb");
    int64_t* bloque = new int64_t[elementos_por_bloque];
    leerBloque(archivo_ptr, bloque, bloque_aleatorio);
    fclose(archivo_ptr);
    
    // Seleccionar a-1 elementos aleatorios como pivotes
    int64_t* pivotes = new int64_t[a - 1];
    size_t max_pivotes = std::min(a - 1, elementos_por_bloque);
    
    // Crear vector de índices y mezclarlos
    std::vector<size_t> indices(elementos_por_bloque);
    for (size_t i = 0; i < elementos_por_bloque; i++) {
        indices[i] = i;
    }
    std::shuffle(indices.begin(), indices.end(), gen);
    
    // Tomar los primeros a-1 elementos como pivotes
    for (size_t i = 0; i < max_pivotes; i++) {
        pivotes[i] = bloque[indices[i]];
    }
    
    // Ordenar los pivotes
    std::sort(pivotes, pivotes + max_pivotes);
    
    delete[] bloque;
    return pivotes;
}

void QuicksortExterno::particionarArchivo(const std::string& archivo_entrada, 
                                       std::string* archivos_particiones,
                                       size_t inicio, size_t fin, 
                                       int64_t* pivotes, size_t num_pivotes,
                                       size_t* tamanos_particiones) {
    size_t elementos_por_bloque = B / sizeof(int64_t);
    size_t num_particiones = num_pivotes + 1;
    
    // Inicializar contadores de particiones
    std::fill(tamanos_particiones, tamanos_particiones + num_particiones, 0);
    
    // Abrir archivos para las particiones
    FILE** archivos = new FILE*[num_particiones];
    for (size_t i = 0; i < num_particiones; i++) {
        archivos[i] = fopen(archivos_particiones[i].c_str(), "wb");
    }
    
    // Abrir archivo de entrada
    FILE* entrada = fopen(archivo_entrada.c_str(), "rb");
    
    // Leer datos y clasificarlos según los pivotes
    int64_t* bloque_entrada = new int64_t[elementos_por_bloque];
    int64_t** bloques_salida = new int64_t*[num_particiones];
    size_t* posiciones_escritura = new size_t[num_particiones]();
    
    // Inicializar buffers de salida
    for (size_t i = 0; i < num_particiones; i++) {
        bloques_salida[i] = new int64_t[elementos_por_bloque]();
    }
    
    // Procesar archivo bloque por bloque
    for (size_t pos = inicio; pos < fin; pos += elementos_por_bloque) {
        // Leer un bloque
        size_t bloque_actual = pos / elementos_por_bloque;
        leerBloque(entrada, bloque_entrada, bloque_actual);
        
        // Procesar cada elemento del bloque
        size_t elementos_a_procesar = std::min(elementos_por_bloque, fin - pos);
        
        for (size_t i = 0; i < elementos_a_procesar; i++) {
            int64_t elemento = bloque_entrada[i];
            
            // Encontrar la partición correcta
            size_t particion = 0;
            while (particion < num_pivotes && elemento >= pivotes[particion]) {
                particion++;
            }
            
            // Agregar al buffer de la partición
            bloques_salida[particion][posiciones_escritura[particion]++] = elemento;
            tamanos_particiones[particion]++;
            
            // Si el buffer está lleno, escribirlo en disco
            if (posiciones_escritura[particion] == elementos_por_bloque) {
                escribirBloque(archivos[particion], bloques_salida[particion], 
                              tamanos_particiones[particion] / elementos_por_bloque - 1);
                posiciones_escritura[particion] = 0;
            }
        }
    }
    
    // Escribir los buffers restantes
    for (size_t i = 0; i < num_particiones; i++) {
        if (posiciones_escritura[i] > 0) {
            // Limpiar el resto del buffer
            if (posiciones_escritura[i] < elementos_por_bloque) {
                memset(bloques_salida[i] + posiciones_escritura[i], 0, 
                      (elementos_por_bloque - posiciones_escritura[i]) * sizeof(int64_t));
            }
            
            escribirBloque(archivos[i], bloques_salida[i], 
                          tamanos_particiones[i] / elementos_por_bloque);
        }
    }
    
    // Cerrar archivos y liberar memoria
    fclose(entrada);
    for (size_t i = 0; i < num_particiones; i++) {
        fclose(archivos[i]);
        delete[] bloques_salida[i];
    }
    
    delete[] bloque_entrada;
    delete[] bloques_salida;
    delete[] posiciones_escritura;
    delete[] archivos;
}

void QuicksortExterno::quicksortRecursivo(const std::string& archivo_entrada, 
                                       const std::string& archivo_salida,
                                       size_t inicio, size_t fin) {
    // Calcular el número de elementos en este rango
    size_t num_elementos = fin - inicio;
    size_t elementos_por_bloque = B / sizeof(int64_t);
    
    // Si el tamaño es menor o igual a M, ordenar en memoria principal
    if (num_elementos * sizeof(int64_t) <= M) {
        // Leer todos los elementos en memoria
        int64_t* datos = new int64_t[num_elementos];
        FILE* entrada = fopen(archivo_entrada.c_str(), "rb");
        
        for (size_t pos = inicio; pos < fin; pos += elementos_por_bloque) {
            size_t bloque_actual = pos / elementos_por_bloque;
            leerBloque(entrada, buffer, bloque_actual);
            
            size_t offset_bloque = pos % elementos_por_bloque;
            size_t elementos_a_copiar = std::min(elementos_por_bloque - offset_bloque, fin - pos);
            
            memcpy(datos + (pos - inicio), buffer + offset_bloque, 
                  elementos_a_copiar * sizeof(int64_t));
        }
        
        fclose(entrada);
        
        // Ordenar en memoria
        std::sort(datos, datos + num_elementos);
        
        // Escribir datos ordenados
        FILE* salida = fopen(archivo_salida.c_str(), "wb");
        
        for (size_t pos = 0; pos < num_elementos; pos += elementos_por_bloque) {
            size_t elementos_a_escribir = std::min(elementos_por_bloque, num_elementos - pos);
            memcpy(buffer, datos + pos, elementos_a_escribir * sizeof(int64_t));
            
            // Llenar el resto del buffer con ceros si es necesario
            if (elementos_a_escribir < elementos_por_bloque) {
                memset(buffer + elementos_a_escribir, 0, 
                      (elementos_por_bloque - elementos_a_escribir) * sizeof(int64_t));
            }
            
            escribirBloque(salida, buffer, pos / elementos_por_bloque);
        }
        
        fclose(salida);
        delete[] datos;
        return;
    }
    
    // Seleccionar pivotes
    int64_t* pivotes = seleccionarPivotes(archivo_entrada, inicio, fin);
    size_t num_pivotes = pivotes ? a - 1 : 0;
    size_t num_particiones = num_pivotes + 1;
    
    // Nombres de archivos temporales para las particiones
    std::string* archivos_particiones = new std::string[num_particiones];
    for (size_t i = 0; i < num_particiones; i++) {
        archivos_particiones[i] = archivo_salida + ".part" + std::to_string(i);
    }
    
    // Tamaños de las particiones
    size_t* tamanos_particiones = new size_t[num_particiones]();
    
    // Particionar el archivo
    particionarArchivo(archivo_entrada, archivos_particiones, inicio, fin, 
                       pivotes, num_pivotes, tamanos_particiones);
    
    // Ordenar recursivamente cada partición
    size_t pos_actual = inicio;
    for (size_t i = 0; i < num_particiones; i++) {
        if (tamanos_particiones[i] > 0) {
            std::string archivo_ordenado = archivos_particiones[i] + ".sorted";
            quicksortRecursivo(archivos_particiones[i], archivo_ordenado, 0, tamanos_particiones[i]);
            
            // Renombrar archivo ordenado
            remove(archivos_particiones[i].c_str());
            rename(archivo_ordenado.c_str(), archivos_particiones[i].c_str());
        }
    }
    
    // Concatenar particiones ordenadas en el archivo final
    FILE* salida = fopen(archivo_salida.c_str(), "wb");
    
    for (size_t i = 0; i < num_particiones; i++) {
        if (tamanos_particiones[i] > 0) {
            FILE* particion = fopen(archivos_particiones[i].c_str(), "rb");
            
            // Copiar datos de la partición al archivo final
            for (size_t j = 0; j < tamanos_particiones[i]; j += elementos_por_bloque) {
                // size_t elementos_a_copiar = std::min(elementos_por_bloque, tamanos_particiones[i] - j);
                
                leerBloque(particion, buffer, j / elementos_por_bloque);
                escribirBloque(salida, buffer, (pos_actual + j) / elementos_por_bloque);
            }
            
            pos_actual += tamanos_particiones[i];
            fclose(particion);
            remove(archivos_particiones[i].c_str());
        }
    }
    
    fclose(salida);
    
    // Liberar memoria
    delete[] archivos_particiones;
    delete[] tamanos_particiones;
    if (pivotes) delete[] pivotes;
}

void QuicksortExterno::ordenar(const std::string& archivo_entrada, const std::string& archivo_salida) {
    // Obtener tamaño del archivo
    FILE* archivo = fopen(archivo_entrada.c_str(), "rb");
    if (!archivo) {
        std::cerr << "Error al abrir el archivo: " << archivo_entrada << std::endl;
        return;
    }
    
    fseek(archivo, 0, SEEK_END);
    size_t tamano_archivo = ftell(archivo);
    size_t num_elementos = tamano_archivo / sizeof(int64_t);
    fclose(archivo);
    
    // Iniciar el proceso de ordenamiento
    quicksortRecursivo(archivo_entrada, archivo_salida, 0, num_elementos);
}

size_t QuicksortExterno::obtenerContadorIO() {
    return contador_io;
}

void QuicksortExterno::resetContadorIO() {
    contador_io = 0;
}