#include "input_generator.h"
#include <iostream>
#include <string>

int main(int argc, char* argv[]){

    // Configuración por defecto
    size_t memory_size_multiplier = 4; // Multiplicador del tamaño de memoria
    size_t B = 512;                    // Tamaño del bloque (512 B por defecto)
    size_t M = 50 * 1024 * 1024;       // 50 MB de memoria principal
    
    if (argc < 2 || argc > 5){
        std::cerr << "Usage:\n"
            << "  " << argv[0] << " <filename> <memory_size_multiplier> <block_size_in_bytes> <memory_size_in_MB>\n";
        return 1;
    }

    // Procesar argumentos
    std::string filename = argv[1];

    if (argc > 2) {
        memory_size_multiplier = std::stoul(argv[2]);
    }

    if (argc > 3) {
        B = std::stoul(argv[3]);
    }
    if (argc > 4) {
        M = std::stoul(argv[4]) * 1024 * 1024;  // Convertir MB a bytes
    }
    
    std::cout << "Configuración:" << std::endl;
    std::cout << "- Nombre de archivo: " << filename << ".bin" << std::endl;
    std::cout << "- Multiplicador del tamaño de memoria: " << memory_size_multiplier << std::endl;
    std::cout << "- Tamaño de bloque (B): " << B << " bytes" << std::endl;
    std::cout << "- Memoria principal (M): " << M / (1024 * 1024) << " MB" << " (" << M << " bytes)" << std::endl;

    generate_binary_file(filename, M, memory_size_multiplier);

    
}