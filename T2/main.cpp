#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include "./union_find/UnionFind.hpp"
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
        
        // Importante: actualizar el puntero parent después de que el nodo esté en su posición final
        seq[i].parent = &seq[i];
    }
    
    cout << "Secuencia generada! " << endl;
    return seq;
}

// Cambiar parámetro de vector<nodo>* a vector<nodo>&
vector<arista> generate_aristas_arreglo(int N, vector<nodo>& nodos){
    cout << endl;
    cout << "Calculando arreglo de aristas para N = " << N << ": " << endl;
   
    vector<arista> aristas;
   
    for(int i = 0; i < N; i++){
        for(int j = i + 1; j < N; j++){
            aristas.emplace_back(&nodos[i], &nodos[j]);  // Ahora funciona correctamente
        }
    }
   
    cout << "Aristas calculadas! " << endl;
   
    sort(aristas.begin(), aristas.end(), [](const arista& a, const arista& b) {
        return a.peso < b.peso;
    });
   
    return aristas;
}

// Cambiar parámetro de vector<nodo>* a vector<nodo>&
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

vector<arista> kruskal_arreglo(int N, vector<arista> aristas, int optimizacion){
    vector<arista> bosque;
    UnionFind uf;

    int aristas_agregadas = 0;

    for(const auto& arista : aristas) { 
        nodo* root_u;
        nodo* root_v;

        if(optimizacion){
            root_u = uf.find_optimized(arista.u);
            root_v = uf.find_optimized(arista.v);
        } else {
            root_u = uf.find(arista.u);
            root_v = uf.find(arista.v);
        }

        if(root_u != root_v){
            if(optimizacion){
                uf.union_sets_optimized(arista.u, arista.v);
            } else {
                uf.union_sets(arista.u, arista.v);
            }

            bosque.push_back(arista);
            aristas_agregadas++;

            if(aristas_agregadas == N - 1){
                break;
            }
        }
    }

    cout << "Kruskal arreglo finalizado: N = " << N << " opt = " << optimizacion << endl; 
    return bosque;
}

vector<arista> kruskal_heap(int N, vector<arista> aristas, int optimizacion){
    vector<arista> bosque;
    UnionFind uf;
    int aristas_agregadas = 0;

    while(!aristas.empty()) {
        arista current_arista = aristas.front();
        pop_heap(aristas.begin(), aristas.end(), [](const arista& a, const arista& b) {
            return a.peso > b.peso;  // Usar el mismo comparador que en make_heap
        });
        aristas.pop_back();

        nodo* root_u;
        nodo* root_v;

        if(optimizacion){
            root_u = uf.find_optimized(current_arista.u);
            root_v = uf.find_optimized(current_arista.v);
        } else {
            root_u = uf.find(current_arista.u);
            root_v = uf.find(current_arista.v);
        }

        if(root_u != root_v){
            if(optimizacion){
                uf.union_sets_optimized(current_arista.u, current_arista.v);
            } else {
                uf.union_sets(current_arista.u, current_arista.v);
            }

            bosque.push_back(current_arista);
            aristas_agregadas++;

            if(aristas_agregadas == N - 1){
                break;
            }
        }
    }

    cout << "Kruskal Heap finalizado: N = " << N << " opt = " << optimizacion << endl; 
    return bosque;
}

int main(){
    // Vector de valores para N
    vector<int> N = {32, 64, 128, 256, 512, 1024, 2048, 4096};

    for (int i = 0; i < N.size(); i++){
        int veces = 0;
        while (veces < 4){
            vector<nodo> nodos_sin_opt_arreglo = generate_seq(N[i]);
            vector<nodo> nodo_sin_opt_heap = nodos_sin_opt_arreglo;
            vector<nodo> nodos_opt_arreglo = nodos_sin_opt_arreglo;
            vector<nodo> nodos_opt_heap = nodos_sin_opt_arreglo;                
            
            // Pasar por referencia en lugar de por puntero
            vector<arista> aristas_posibles_sin_opt_arreglo = generate_aristas_arreglo(N[i], nodos_sin_opt_arreglo);
            vector<arista> aristas_posibles_sin_opt_heap = generate_aristas_heap(N[i], nodo_sin_opt_heap);
            vector<arista> aristas_posibles_opt_arreglo = generate_aristas_arreglo(N[i], nodos_opt_arreglo);
            vector<arista> aristas_posibles_opt_heap = generate_aristas_heap(N[i], nodos_opt_heap);

            kruskal_arreglo(N[i], aristas_posibles_sin_opt_arreglo, false);
            kruskal_heap(N[i], aristas_posibles_sin_opt_heap, false);

            // Ahora ejecutamos las versiones con optimizacion
            kruskal_arreglo(N[i], aristas_posibles_opt_arreglo, true);
            kruskal_heap(N[i], aristas_posibles_opt_heap, true);
            
            veces++; // No olvides incrementar el contador
        }
    }
    
    // Vector de tiempos
    vector<double> time_avg;

    return 0;
}