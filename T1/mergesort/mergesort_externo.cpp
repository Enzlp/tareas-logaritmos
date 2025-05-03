#include "mergesort_externo.hpp"


/** 
 * Constructor del mergesort externo, se inicializa declarando el tamaño de bloque, el tamaño de memoria, y la aridad. 
 * Inicializa el contador de I/O en 0, y inicializa un buffer de lectura de tamaño B.
 */
MergesortExterno::MergesortExterno(size_t tamano_bloque, size_t tamano_memoria, size_t aridad)
    : B(tamano_bloque), M(tamano_memoria), a(aridad), contadorIO(0) {
    buffer = new int64_t[B / sizeof(int64_t)];
}

/**
 * Destructor de la estructura Mergesort exteno
 */
MergesortExterno::~MergesortExterno(){
    delete[] buffer;
}

/**
 * Lector de bloques de archivo. 
 * @param archivo archivo que se va a leer
 * @param bloque buffer en el que se va a copiar los valores leidos
 * @param posicion indice en el archivo donde se va a leer
 */
void MergesortExterno::leerBloque(FILE* archivo, int64_t* bloque, size_t posicion) {
    fseek(archivo, posicion * B, SEEK_SET); //Se posiciona el lector en el indice del pedazo a leer
    fread(bloque, sizeof(int64_t), B / sizeof(int64_t), archivo); //se lee el pedazo de archivo y se copia al bloque
    contadorIO++; //Se aumenta el numero de I/O
}


/**
 * Escritor de bloques de archivo
 * @param archivo archivo sobre el que se va a escribir
 * @param bloque buffer de datos desde el que se va a copiar los datos a escribir
 * @param posicion indice en el archivo, donde se va escribir
 */
void MergesortExterno::escribirBloque(FILE* archivo, int64_t* bloque, size_t posicion) {
    fseek(archivo, posicion * B, SEEK_SET);
    fwrite(bloque, sizeof(int64_t), B / sizeof(int64_t), archivo);
    contadorIO++;
}

/**
 * Divide un archivos en multiples archivos temporales segun la aridad definida
 * @param archivo_entrada nombre del archivo de entrada que se va a dividir
 * @param archivo_salida nombre del archivo de salida donde se va a escribir el resultado final
 * @param inicio indice de inicio del archivo 
 * @param fin posicion del final del archivo
 * 
 * @return vector con los nombres de los archivos temporales creados
 */
std::vector<std::string> MergesortExterno::dividirArchivo(const std::string& archivo_entrada, const std::string& archivo_salida, size_t inicio, size_t fin) {
    //vector para almacenar los nombres de los archivos temporales creados
    std::vector<std::string> nombres_temp;

    //Leemos archivo de entrada
    FILE* entrada = fopen(archivo_entrada.c_str(), "rb"); 

    //calculamos elementos por bloque y bloques totales
    size_t elementos_por_bloque = this->B / sizeof(int64_t); 
    size_t num_bloques = (fin - inicio) / elementos_por_bloque;

    //calculamos bloques en archivo
    size_t bloques_por_archivo = num_bloques / this->a;

    // creamos los archivos temporales
    std::vector<FILE*> archivos_temp; //vector para almacenar archivos temporales creados
    for (size_t i = 0; i < this->a; i++) {
        std::string name = archivo_salida + ".temp" + std::to_string(i);
        FILE* temp = fopen(name.c_str(), "wb+");
        nombres_temp.push_back(name); //agregamos nombre del archivo temporal al vector
        archivos_temp.push_back(temp); //agregamos el archivo temporal creado al arreglo
    }

    //copiamos los valores del archivo de entrada a los archivos temporales creados
    size_t pos = inicio / elementos_por_bloque; 
    for (size_t i = 0; i < this->a; ++i) {
        for (size_t j = 0; j < bloques_por_archivo; ++j) {
            leerBloque(entrada, this->buffer, pos);
            escribirBloque(archivos_temp[i], this->buffer, j);
            pos++;
        }
    }
    
    //cerrar todos los archivo temporales
    for (auto& archivo : archivos_temp) {
        fclose(archivo);
    }

    //archivo de entrada cerrado 
    fclose(entrada);

    return nombres_temp;
}

