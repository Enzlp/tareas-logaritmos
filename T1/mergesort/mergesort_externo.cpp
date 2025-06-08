#include "mergesort_externo.hpp"
#include <queue>
#include <stack>

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
    fseek(archivo, posicion * B, SEEK_SET); // Se posiciona el lector en el índice del bloque
    size_t ignorado = fread(bloque, sizeof(int64_t), B / sizeof(int64_t), archivo); // Se lee el bloque
    (void)ignorado; // Se ignora el valor es solo para que no salgan flags al compilar
    contadorIO++; 
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
    // Vector para almacenar los nombres de los archivos temporales creados
    std::vector<std::string> nombres_temp;

    // Calculamos elementos por bloque
    size_t elementos_por_bloque = B / sizeof(int64_t);

    // Calculamos el número total de elementos
    size_t num_elementos = fin - inicio;
    
    // Abrimos el archivo de entrada
    FILE* entrada = fopen(archivo_entrada.c_str(), "rb");

    // Calculamos elementos por archivo temporal (distribución equitativa)
    size_t elementos_por_archivo = (num_elementos + a - 1) / a; // Redondeamos hacia arriba
    
    // Creamos y abrimos los archivos temporales
    std::vector<FILE*> archivos_temp;
    for (size_t i = 0; i < a; i++) {
        std::string nombre = archivo_salida + ".temp" + std::to_string(i);
        FILE* temp = fopen(nombre.c_str(), "wb+");

        nombres_temp.push_back(nombre);
        archivos_temp.push_back(temp);
    }

    // Posicionamos el archivo de entrada en el inicio correcto
    if (fseek(entrada, inicio * sizeof(int64_t), SEEK_SET) != 0) {
        std::cerr << "Error: No se pudo posicionar en el archivo de entrada" << std::endl;
        fclose(entrada);
        for (auto& archivo : archivos_temp) {
            fclose(archivo);
        }
        return nombres_temp;
    }

    // Distribuimos los elementos del archivo de entrada entre los archivos temporales
    for (size_t i = 0; i < num_elementos; ) {
        // Determinamos a qué archivo temporal va este bloque
        size_t archivo_idx = i / elementos_por_archivo;
        if (archivo_idx >= a) archivo_idx = a - 1; // Para asegurar que no excedemos la aridad
        
        // Leemos un bloque del archivo de entrada
        size_t elementos_a_leer = std::min(elementos_por_bloque, num_elementos - i); //Menor o igual B
        size_t leidos = fread(buffer, sizeof(int64_t), elementos_a_leer, entrada);
        contadorIO++;
        
        if (leidos == 0) break; // Si no leímos nada, salimos del bucle
        
        // Escribimos el bloque en el archivo temporal correspondiente
        fwrite(buffer, sizeof(int64_t), leidos, archivos_temp[archivo_idx]);
        contadorIO++;
        
        i += leidos;
    }
    
    // Cerramos todos los archivos
    fclose(entrada);
    for (auto& archivo : archivos_temp) {
        fclose(archivo);
    }
    
    return nombres_temp;
}

/**
 * Ordena un pequeño archivo que cabe en memoria
 * @param archivo_entrada Nombre del archivo a ordenar
 * @param archivo_salida Nombre del archivo de salida
 * @param inicio Índice inicial en el archivo
 * @param fin Índice final en el archivo
 */
