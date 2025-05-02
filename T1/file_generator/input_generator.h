#ifndef FILE_GENERATOR_H
#define FILE_GENERATOR_H

#include <cstddef>
#include <fstream>
#include <string>
#include <iostream>
#include <vector>
#include <random>

void generate_binary_file(std::string filename, size_t memory_size, size_t memory_size_multiplier);

void read_binary_file(std::string filename, size_t memory_size, size_t memory_size_multiplier);

#endif