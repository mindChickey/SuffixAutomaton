
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sam.h"

unsigned getNodeIndex(Sam* sam, PNode node){
  return node - sam->nodes.mem;
}

PEdge transfer(PNode srcNode, char c){
  for(auto e = srcNode->edges; e; e = e->next){
    if(e->data->transit == c) return e;
  }
  return nullptr;
}

PNode insertRightPath(Sam* sam, PNode p, PNode node, char c){
  while(p != nullptr && transfer(p, c) == nullptr){
    p->edges = cons(&sam->edges, node, p->edges);
    printf("insert [%u][%c] = %u\n", getNodeIndex(sam, p), c, getNodeIndex(sam, node));
    p = p->parent;
  }
  return p;
}

PNode replaceRightPath(Sam* sam, PNode p, PNode e, PNode node, char c){
  PEdge edge = transfer(p, c);
  while(edge->data == e){
    edge->data = node;
    printf("replace [%u][%c] = %u\n", getNodeIndex(sam, p), c, getNodeIndex(sam, node));
    p = p->parent;
    if(p == nullptr) return nullptr;
    edge = transfer(p, c);
  }
  return p;
}

void insertClone(Sam* sam, PNode p, PNode q, PNode newNode, char c){
  PNode extraNode = alloc(&sam->nodes);

  extraNode->parent = q->parent;
  q->parent = extraNode;
  newNode->parent = extraNode;

  extraNode->transit = q->transit;
  extraNode->len = p->len + 1;
  extraNode->edges = copyList(&sam->edges, q->edges);
  printf("copy %u <- %u\n", getNodeIndex(sam, extraNode), getNodeIndex(sam, q));

  replaceRightPath(sam, p, q, extraNode, c);
}

void insert(Sam* sam, char c){
  PNode newNode = alloc(&sam->nodes);
  PNode p = sam->lastRealNode;
  sam->lastRealNode = newNode;

  newNode->transit = c;
  newNode->edges = nullptr;
  newNode->len = p->len+1;

  p = insertRightPath(sam, p, newNode, c);

  if(p == nullptr){
    newNode->parent = sam->nodes.mem;
    return;
  }
  PNode q = transfer(p, c)->data;
  if(p->len + 1 == q->len){
    newNode->parent = q;
  } else {
    insertClone(sam, p, q, newNode, c);
  }
}

Sam newSam(unsigned length){
  PNode nodeMem = (PNode)malloc(2 * length * sizeof(Node));
  PEdge edgeMem = (PEdge)malloc(3 * length * sizeof(List<PNode>));

  Sam sam = { {nodeMem, 0}, {edgeMem, 0}, nodeMem };
  PNode star = alloc(&sam.nodes);
  star->edges = nullptr;
  star->len = 0;
  star->parent = nullptr;
  return sam;
}

Sam buildSam(char* s){
  unsigned length = strlen(s);
  Sam sam = newSam(length);
 
  for(unsigned i=0;i<length;i++){
    printf("insert %c\n", s[i]);
    insert(&sam, s[i]);
    puts("\0");
  }
  return sam;
}