void MergesortExterno::ordenarEnMemoria(const std::string& archivo_entrada, const std::string& archivo_salida, size_t inicio, size_t fin) {
    size_t num_elementos = fin - inicio;
    
    // Reservar memoria para todos los elementos
    int64_t* data = new int64_t[num_elementos];
    
    // Abrir archivo de entrada
    FILE* entrada = fopen(archivo_entrada.c_str(), "rb");
    
    // Posicionar en el punto de inicio
    if (fseek(entrada, inicio * sizeof(int64_t), SEEK_SET) != 0) {
        std::cerr << "Error: No se pudo posicionar en el archivo de entrada" << std::endl;
        fclose(entrada);
        delete[] data;
        return;
    }
    
    // Leer todos los elementos
    size_t posicion_data = 0;  // Índice para controlar la posición dentro de 'data'
    size_t elementos_por_bloque = B / sizeof(int64_t);  // Número de elementos que caben en un bloque
    size_t elementos_leidos = 0;
    while (posicion_data < num_elementos) {
        // Calcula cuántos elementos leer en este bloque
        size_t elementos_a_leer = std::min(elementos_por_bloque, num_elementos - posicion_data);

        // Lee un bloque del archivo
        leerBloque(entrada, buffer, posicion_data / elementos_por_bloque);

        // Copia el contenido del buffer a 'data' de forma secuencial
        memcpy(data + posicion_data, buffer, elementos_a_leer * sizeof(int64_t));

        // Actualiza la posición en 'data'
        posicion_data += elementos_a_leer;
        elementos_leidos += elementos_a_leer;
    }
    fclose(entrada);
    
    // Ordenar los datos leídos
    std::sort(data, data + elementos_leidos);
    
    // Escribir los datos ordenados al archivo de salida
    FILE* salida = fopen(archivo_salida.c_str(), "wb");

    //Escribimos en el archivo desde data
    posicion_data = 0;  
    while (posicion_data < elementos_leidos) {
        // Calcula cuántos elementos a escribir en este bloque
        size_t elementos_a_escribir = std::min(elementos_por_bloque, elementos_leidos - posicion_data);
        // Copia el bloque correspondiente en el buffer
        memcpy(buffer, data + posicion_data, elementos_a_escribir * sizeof(int64_t));
        // Escribe el bloque en el archivo
        escribirBloque(salida, buffer, posicion_data / elementos_por_bloque);
        // Actualiza la posición en 'data'
        posicion_data += elementos_a_escribir;
    }
    fclose(salida);
    
    delete[] data;
}

/**
 * mezcla los archivos temporales que pertenecen al mismo archivo original, manteniendo orden del arreglo
 * @param archivos_temp vector con los nombres de los archivos temporales para este nivel
 * @param archivo_salida nombre del archivo de salida al mezclar los archivos temporales
 */
void MergesortExterno::mergeArchivos(const std::vector<std::string>& archivos_temp, const std::string& archivo_salida) {
    const size_t elementos_por_bloque = B / sizeof(int64_t);
    
    // Estructuras para manejar cada archivo temporal
    struct ArchivoTemp {
        FILE* archivo;
        std::vector<int64_t> buffer;
        size_t pos_actual;      // Posición actual en el buffer
        size_t elementos_leidos; // Elementos válidos en el buffer
        bool fin_archivo;       // Indicador de fin de archivo
    };
    
    std::vector<ArchivoTemp> archivos(archivos_temp.size());
    
    // Abrir todos los archivos temporales e inicializar buffers
    for (size_t i = 0; i < archivos_temp.size(); ++i) {
        archivos[i].archivo = fopen(archivos_temp[i].c_str(), "rb");
        if (!archivos[i].archivo) {
            // Manejar error de apertura de archivo
            std::cerr << "Error al abrir archivo temporal: " << archivos_temp[i] << std::endl;
            
            // Cerrar archivos ya abiertos
            for (size_t j = 0; j < i; ++j) {
                fclose(archivos[j].archivo);
            }
            return;
        }
        
        archivos[i].buffer.resize(elementos_por_bloque);
        archivos[i].pos_actual = 0;
        
        // Leer el primer bloque de cada archivo
        archivos[i].elementos_leidos = fread(archivos[i].buffer.data(), sizeof(int64_t), 
                                            elementos_por_bloque, archivos[i].archivo);
        contadorIO++; // Contar operación I/O
        
        archivos[i].fin_archivo = (archivos[i].elementos_leidos == 0);
    }
    
    // Abrir archivo de salida
    FILE* salida = fopen(archivo_salida.c_str(), "wb");
    if (!salida) {
        std::cerr << "Error al abrir archivo de salida: " << archivo_salida << std::endl;
        // Cerrar archivos temporales
        for (auto& archivo : archivos) {
            fclose(archivo.archivo);
        }
        return;
    }
    
    // Buffer para escribir en archivo de salida
    std::vector<int64_t> buffer_salida;
    buffer_salida.resize(elementos_por_bloque);
    size_t pos_buffer_salida = 0;
    
    // Proceso de mezcla
    while (true) {
        // Encontrar el valor mínimo entre todos los archivos
        int64_t min_valor = INT64_MAX;
        int min_indice = -1;
        
        for (size_t i = 0; i < archivos.size(); ++i) {
            if (!archivos[i].fin_archivo && 
                archivos[i].pos_actual < archivos[i].elementos_leidos) {
                
                int64_t valor_actual = archivos[i].buffer[archivos[i].pos_actual];
                if (min_indice == -1 || valor_actual < min_valor) {
                    min_valor = valor_actual;
                    min_indice = i;
                }
            }
        }
        
        // Si no encontramos un valor mínimo, todos los archivos han sido procesados
        if (min_indice == -1) break;
        
        // Agregar el valor mínimo al buffer de salida
        buffer_salida[pos_buffer_salida++] = min_valor;
        
        // Incrementar la posición en el archivo del valor mínimo
        archivos[min_indice].pos_actual++;
        
        // Si agotamos el buffer de este archivo, cargar un nuevo bloque
        if (archivos[min_indice].pos_actual >= archivos[min_indice].elementos_leidos) {
            archivos[min_indice].elementos_leidos = fread(
                archivos[min_indice].buffer.data(), 
                sizeof(int64_t), 
                elementos_por_bloque, 
                archivos[min_indice].archivo
            );
            contadorIO++; // Contar operación I/O
            
            archivos[min_indice].pos_actual = 0;
            
            // Verificar si hemos llegado al fin del archivo
            if (archivos[min_indice].elementos_leidos == 0) {
                archivos[min_indice].fin_archivo = true;
            }
        }
        
        // Si el buffer de salida está lleno, escribirlo al archivo
        if (pos_buffer_salida == elementos_por_bloque) {
            fwrite(buffer_salida.data(), sizeof(int64_t), elementos_por_bloque, salida);
            contadorIO++; // Contar operación I/O
            pos_buffer_salida = 0;
        }
    }
    
    // Escribir cualquier dato restante en el buffer de salida (tamaño menor a B)
    if (pos_buffer_salida > 0) {
        fwrite(buffer_salida.data(), sizeof(int64_t), pos_buffer_salida, salida);
        contadorIO++; // Contar operación I/O
    }
    
    // Cerrar todos los archivos
    fclose(salida);
    for (auto& archivo : archivos) {
        fclose(archivo.archivo);
    }
}

