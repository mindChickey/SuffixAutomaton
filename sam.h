#include "utils.h"

struct Node {
  char transit;
  unsigned len;
  struct Node* parent;
  List<Node*>* edges;
};

using PNode = Node*;
using PEdge = List<PNode>*;

struct Sam {
  Array<Node> nodes;
  Array<List<PNode>> edges;
  PNode lastRealNode;
};

unsigned getNodeIndex(Sam* sam, PNode node);
Sam buildSam(char* s);

struct AmNode {
  char transit;
  Array<unsigned> edges;
};

struct Automaton {
  Array<AmNode> nodes;
  Array<unsigned> edgeBuffer;
};

Automaton packSam(Sam* sam);