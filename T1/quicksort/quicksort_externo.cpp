// quicksort_externo.cpp
// Implementación de QuicksortExterno

#include "quicksort_externo.h"
#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <vector>
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
    // Calculamos el tamaño del rango
    size_t tamano = fin - inicio;
    
    // Si el tamaño es pequeño, no podemos seleccionar suficientes pivotes
    if (tamano <= a) {
        return nullptr;
    }
    
    // Seleccionamos un bloque aleatorio
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<size_t> dis(inicio, fin - 1);
    size_t bloque_aleatorio = dis(gen) / (B / sizeof(int64_t));
    
    // Nos aseguramos de que el bloque está dentro del rango
    bloque_aleatorio = std::max(inicio / (B / sizeof(int64_t)), 
                             std::min(bloque_aleatorio, (fin - 1) / (B / sizeof(int64_t))));
    
    // Leemos el bloque aleatorio
    FILE* archivo_ptr = fopen(archivo.c_str(), "rb");
    int64_t* bloque = new int64_t[B / sizeof(int64_t)];
    leerBloque(archivo_ptr, bloque, bloque_aleatorio);
    fclose(archivo_ptr);
    
    // Copiamos el bloque (para no modificar el original)
    int64_t* copia_bloque = new int64_t[B / sizeof(int64_t)];
    memcpy(copia_bloque, bloque, B);
    delete[] bloque;
    
    // Seleccionanmos (a-1) pivotes
    size_t elementos_por_bloque = B / sizeof(int64_t);
    int64_t* pivotes = new int64_t[a - 1];
    
    // Seleccionamos aleatoriamente (a-1) elementos como pivotes
    std::vector<size_t> indices;
    for (size_t i = 0; i < elementos_por_bloque; i++) {
        indices.push_back(i);
    }
    
    // Mezclamos y tomamos los primeros a-1 elementos
    std::shuffle(indices.begin(), indices.end(), gen);
    
    for (size_t i = 0; i < a - 1 && i < indices.size(); i++) {
        pivotes[i] = copia_bloque[indices[i]];
    }
    
    // Ordenamos los pivotes
    std::sort(pivotes, pivotes + a - 1);
    
    delete[] copia_bloque;
    return pivotes;
}

