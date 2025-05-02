#include "input_generator.h"
#include <cstdint>
#include <limits>
#include <random>
#include <vector>

using namespace std;

void generate_binary_file(std::string filename, size_t memory_size, size_t memory_size_multiplier){
    cout << endl;
    cout << "Generando binario: " << endl;

    ofstream file(filename, ios::binary);
    
    // Descomentar esta linea y comentar la siguiente para establecer una seed fija.
    
    //mt19937_64 rng(42); // Semilla fija
    mt19937_64 rng(random_device{}());

    uniform_int_distribution<uint64_t> dist(0, numeric_limits<uint64_t>::max());
    
    uint64_t primer_elem = 0;
    uint64_t ultimo_elem = 0;

    // Usamos un buffer de tamaño M/2
    size_t buffer_elements = (memory_size / sizeof(uint64_t)) / 2;
  
    if (!file.is_open()) {
        cerr << "Error creating file: " << filename << endl;
        return;
    }

    cout << "Elementos por buffer: " << buffer_elements << endl; 
    cout << "Elementos totales: " << memory_size_multiplier * memory_size << endl;

    vector<uint64_t> buffer_vector(buffer_elements);
    
    for (size_t i = 0; i < 2 * memory_size_multiplier; i++){
        
        for (size_t j = 0; j < buffer_elements; j++){
            uint64_t value = dist(rng);
            buffer_vector[j] = value;

            if (i == 0 && j == 0){
                primer_elem = value;
            }

            if ((i == 2*memory_size_multiplier - 1) && (j == buffer_elements - 1)){
                ultimo_elem = value;
            }

        }

        file.write(reinterpret_cast<const char*>(buffer_vector.data()), buffer_elements * sizeof(int64_t));
    }

    file.close();

    cout << "Primer elemento: " << primer_elem << endl;
    cout << "Ultimo elemento: " << ultimo_elem << endl;
}

void read_binary_file(std::string filename, size_t memory_size, size_t memory_size_multiplier){

}