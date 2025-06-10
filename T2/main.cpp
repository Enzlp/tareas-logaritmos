#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <ctime> 
#include <chrono>
#include <fstream>
#include <tuple>
#include "./union_find/UnionFind.hpp"
using namespace std::chrono;
using namespace std;

void generate_seq(int N, vector<nodo>& seq){
    cout << endl;
    cout << "Generando secuencias para N = " << N << ": " << endl;
    
    // Limpiar el vector por si acaso
    seq.clear();
    seq.reserve(N); // Reservar espacio para evitar relocaciones
    
    // Generador random
    random_device rd;
    mt19937_64 gen(rd());
    uniform_real_distribution<double> dist(0.0, 1.0);
    
    for(int i = 0; i < N; i++){
        double x = dist(gen);
        double y = dist(gen);
        
        // Usar constructor con parámetros
        seq.emplace_back(x, y);
    }
    
    // IMPORTANTE: Actualizar los punteros parent después de que todos los nodos estén en su posición final
    for(int i = 0; i < N; i++){
        seq[i].parent = &seq[i];
    }
    
    cout << "Secuencia generada! " << endl;
}

vector<arista> generate_aristas_arreglo(int N, vector<nodo>& nodos){
    cout << endl;
    cout << "Calculando arreglo de aristas para N = " << N << ": " << endl;
   
    vector<arista> aristas;
    // Reservar espacio para evitar relocaciones
    aristas.reserve((N * (N - 1)) / 2);
   
    for(int i = 0; i < N; i++){
        for(int j = i + 1; j < N; j++){
            aristas.emplace_back(&nodos[i], &nodos[j]);
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
    aristas.reserve((N * (N - 1)) / 2);
   
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
    bosque.reserve(N - 1); // Un MST tiene exactamente N-1 aristas
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
    bosque.reserve(N - 1);
    UnionFind uf;
    int aristas_agregadas = 0;

    while(!aristas.empty()) {
        arista current_arista = aristas.front();
        pop_heap(aristas.begin(), aristas.end(), [](const arista& a, const arista& b) {
            return a.peso > b.peso;
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

    // Vector de tiempos
    vector<double> time_avg_arreglo_no_opti;
    vector<double> time_avg_heap_no_opti;
    vector<double> time_avg_arreglo_opti;
    vector<double> time_avg_heap_opti;


    for (int i = 0; i < N.size(); i++){
        int veces = 0;
        vector<double> N_time_avg_arreglo_no_opti;
        vector<double> N_time_avg_heap_no_opti;
        vector<double> N_time_avg_arreglo_opti;
        vector<double> N_time_avg_heap_opti;

        while (veces < 5){
            vector<nodo> nodos_sin_opt_arreglo;
            generate_seq(N[i], nodos_sin_opt_arreglo);    
            vector<nodo> nodo_sin_opt_heap = nodos_sin_opt_arreglo;
            vector<nodo> nodos_opt_arreglo = nodos_sin_opt_arreglo;
            vector<nodo> nodos_opt_heap = nodos_sin_opt_arreglo;                
            
            // Pasar por referencia en lugar de por puntero
            vector<arista> aristas_posibles_sin_opt_arreglo = generate_aristas_arreglo(N[i], nodos_sin_opt_arreglo);
            vector<arista> aristas_posibles_sin_opt_heap = generate_aristas_heap(N[i], nodo_sin_opt_heap);
            vector<arista> aristas_posibles_opt_arreglo = generate_aristas_arreglo(N[i], nodos_opt_arreglo);
            vector<arista> aristas_posibles_opt_heap = generate_aristas_heap(N[i], nodos_opt_heap);

            auto start1 = high_resolution_clock::now();
            kruskal_arreglo(N[i], aristas_posibles_sin_opt_arreglo, false);
            auto end1 = high_resolution_clock::now();
            N_time_avg_arreglo_no_opti.push_back(duration<double>(end1 - start1).count());

            auto start2 = high_resolution_clock::now();
            kruskal_heap(N[i], aristas_posibles_sin_opt_heap, false);
            auto end2 = high_resolution_clock::now();
            N_time_avg_heap_no_opti.push_back(duration<double>(end2 - start2).count());
            

            auto start3 = high_resolution_clock::now();
            kruskal_arreglo(N[i], aristas_posibles_opt_arreglo, true);
            auto end3 = high_resolution_clock::now();
            N_time_avg_arreglo_opti.push_back(duration<double>(end3 - start3).count());
        

            auto start4 = high_resolution_clock::now();
            kruskal_heap(N[i], aristas_posibles_opt_heap, true);
            auto end4 = high_resolution_clock::now();
            N_time_avg_heap_opti.push_back(duration<double>(end4 - start4).count());
            
            veces++; // No olvides incrementar el contador
        }
        // Calcular promedios y agregar al vector final
        auto promedio = [](const vector<double>& tiempos) {
            double suma = 0.0;
            for (double t : tiempos) suma += t;
            return suma / tiempos.size();
        };

        time_avg_arreglo_no_opti.push_back(promedio(N_time_avg_arreglo_no_opti));
        time_avg_heap_no_opti.push_back(promedio(N_time_avg_heap_no_opti));
        time_avg_arreglo_opti.push_back(promedio(N_time_avg_arreglo_opti));
        time_avg_heap_opti.push_back(promedio(N_time_avg_heap_opti));
    }
    
    // Falta exportar los datos a csv para generar los graficos

    return 0;
}