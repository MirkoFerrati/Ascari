#ifndef ARC_H
#define ARC_H

#include <string>
#include "node.h"


using namespace std;

class arc
{

  
  int first;
  int second;
  double len;
  void readLabelGraphics(fstream* arg1);
  
  
public:
  string name;
    arc();
    
    virtual ~arc();
    
    friend ostream &operator<<( ostream &out, const arc &a );
    void readArc(fstream* arg1);

};

#endif // ARC_H
