#ifndef NODE_H
#define NODE_H

struct node {
  int label;
  int degree;
};

int nodecmp(void *a, void *b);

#endif // NODE_H 
