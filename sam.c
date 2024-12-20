
#include <stdio.h>
#include <string.h>

#define N 10
#define C 2

unsigned ch[N][C];
unsigned parent[N] = {-1u};
unsigned len[N];
unsigned lastNode = 0, nodeNum = 1;

unsigned insertRightPath(unsigned p, unsigned e, unsigned node, unsigned c){
  while(p != -1u && ch[p][c] == e){
    ch[p][c] = node;
    printf("ch[%u][%u] = %u\n", p, c, node);
    p = parent[p];
  }
  return p;
}

void insertClone(unsigned p, unsigned q, unsigned newNode, unsigned c){
  unsigned extraNode = nodeNum; 
  nodeNum++;
  len[extraNode] = len[p] + 1;
  memcpy(ch[extraNode], ch[q], sizeof(ch[q]));
  printf("ch[%u] = [%u, %u]\n", extraNode, ch[extraNode][0], ch[extraNode][1]);

  parent[extraNode] = parent[q];
  parent[q] = parent[newNode] = extraNode;
  insertRightPath(p, q, extraNode, c);
}

void insert(unsigned c){
  unsigned p = lastNode; // right path node
  unsigned newNode = nodeNum; 
  nodeNum++;
  lastNode = newNode;

  len[newNode] = len[p] +1;
  p = insertRightPath(p, 0, newNode, c);

  if(p == -1u){
    parent[newNode] = 0;
    return;
  }
  unsigned q = ch[p][c];
  if(len[p] + 1 == len[q]){
    parent[newNode] = q;
  } else {
    insertClone(p, q, newNode, c);
  }
}

void build(char* s, unsigned length){
  for(unsigned i=0;i<length;i++){
    printf("insert %c\n", s[i]);
    insert(s[i] - 'a');
    puts("\0");
  }
}

void print(){
  for(unsigned i = 0; i < N; i++){
    for(unsigned j = 0; j < C; j++){
      printf("%u %c %u\n", i, 'a'+j, ch[i][j]);
    }
  }
}

int main(){
  char str[] = "abaabb";
  build(str, 6);
  print();
  return 0;
}