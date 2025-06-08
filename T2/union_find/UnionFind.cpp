#include "UnionFind.hpp"

nodo* UnionFind::find(nodo* u){
    nodo* current = u;

    while (current->parent != current) {
        current = current->parent;
    }
    return current;
}

nodo* UnionFind::find_optimized(nodo* u){
	if (u->parent == u) {
        return u;
    }

	u->parent = find_optimized(u->parent);
    return u->parent;
}


void UnionFind::union_sets(nodo* u, nodo* v){
	nodo* u_root = find(u);
	nodo* v_root = find(v);

	if(u_root == v_root){
		return;
	}

	if (u_root->size < v_root->size) {
        u_root->parent = v_root;
        v_root->size += u_root->size;
    } else {
        v_root->parent = u_root;
        u_root->size += v_root->size;
    }
}

void UnionFind::union_sets_optimized(nodo* u, nodo* v) {
	nodo* u_root = find_optimized(u);
	nodo* v_root = find_optimized(v);

    // Si ya están en el mismo conjunto, no hacer nada
    if (u_root == v_root) {
        return;
    }
    
    // Union by size: el árbol más pequeño se convierte en hijo del más grande
    if (u_root->size < v_root->size) {
        u_root->parent = v_root;
        v_root->size += u_root->size;
    } else {
        v_root->parent = u_root;
        u_root->size += v_root->size;
    }
}