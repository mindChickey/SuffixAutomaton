
#include <cctype>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"

struct Node {
  char transit;
  unsigned len;
  struct Node* parent;
  List<Node*>* edges;
};

using PNode = Node*;
using PEdge = List<PNode>*;

struct Sem {
  Array<Node> nodes;
  Array<List<PNode>> edges;
  PNode lastRealNode;
};

unsigned getNodeIndex(Sem* sem, PNode node){
  return node - sem->nodes.mem;
}

PEdge transfer(PNode srcNode, char c){
  for(auto e = srcNode->edges; e; e = e->next){
    if(e->data->transit == c) return e;
  }
  return nullptr;
}

PNode insertRightPath(Sem* sem, PNode p, PNode node, char c){
  while(p != nullptr && transfer(p, c) == nullptr){
    p->edges = cons(&sem->edges, node, p->edges);
    printf("insert [%u][%c] = %u\n", getNodeIndex(sem, p), c, getNodeIndex(sem, node));
    p = p->parent;
  }
  return p;
}

PNode replaceRightPath(Sem* sem, PNode p, PNode e, PNode node, char c){
  PEdge edge = transfer(p, c);
  while(edge->data == e){
    edge->data = node;
    printf("replace [%u][%c] = %u\n", getNodeIndex(sem, p), c, getNodeIndex(sem, node));
    p = p->parent;
    if(p == nullptr) return nullptr;
    edge = transfer(p, c);
  }
  return p;
}

void insertClone(Sem* sem, PNode p, PNode q, PNode newNode, char c){
  PNode extraNode = alloc(&sem->nodes);

  extraNode->parent = q->parent;
  q->parent = extraNode;
  newNode->parent = extraNode;

  extraNode->len = p->len + 1;
  extraNode->edges = copyList(&sem->edges, q->edges);
  printf("copy %u <- %u\n", getNodeIndex(sem, extraNode), getNodeIndex(sem, q));

  replaceRightPath(sem, p, q, extraNode, c);
}

void insert(Sem* sem, char c){
  PNode newNode = alloc(&sem->nodes);
  PNode p = sem->lastRealNode;
  sem->lastRealNode = newNode;

  newNode->transit = c;
  newNode->edges = nullptr;
  newNode->len = p->len+1;

  p = insertRightPath(sem, p, newNode, c);

  if(p == nullptr){
    newNode->parent = sem->nodes.mem;
    return;
  }
  PNode q = transfer(p, c)->data;
  if(p->len + 1 == q->len){
    newNode->parent = q;
  } else {
    insertClone(sem, p, q, newNode, c);
  }
}

Sem newSem(unsigned length){
  PNode nodeMem = (PNode)malloc(2 * length * sizeof(Node));
  PEdge edgeMem = (PEdge)malloc(3 * length * sizeof(List<PNode>));

  Sem sem = { {nodeMem, 0}, {edgeMem, 0}, nodeMem };
  PNode star = alloc(&sem.nodes);
  star->edges = nullptr;
  star->len = 0;
  star->parent = nullptr;
  return sem;
}

Sem build(char* s){
  unsigned length = strlen(s);
  Sem sem = newSem(length);
 
  for(unsigned i=0;i<length;i++){
    printf("insert %c\n", s[i]);
    insert(&sem, s[i]);
    puts("\0");
  }
  return sem;
}

void querySubString(PNode state, char* str, unsigned len, bool ignoreLower, bool ignoreUpper){
  if(len == 0) printf("succ %p\n", state);

  char c = str[0];

  PEdge edge = transfer(state, c);
  if(edge) querySubString(edge->data, str+1, len-1, ignoreLower, ignoreUpper);

  if(islower(c) && ignoreLower){
    PEdge edge = transfer(state, c-'a'+'A');
    if(edge) querySubString(edge->data, str+1, len-1, ignoreLower, ignoreUpper);
  } 

  if(isupper(c) && ignoreUpper) {
    PEdge edge = transfer(state, c-'A'+'a');
    if(edge) querySubString(edge->data, str+1, len-1, ignoreLower, ignoreUpper);
  }
}

int main(){
  char str[] = "abaabAa";
  Sem sem = build(str);
  querySubString(&sem.nodes.mem[0], "aba", 3, true, true);
  return 0;
}