/**
 * Estructura para manejar fragmentos de archivos en el proceso iterativo
 */
struct FragmentoArchivo {
    std::string nombre;
    size_t inicio;
    size_t fin;
};

/**
 * Implementación iterativa del algoritmo MergeSort externo. Se encarga de usar las funciones auxiliares para hacer divisiones del archivo y la posterior mezcla o fusion
 * en los archivos finales
 * @param archivo_entrada Nombre del archivo a ordenar
 * @param archivo_salida Nombre del archivo de salida ordenado
 * @param N Tamaño del archivo en bytes
 */
void MergesortExterno::mergesort(const std::string& archivo_entrada, const std::string& archivo_salida, size_t N) {
    // Verificar que el archivo existe y obtener su tamaño real si no se especificó
    FILE* archivo = fopen(archivo_entrada.c_str(), "rb");
    if (!archivo) {
        std::cerr << "Error: No se pudo abrir el archivo de entrada" << std::endl;
        return;
    }
    fclose(archivo);
    
    // Calcular el número real de elementos (int64_t) en el archivo
    size_t num_elementos = N / sizeof(int64_t);
    
    // Cola para almacenar los fragmentos del archivo que debemos procesar
    std::queue<FragmentoArchivo> fragmentos_por_procesar;
    
    // Iniciar con el archivo completo
    fragmentos_por_procesar.push({archivo_entrada, 0, num_elementos});
    
    // Contador para generar nombres únicos para archivos temporales
    int contador_temp = 0;
    
    // Cola para almacenar los archivos ya procesados (ordenados)
    std::queue<std::string> archivos_ordenados;
    
    // Procesar los fragmentos iterativamente
    while (!fragmentos_por_procesar.empty()) {
        FragmentoArchivo fragmento_actual = fragmentos_por_procesar.front();
        fragmentos_por_procesar.pop();
        
        std::string archivo_actual = fragmento_actual.nombre;
        size_t inicio = fragmento_actual.inicio;
        size_t fin = fragmento_actual.fin;
        size_t num_elementos_fragmento = fin - inicio;
        
        // Caso base: si el fragmento cabe en memoria, lo ordenamos directamente
        if (num_elementos_fragmento * sizeof(int64_t) <= M) {
            std::string archivo_ordenado = archivo_salida + ".sorted_" + std::to_string(contador_temp++);
            ordenarEnMemoria(archivo_actual, archivo_ordenado, inicio, fin);
            archivos_ordenados.push(archivo_ordenado);
            
            // Si el archivo actual es temporal (no es el original), lo eliminamos
            if (archivo_actual != archivo_entrada) {
                remove(archivo_actual.c_str());
            }
        } else {
            // Dividir el fragmento en subfragmentos según la aridad
            std::string base_temporal = archivo_salida + ".temp_" + std::to_string(contador_temp++);
            std::vector<std::string> nombres_temp = dividirArchivo(archivo_actual, base_temporal, inicio, fin);
            
            // Si el archivo actual es temporal (no es el original), lo eliminamos
            if (archivo_actual != archivo_entrada) {
                remove(archivo_actual.c_str());
            }
            
            // Calcular elementos por subfragmento
            size_t elementos_por_subfragmento = (num_elementos_fragmento + a - 1) / a;
            
            // Añadir cada subfragmento a la cola para procesarlo
            for (size_t i = 0; i < nombres_temp.size(); i++) {
                size_t inicio_subfragmento = 0;
                size_t fin_subfragmento;
                
                // Para el último subfragmento, verificar su tamaño real
                if (i == nombres_temp.size() - 1) {
                    FILE* temp_file = fopen(nombres_temp[i].c_str(), "rb");
                    if (temp_file) {
                        fseek(temp_file, 0, SEEK_END);
                        fin_subfragmento = ftell(temp_file) / sizeof(int64_t);
                        fclose(temp_file);
                    } else {
                        fin_subfragmento = elementos_por_subfragmento;
                    }
                } else {
                    fin_subfragmento = elementos_por_subfragmento;
                }
                
                fragmentos_por_procesar.push({nombres_temp[i], inicio_subfragmento, fin_subfragmento});
            }
        }
    }
    
    // Ahora tenemos una cola de archivos ordenados, los mezclamos de a pares hasta que quede uno solo
    while (archivos_ordenados.size() > 1) {
        std::vector<std::string> grupo_fusion;
        
        // Tomamos hasta 'a' archivos para mezclarlos
        for (size_t i = 0; i < a && !archivos_ordenados.empty(); i++) {
            grupo_fusion.push_back(archivos_ordenados.front());
            archivos_ordenados.pop();
        }
        
        // Nombre del archivo resultante de la fusión
        std::string archivo_fusionado = archivo_salida + ".merged_" + std::to_string(contador_temp++);
        
        // Mezclar(Fusionar) los archivos
        mergeArchivos(grupo_fusion, archivo_fusionado);
        
        // Añadir el archivo fusionado a la cola
        archivos_ordenados.push(archivo_fusionado);
        
        // Eliminar los archivos ya fusionados (o mezclados)
        for (const auto& nombre : grupo_fusion) {
            remove(nombre.c_str());
        }
    }
    
    // Al final solo queda un archivo ordenado, lo renombramos al nombre de salida deseado
    if (!archivos_ordenados.empty()) {
        std::string ultimo_archivo = archivos_ordenados.front();
        
        FILE* src = fopen(ultimo_archivo.c_str(), "rb");
        FILE* dst = fopen(archivo_salida.c_str(), "wb");
        
        if (src && dst) {
            // Copiar contenido
            size_t elementos_por_bloque = B / sizeof(int64_t);
            while (true) {
                size_t leidos = fread(buffer, sizeof(int64_t), elementos_por_bloque, src);
                if (leidos == 0) break;
                
                fwrite(buffer, sizeof(int64_t), leidos, dst);
                contadorIO += 2; // Contar operaciones I/O (lectura y escritura)
            }
        }
        
        if (src) fclose(src);
        if (dst) fclose(dst);
        
        // Eliminar el último archivo temporal
        remove(ultimo_archivo.c_str());
    }
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
 * limpia el buffer de la estructura de datos
 */
void MergesortExterno::limpiarBuffer(){
    if (buffer != nullptr) {
        size_t tamano_buffer = B / sizeof(int64_t);
        std::fill(buffer, buffer + tamano_buffer, 0);
    }
}