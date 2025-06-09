#include "Kruskal.hpp"

vector<arista> kruskal_arreglo(int N, vector<arista>* aristas, bool optimized){
    vector<arista> bosque;
    UnionFind uf;

    int aristas_agregadas = 0;
    
    for(const auto& arista : aristas) { 
        nodo* root_u;
        nodo* root_v;
        
        if(optimized){
            root_u = uf.find_optimized(arista.u);
            root_v = uf.find_optimized(arista.v);
        } else {
            root_u = uf.find(arista.u);
            root_v = uf.find(arista.v);
        }
        
        if(root_u != root_v){
            if(optimized){
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
    return bosque;
}

vector<arista> kruskal_heap(int N, vector<arista>& aristas, bool optimized){
    vector<arista> bosque;
    UnionFind uf;
    int aristas_agregadas = 0;
   
    while(!aristas.empty()) {
        arista current_arista = aristas.front();
        pop_heap(aristas.begin(), aristas.end());
        aristas.pop_back();
        
        nodo* root_u;
        nodo* root_v;
       
        if(optimized){
            root_u = uf.find_optimized(current_arista.u);
            root_v = uf.find_optimized(current_arista.v);
        } else {
            root_u = uf.find(current_arista.u);
            root_v = uf.find(current_arista.v);
        }
       
        if(root_u != root_v){
            if(optimized){
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
    return bosque;
}