void QuicksortExterno::particionarArchivo(const std::string& archivo_entrada, 
                                      const std::string& archivo_salida,
                                      size_t inicio, size_t fin, 
                                      int64_t* pivotes, size_t num_pivotes) {
    if (num_pivotes == 0 || pivotes == nullptr) {
        // Si no hay pivotes, simplemente copiamos el archivo
        FILE* entrada = fopen(archivo_entrada.c_str(), "rb");
        FILE* salida = fopen(archivo_salida.c_str(), "wb");
        
        size_t elementos_por_bloque = B / sizeof(int64_t);
        size_t num_bloques = (fin - inicio + elementos_por_bloque - 1) / elementos_por_bloque;
        
        for (size_t i = 0; i < num_bloques; i++) {
            leerBloque(entrada, buffer, inicio / elementos_por_bloque + i);
            escribirBloque(salida, buffer, inicio / elementos_por_bloque + i);
        }
        
        fclose(entrada);
        fclose(salida);
        return;
    }
    
    // Creamos archivos temporales para cada partición
    std::vector<FILE*> archivos_temp;
    for (size_t i = 0; i <= num_pivotes; i++) {
        std::string nombre_temp = archivo_salida + ".temp" + std::to_string(i);
        FILE* temp = fopen(nombre_temp.c_str(), "wb+");
        archivos_temp.push_back(temp);
    }
    
    // Abrimos archivo de entrada
    FILE* entrada = fopen(archivo_entrada.c_str(), "rb");
    
    // Tamaño de elementos por bloque
    size_t elementos_por_bloque = B / sizeof(int64_t);
    
    // Contador para seguir cuántos elementos van en cada partición
    std::vector<size_t> contadores(num_pivotes + 1, 0);
    
    // Leemos los datos y los clasificamos según los pivotes
    for (size_t pos = inicio; pos < fin; pos += elementos_por_bloque) {
        // Leemos un bloque
        leerBloque(entrada, buffer, pos / elementos_por_bloque);
        
        // Procesamos cada elemento del bloque
        size_t elementos_a_procesar = std::min(elementos_por_bloque, fin - pos);
        
        for (size_t i = 0; i < elementos_a_procesar; i++) {
            int64_t elemento = buffer[i];
            
            // Encontrar la partición correcta
            size_t particion = 0;
            while (particion < num_pivotes && elemento >= pivotes[particion]) {
                particion++;
            }
            
            // Calculamos la posición en la partición
            size_t posicion_particion = contadores[particion]++;
            
            // Calculamos en qué bloque va
            size_t bloque_particion = posicion_particion / elementos_por_bloque;
            size_t offset_particion = posicion_particion % elementos_por_bloque;
            
            // Leeemos el bloque si es necesario
            int64_t* bloque_temp = new int64_t[elementos_por_bloque]();
            
            if (offset_particion == 0 || posicion_particion == 0) {
                // Primer elemento del bloque, inicializar bloque
                memset(bloque_temp, 0, B);
            } else {
                // Leer bloque existente
                fseek(archivos_temp[particion], bloque_particion * B, SEEK_SET);
                fread(bloque_temp, sizeof(int64_t), elementos_por_bloque, archivos_temp[particion]);
                contador_io++;
            }
            
            // Insertar elemento
            bloque_temp[offset_particion] = elemento;
            
            // Escribimos en el bloque
            fseek(archivos_temp[particion], bloque_particion * B, SEEK_SET);
            fwrite(bloque_temp, sizeof(int64_t), elementos_por_bloque, archivos_temp[particion]);
            contador_io++;
            
            delete[] bloque_temp;
        }
    }
    
    fclose(entrada);
    
    // Cerramos archivos temporales
    for (auto& archivo : archivos_temp) {
        fclose(archivo);
    }
    
    // Concatenamos archivos temporales en el archivo de salida
    FILE* salida = fopen(archivo_salida.c_str(), "wb");
    size_t pos_actual = inicio;
    
    for (size_t i = 0; i <= num_pivotes; i++) {
        std::string nombre_temp = archivo_salida + ".temp" + std::to_string(i);
        FILE* temp = fopen(nombre_temp.c_str(), "rb");
        
        // Obtenemos el tamaño del archivo
        fseek(temp, 0, SEEK_END);
        size_t tamano_archivo = ftell(temp);
        size_t num_elementos = tamano_archivo / sizeof(int64_t);
        fseek(temp, 0, SEEK_SET);
        
        // Leemos y escribimos por bloques
        for (size_t j = 0; j < num_elementos; j += elementos_por_bloque) {
            size_t elementos_a_leer = std::min(elementos_por_bloque, num_elementos - j);
            
            // Leemos bloque del archivo temporal
            fread(buffer, sizeof(int64_t), elementos_a_leer, temp);
            contador_io++;
            
            // Escribimos en el archivo de salida
            fwrite(buffer, sizeof(int64_t), elementos_a_leer, salida);
            contador_io++;
            
            pos_actual += elementos_a_leer;
        }
        
        fclose(temp);
        remove(nombre_temp.c_str());
    }
    
    fclose(salida);
}

