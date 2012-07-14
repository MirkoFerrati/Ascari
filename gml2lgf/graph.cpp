#include "graph.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>
#include "utils.h"


graph::graph()
{

}


void graph::readGraph(fstream* f)
{
  int numArc=0;
  char s[256];
  f->getline(s,256);
  string tmp=s;
  if (tmp.compare("[")==0) //deve esserci subito una quadra
  {
    while(!f->eof()) //leggiamo i nodi e gli archi
    {
      f->getline(s,256);
      tmp=s;
      tmp.erase(remove(tmp.begin(), tmp.end(), ' '),tmp.end());
      tmp.erase(remove(tmp.begin(), tmp.end(), '\t'),tmp.end());
      if (tmp.compare("node")==0)
      {
	 node n;
	 n.readNode(f);
	 nodes.push_back(n);
	 continue;
      }
      if (tmp.compare("edge")==0)
      {
	arc a;
	a.readArc(f);
	stringstream s;
	s<<numArc;
	numArc++;
	a.name=s.str();
	arcs.push_back(a);
	continue;
      }
      if (tmp.find("[")==0)
      {
	utils::skipSection(f);
	continue;
      }
      if (tmp.find("]")==0)
      {
	 break;
      }
    }
  }
  else
  {
  cerr<<"impossibile trovare la [ di apertura del graph"<<endl;
  }
}

ostream &operator<<( ostream &out, const graph &g ){
  out<<"@nodes"<<endl;
  out<<"label\tcoordinates_x\tcoordinates_y"<<endl;
  for (int i=0;i<g.nodes.size();i++)
    out<<g.nodes[i]<<endl;
  out<<"@arcs"<<endl;
  out<<"\t\tlabel\tlength"<<endl;
  for (int i=0;i<g.arcs.size();i++)
    out<<g.arcs[i]<<endl;
  out<<"@attributes"<<endl;
  out<<"source\t"<<endl;
  out<<"target\t"<<endl;
  cout<<"REMEMBER TO SET SOURCE AND TARGET IN THE FILE"<<endl;
}

graph::~graph()
{

}