/**
 * mezcla los archivos temporales que pertenecen al mismo archivo original, manteniendo orden del arreglo
 * @param archivos_temp vector con los nombres de los archivos temporales para este nivel
 * @param archivo_salida nombre del archivo de salida al mezclar los archivos temporales
 */
void MergesortExterno::mergeArchivos(const std::vector<std::string>& archivos_temp, const std::string& archivo_salida) {
    size_t elementos_por_bloque = B / sizeof(int64_t); // cuántos elementos caben en un bloque B
    std::vector<FILE*> archivos; // vector para almacenar los punteros a los archivos temporales abiertos
    std::vector<int64_t> top(archivos_temp.size(), 0); //vector guarda el primer elemento de cada archivo temporal
    std::vector<bool> fin_archivo(archivos_temp.size(), false); // vector indica si un archivo temporal ya ha sido procesado o no
    std::vector<size_t> indices(archivos_temp.size(), 0); //  Vector de índices para saber qué parte de cada archivo se está leyendo actualmente
    std::vector<std::vector<int64_t>> buffers(archivos_temp.size()); //buffer de lectura para cada archivo temporal

    // Abrir archivos temporales
    for (const auto& nombre : archivos_temp) {
        FILE* archivo = fopen(nombre.c_str(), "rb");
        archivos.push_back(archivo);
        buffers.push_back(std::vector<int64_t>(elementos_por_bloque));
    }

    // Leer el primer bloque de cada archivo
    for (size_t i = 0; i < archivos.size(); ++i) {
        if (fread(buffers[i].data(), sizeof(int64_t), elementos_por_bloque, archivos[i]) > 0) {
            top[i] = buffers[i][0]; //primer elemento de cada bloque
        } else {
            fin_archivo[i] = true;
        }
    }

    FILE* salida = fopen(archivo_salida.c_str(), "wb");
    std::vector<int64_t> buffer_salida;
    buffer_salida.reserve(elementos_por_bloque);

    //Mezcla de los archivos temporales
    while (true) {
        int min_index = -1;
        for (size_t i = 0; i < archivos.size(); ++i) {
            if (!fin_archivo[i]) {
                if (min_index == -1 || top[i] < top[min_index]) {
                    min_index = i;
                }
            }
        }

        if (min_index == -1) break; // Todos los archivos han terminado

        buffer_salida.push_back(top[min_index]);
        indices[min_index]++;

        if (indices[min_index] >= elementos_por_bloque || buffers[min_index][indices[min_index]] == 0) {
            size_t leidos = fread(buffers[min_index].data(), sizeof(int64_t), elementos_por_bloque, archivos[min_index]);
            if (leidos > 0) {
                top[min_index] = buffers[min_index][0];
                indices[min_index] = 0;
            } else {
                fin_archivo[min_index] = true;
            }
        } else {
            top[min_index] = buffers[min_index][indices[min_index]];
        }

        if (buffer_salida.size() == elementos_por_bloque) {
            escribirBloque(salida, buffer_salida.data(), ftell(salida) / B);
            buffer_salida.clear();
        }
    }

    if (!buffer_salida.empty()) {
        escribirBloque(salida, buffer_salida.data(), ftell(salida) / B);
    }

    //Cerramos los archivos temporales
    for (auto& archivo : archivos) fclose(archivo);
}

/**
 * Función recursiva que invoca la subdivisión de archivos y su posterior mezcla para ir ordenando el archivo original.
 * Si el tamaño del fragmento es lo suficientemente pequeño como para caber en memoria, se ordena directamente. Si no se subdivide, ordena
 * y luego se fusionan.
 *
 * @param archivo_entrada Nombre del archivo actual que se va a subdividir y posteriormente mezclar.
 * @param archivo_salida Nombre del archivo donde se escribirá la salida ordenada.
 * @param inicio Índice inicial del segmento del archivo.
 * @param fin Índice final del archivo.
 */

