struct nodo{
  double x, y;
  nodo* parent;
  int size;
  // Constructor
  nodo(double x_coord, double y_coord) : x(x_coord), y(y_coord), parent(this), size(1) {}
};

struct arista{
  nodo* u;
  nodo* v;
  double peso; // (x1 - x2)^2 + (y1 - y2)^2
  // Constructor
  arista(nodo* nodo1, nodo* nodo2): u(nodo1), v(nodo2) {
      double dx = nodo1->x - nodo2->x;
      double dy = nodo1->y - nodo2->y;
      peso = dx * dx + dy * dy;
  }
};