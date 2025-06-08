#ifndef UNIONFIND_HPP
#define UNIONFIND_HPP
#include "../structs/estructuras.hpp"
#include <vector>
using namespace std;

class UnionFind {
public:
    // Constructor 
    UnionFind(){}
    
    nodo* find(nodo* u);

    nodo* find_optimized(nodo* u);

    void union_sets(nodo* u, nodo* v);
};

#endif