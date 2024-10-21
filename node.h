#ifndef NODE_H
#define NODE_H

struct node {
  int label;
  int degree;
};

int nodecmp(const void *a, const void *b);

#endif // NODE_H 
