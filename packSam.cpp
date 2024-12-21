
#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "sam.h"

Automaton newAutomaton(unsigned nodeSize, unsigned edgeSize){
  AmNode* packNodeMem = (AmNode*)malloc(nodeSize * sizeof(Node));
  unsigned* edgeMem = (unsigned*)malloc(edgeSize * sizeof(unsigned));
  return {{packNodeMem, 0}, {edgeMem, 0}};
}

Array<unsigned> pushEdge(Sam* sam, Array<unsigned>* edgeBuffer, PEdge edge0){
  auto begin = edgeBuffer->mem + edgeBuffer->size;
  for(; edge0; edge0 = edge0->next){
    unsigned* edge1 = alloc(edgeBuffer);
    *edge1 = getNodeIndex(sam, edge0->data);
  }
  auto end = edgeBuffer->mem + edgeBuffer->size;

  auto cmp = [sam](unsigned x, unsigned y){
    return sam->nodes.mem[x].transit < sam->nodes.mem[y].transit;
  };
  std::sort(begin, end, cmp);
  return {begin, unsigned(end - begin)};
}

Automaton packSam(Sam* sam){
  Automaton pack = newAutomaton(sam->nodes.size, sam->edges.size);
  auto edgeBuffer = &pack.edgeBuffer;

  for(unsigned i = 0; i < sam->nodes.size; i++){
    Node node0 = sam->nodes.mem[i];
    auto node1 = alloc(&pack.nodes);
    node1->transit = node0.transit;
    node1->edges = pushEdge(sam, edgeBuffer, node0.edges);
  }
  return pack;
}

void printAutomaton(Automaton* am){
  for(unsigned i = 0; i < am->nodes.size; i++){
    AmNode node = am->nodes.mem[i];
    for(unsigned k = 0; k < node.edges.size; k++){
      unsigned destIndex = node.edges.mem[k];
      AmNode dest = am->nodes.mem[destIndex];
      printf("[%u][%c] -> %u\n", i, dest.transit, destIndex);
    }
  }
}

unsigned transfer(Automaton* am, unsigned state, char c){
  AmNode node = am->nodes.mem[state];
  auto cmp = [am](unsigned x, char c){
    return am->nodes.mem[x].transit < c;
  };
 
  unsigned* last = node.edges.mem + node.edges.size;
  unsigned* r = std::lower_bound(node.edges.mem, last, c, cmp);

  bool eq = r != last && am->nodes.mem[*r].transit == c;
  return eq ? *r : -1u;
}

void querySubString(Automaton* am, unsigned state, char* str, bool ignoreLower, bool ignoreUpper){
  if(state == -1u) return;

  char c = str[0];
  if(c == '\0') {
    printf("succ %u\n", state);
    return;
  }

  if(islower(c) && ignoreLower){
    unsigned state1 = transfer(am, state, c-'a'+'A');
    querySubString(am, state1, str+1, ignoreLower, ignoreUpper);
  } 
  if(isupper(c) && ignoreUpper) {
    unsigned state1 = transfer(am, state, c-'A'+'a');
    querySubString(am, state1, str+1, ignoreLower, ignoreUpper);
  }
  unsigned state1 = transfer(am, state, c);
  querySubString(am, state1, str+1, ignoreLower, ignoreUpper);
}

int main(){
  char str[] = "abaabbAba";
  Sam sam = buildSam(str);
  Automaton pSam = packSam(&sam);

  querySubString(&pSam, 0, "aba", false, true);
  return 0;
}
