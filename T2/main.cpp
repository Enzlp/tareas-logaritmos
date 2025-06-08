#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include "./structs/estructuras.hpp"
using namespace std;

vector<nodo> generate_seq(int N){
    cout << endl;
    cout << "Generando secuencias para N = " << N << ": " << endl;
    
    // Generador random
    random_device rd;
    mt19937_64 gen(rd());
    uniform_real_distribution<double> dist(0.0, 1.0);
    
    vector<nodo> seq;
    seq.reserve(N); // Reservar espacio para eficiencia
    
    for(int i = 0; i < N; i++){
        double x = dist(gen);
        double y = dist(gen);
        
        // Usar constructor con parámetros
        seq.emplace_back(x, y);
        
        // Importante: actualizar el puntero parent después de que el nodo esté en su posición final
        seq[i].parent = &seq[i];
    }
    
    cout << "Secuencia generada! " << endl;
    return seq;
}

vector<arista> generate_aristas_arreglo(int N, vector<nodo>& nodos){
    cout << endl;
    cout << "Calculando arreglo de aristas para N = " << N << ": " << endl;
    
    // Creamos arreglo de aristas
    vector<arista> aristas;
    aristas.reserve(N * (N - 1) / 2); // Reservar espacio para N(N-1)/2 aristas
    
    for(int i = 0; i < N; i++){
        for(int j = i + 1; j < N; j++){
            // Usar el constructor de arista que calcula automáticamente el peso
            aristas.emplace_back(&nodos[i], &nodos[j]);
        }
    }
    
    cout << "Aristas calculadas! " << endl;
    
    // Ordenamos por peso de menor a mayor usando sort
    sort(aristas.begin(), aristas.end(), [](const arista& a, const arista& b) {
        return a.peso < b.peso;
    });
    
    return aristas;
}

vector<arista> generate_aristas_heap(int N, vector<nodo>& nodos){
    cout << endl;
    cout << "Calculando heap de aristas para N = " << N << ": " << endl;
    
    // Creamos arreglo de aristas
    vector<arista> aristas;
    aristas.reserve(N * (N - 1) / 2); // Reservar espacio para N(N-1)/2 aristas
    
    for(int i = 0; i < N; i++){
        for(int j = i + 1; j < N; j++){
            // Usar el constructor de arista que calcula automáticamente el peso
            aristas.emplace_back(&nodos[i], &nodos[j]);
        }
    }
    
    // Convertimos a heap min
    make_heap(aristas.begin(), aristas.end(), [](const arista& a, const arista& b) {
        return a.peso > b.peso;
    });
    
    cout << "Aristas calculadas! " << endl;
    return aristas;
}

int main(){
	// Vector de valores para N
	vector<int> N = {32, 64, 128, 256, 512, 1024, 2048, 4096};

	// Vector de tiempos
	vector<double> time_avg;

}