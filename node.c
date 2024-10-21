#include "node.h"

int nodecmp(const void *a, const void *b) {
  struct node *x = (struct node *)a;
  struct node *y = (struct node *)b;
  return y->degree - x->degree;
}
