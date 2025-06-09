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
    
    for(int i = 0; i < N; i++){
        double x = dist(gen);
        double y = dist(gen);
        
        // Usar constructor con parámetros
        seq.emplace_back(x, y);
        
        seq[i].parent = &seq[i];
    }
    
    cout << "Secuencia generada! " << endl;
    return seq;
}

vector<arista> generate_aristas_arreglo(int N, vector<nodo>& nodos){
    cout << endl;
    cout << "Calculando arreglo de aristas para N = " << N << ": " << endl;
   
    vector<arista> aristas;
   
    for(int i = 0; i < N; i++){
        for(int j = i + 1; j < N; j++){
            aristas.emplace_back(&nodos[i], &nodos[j]);  // Now nodos[i] gives nodo&
        }
    }
   
    cout << "Aristas calculadas! " << endl;
   
    sort(aristas.begin(), aristas.end(), [](const arista& a, const arista& b) {
        return a.peso < b.peso;
    });
   
    return aristas;
}

vector<arista> generate_aristas_heap(int N, vector<nodo>& nodos){
    cout << endl;
    cout << "Calculando heap de aristas para N = " << N << ": " << endl;
   
    vector<arista> aristas;
   
    for(int i = 0; i < N; i++){
        for(int j = i + 1; j < N; j++){
            aristas.emplace_back(&nodos[i], &nodos[j]);
        }
    }
   
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