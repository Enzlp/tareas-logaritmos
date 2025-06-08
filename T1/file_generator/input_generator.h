#ifndef FILE_GENERATOR_H
#define FILE_GENERATOR_H

#include <cstddef>
#include <fstream>
#include <string>
#include <iostream>
#include <vector>
#include <random>

//Header declarativo de la funcion para crear el archivo binario
void generate_binary_file(std::string filename, size_t memory_size, size_t memory_size_multiplier);

#endif