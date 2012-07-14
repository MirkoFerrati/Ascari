#ifndef GRAPH_H
#define GRAPH_H

#include "arc.h"
#include "node.h"
#include <vector>
#include <fstream>

using namespace std;

class graph
{
  
  vector<node> nodes;
  vector<arc> arcs;


public:
     graph();
     virtual ~graph();

  
    friend ostream &operator<<( ostream &out, const graph &g );
    void readGraph(fstream* arg1);

};

#endif // GRAPH_H