void MergesortExterno::mergesortRecursivo(const std::string& archivo_entrada, const std::string& archivo_salida, size_t inicio, size_t fin) {

    //Numero de elementos en este archivo actual
    size_t num_elementos = fin - inicio;

    // Caso base: si el número de elementos es menor a la memoria, los ordenamos en memoria principal
    if (num_elementos * sizeof(int64_t) <= this->M) {
        int64_t* data = new int64_t[num_elementos];
        FILE* entrada = fopen(archivo_entrada.c_str(), "rb");
        size_t elementos_por_bloque = B / sizeof(int64_t);

        for (size_t i = 0; i < num_elementos; i += elementos_por_bloque) {
            size_t elementos_a_leer = std::min(elementos_por_bloque, num_elementos - i);
            leerBloque(entrada, buffer, (inicio + i) / elementos_por_bloque);
            memcpy(data + i, buffer, elementos_a_leer * sizeof(int64_t));
        }

        fclose(entrada);

        // Ordenar los datos leídos
        std::sort(data, data + num_elementos);

        // Escribir los datos ordenados al archivo de salida
        FILE* salida = fopen(archivo_salida.c_str(), "wb");
        for (size_t i = 0; i < num_elementos; i += elementos_por_bloque) {
            size_t elementos_a_escribir = std::min(elementos_por_bloque, num_elementos - i);
            memcpy(buffer, data + i, elementos_a_escribir * sizeof(int64_t));
            escribirBloque(salida, buffer, i / elementos_por_bloque);
        }

        fclose(salida);
        delete[] data;
        return;
    }

    // División recursiva: dividimos el archivo en varios subarchivos
    std::string archivo_base = archivo_salida + "_nivel";
    std::vector<std::string> nombres_temp = dividirArchivo(archivo_entrada, archivo_base, inicio, fin);

    // Implementamos la llamada recursiva
    std::vector<std::string> nombres_temp_originales; //guardamos copia de los nombres actuales para posterior cierre y borrado
    for (size_t i = 0; i < this->a; i++) {
        std::string temp_name = archivo_base + ".temp" + std::to_string(i);
        std::string sorted_name = archivo_base + ".sorted" + std::to_string(i);
        
        // Llamada recursiva sobre cada archivo temporal generado
        mergesortRecursivo(temp_name, sorted_name, 0, (fin - inicio) / this->a);
        
        nombres_temp.push_back(sorted_name);
        nombres_temp_originales.push_back(temp_name);
    }

    // Fusión de los archivos temporales ya ordenados
    mergeArchivos(nombres_temp, archivo_salida);

    // Eliminar los archivos temporales originales y los ordenados
    for (const auto& nombre : nombres_temp) {
        remove(nombre.c_str());
    }
    for (const auto& nombre : nombres_temp_originales) {
        remove(nombre.c_str());
    }
}

/**
 * Invoca a la funcion recursiva
 * @param archivo_entrada nombre del archivo a ordenar
 * @param archivo_salida nombre del archivo a generar con el resultado ya ordenado
 * @param N tamaño del archivo original
 */
void MergesortExterno::mergesort(const std::string& archivo_entrada, const std::string& archivo_salida, size_t N) {
    size_t num_elementos = N / sizeof(int64_t);
    mergesortRecursivo(archivo_entrada, archivo_salida, 0, num_elementos);
}


/**
 * Obtiene el contador de I/O
 * @return contador de I/O
 */
int MergesortExterno::obtenerContadorIO() {
    return contadorIO;
}

/**
 * Reinicia el contador de IO
 */
void MergesortExterno::resetContadorIO() {
    contadorIO = 0;
}

/**
 * Actualiza la aridad del mergesort
 */
void MergesortExterno::updateAridad(size_t new_a){
    this->a = new_a;
}

/** 
 * Actualiza el valor de memoria principal M
 */
void MergesortExterno::updateMemoria(size_t new_M){
    this->M = new_M;
}

/**
 * Actulaiza el bloque B de memoria
 */
void MergesortExterno::updateBloque(size_t new_B){
    this->B = new_B;
}