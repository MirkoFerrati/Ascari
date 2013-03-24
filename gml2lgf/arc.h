#ifndef ARC_H
#define ARC_H

#include <string>
#include "node.h"
#include <ostream>
#include <fstream>

class arc
{

  
  int first;
  int second;
  double len;
  void readLabelGraphics(std::fstream* arg1);
  
  
public:
  std::string name;
    arc();
    
    virtual ~arc();
    
    friend std::ostream &operator<<( std::ostream &out, const arc &a );
    void readArc(std::fstream* arg1);

};

#endif // ARC_H
