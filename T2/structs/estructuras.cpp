struct nodo{
  double x, y;
};

struct arista{
  nodo* x;
  nodo* y;
  double peso; // (x1 - x2)^2 + (y1 - y2)^2
};