void QuicksortExterno::quicksortRecursivo(const std::string& archivo_entrada, 
                                       const std::string& archivo_salida,
                                       size_t inicio, size_t fin) {
    // Calculamos el número de elementos en este rango
    size_t num_elementos = fin - inicio;
    size_t elementos_por_bloque = B / sizeof(int64_t);
    
    // Si el tamaño es menor o igual a M, ordenamos en memoria principal
    if (num_elementos * sizeof(int64_t) <= M) {
        // Leemos todos los elementos en memoria
        int64_t* datos = new int64_t[num_elementos];
        FILE* entrada = fopen(archivo_entrada.c_str(), "rb");
        
        // Leemos por bloques
        for (size_t pos = inicio; pos < fin; pos += elementos_por_bloque) {
            size_t elementos_a_leer = std::min(elementos_por_bloque, fin - pos);
            leerBloque(entrada, buffer, pos / elementos_por_bloque);
            memcpy(datos + (pos - inicio), buffer, elementos_a_leer * sizeof(int64_t));
        }
        
        fclose(entrada);
        
        // Ordenamos en memoria
        std::sort(datos, datos + num_elementos);
        
        // Escribimos los datos ordenados
        FILE* salida = fopen(archivo_salida.c_str(), "wb");
        
        // Escribimos por bloques
        for (size_t pos = inicio; pos < fin; pos += elementos_por_bloque) {
            size_t elementos_a_escribir = std::min(elementos_por_bloque, fin - pos);
            memcpy(buffer, datos + (pos - inicio), elementos_a_escribir * sizeof(int64_t));
            escribirBloque(salida, buffer, pos / elementos_por_bloque);
        }
        
        fclose(salida);
        delete[] datos;
        return;
    }
    
    // Seleccionamos los pivotes
    int64_t* pivotes = seleccionarPivotes(archivo_entrada, inicio, fin);
    
    // Particionamos el archivo
    std::string archivo_temp = archivo_salida + ".tmp";
    particionarArchivo(archivo_entrada, archivo_temp, inicio, fin, pivotes, a - 1);
    
    // Determinamos las posiciones finales de cada partición
    std::vector<size_t> posiciones_particiones;
    posiciones_particiones.push_back(inicio);
    
    FILE* temp = fopen(archivo_temp.c_str(), "rb");
    
    // Cerramos el archivo (no usaremos el tamaño)
    fclose(temp);
    
    // Si hay pivotes, necesitamos calcular los tamaños de cada partición
    if (pivotes != nullptr) {
        // Creamos archivos temporales para poder contar los elementos
        std::vector<std::string> nombres_temp;
        for (size_t i = 0; i <= a - 1; i++) {
            std::string nombre = archivo_temp + ".part" + std::to_string(i);
            nombres_temp.push_back(nombre);
        }
        
        // Contamos elementos en cada partición
        std::vector<size_t> contadores(a, 0);
        
        // Abrimos el archivo temporal para lectura
        FILE* archivo = fopen(archivo_temp.c_str(), "rb");
        
        // Leemos por bloques y contamos
        for (size_t pos = inicio; pos < fin; pos += elementos_por_bloque) {
            leerBloque(archivo, buffer, pos / elementos_por_bloque);
            size_t elementos_en_bloque = std::min(elementos_por_bloque, fin - pos);
            
            for (size_t i = 0; i < elementos_en_bloque; i++) {
                int64_t elemento = buffer[i];
                
                // Encontrar la partición correcta
                size_t particion = 0;
                while (particion < a - 1 && elemento >= pivotes[particion]) {
                    particion++;
                }
                
                contadores[particion]++;
            }
        }
        
        fclose(archivo);
        
        // Calculamos posiciones acumulativas
        for (size_t i = 0; i < a; i++) {
            posiciones_particiones.push_back(posiciones_particiones.back() + contadores[i]);
        }
    } else {
        // Si no hay pivotes, solo hay una partición
        posiciones_particiones.push_back(fin);
    }
    
    // Ordenamos recursivamente cada partición
    for (size_t i = 0; i < posiciones_particiones.size() - 1; i++) {
        size_t inicio_particion = posiciones_particiones[i];
        size_t fin_particion = posiciones_particiones[i + 1];
        
        if (inicio_particion < fin_particion) {
            std::string archivo_particion = archivo_salida + ".part" + std::to_string(i);
            quicksortRecursivo(archivo_temp, archivo_particion, inicio_particion, fin_particion);
        }
    }
    
    // Concatenamos las particiones ordenadas
    FILE* archivo_final = fopen(archivo_salida.c_str(), "wb");
    
    for (size_t i = 0; i < posiciones_particiones.size() - 1; i++) {
        size_t inicio_particion = posiciones_particiones[i];
        size_t fin_particion = posiciones_particiones[i + 1];
        
        if (inicio_particion < fin_particion) {
            std::string archivo_particion = archivo_salida + ".part" + std::to_string(i);
            FILE* particion = fopen(archivo_particion.c_str(), "rb");
            
            // Leemos y escribimos por bloques
            for (size_t pos = inicio_particion; pos < fin_particion; pos += elementos_por_bloque) {
                size_t elementos_a_procesar = std::min(elementos_por_bloque, fin_particion - pos);
                
                // Leemos el bloque de la partición
                fseek(particion, (pos - inicio_particion) * sizeof(int64_t), SEEK_SET);
                fread(buffer, sizeof(int64_t), elementos_a_procesar, particion);
                contador_io++;
                
                // Escribimos en el archivo final
                fseek(archivo_final, pos * sizeof(int64_t), SEEK_SET);
                fwrite(buffer, sizeof(int64_t), elementos_a_procesar, archivo_final);
                contador_io++;
            }
            
            fclose(particion);
            remove(archivo_particion.c_str());
        }
    }
    
    fclose(archivo_final);
    remove(archivo_temp.c_str());
    
    // Liberamos la memoria de los pivotes
    if (pivotes != nullptr) {
        delete[] pivotes;
    }
}

void QuicksortExterno::ordenar(const std::string& archivo_entrada, const std::string& archivo_salida) {
    // Obtenemos el tamaño del archivo
    FILE* archivo = fopen(archivo_entrada.c_str(), "rb");
    if (!archivo) {
        std::cerr << "Error al abrir el archivo: " << archivo_entrada << std::endl;
        return;
    }
    
    fseek(archivo, 0, SEEK_END);
    size_t tamano_archivo = ftell(archivo);
    size_t num_elementos = tamano_archivo / sizeof(int64_t);
    fclose(archivo);
    
    // Iniciamos el proceso de ordenar
    quicksortRecursivo(archivo_entrada, archivo_salida, 0, num_elementos);
}

size_t QuicksortExterno::obtenerContadorIO() {
    return contador_io;
}

void QuicksortExterno::resetContadorIO() {
    contador_io = 0;
}