#ifndef GRAPH_H
#define GRAPH_H

#include "arc.h"
#include "node.h"
#include <vector>
#include <fstream>
#include <ostream>

class graph
{
  
  std::vector<node> nodes;
  std::vector<arc> arcs;


public:
     graph();
     virtual ~graph();

  
    friend std::ostream &operator<<( std::ostream &out, const graph &g );
    void readGraph(std::fstream* arg1);

};

#endif // GRAPH_H
