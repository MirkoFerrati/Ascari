#ifndef NODE_H
#define NODE_H

#include <string>
#include <fstream>
#include <ostream>
class node
{
  
int x,y;
std::string name;
std::string label;
    void readGraphics(std::fstream* arg1);
    void readTextGraphics(std::fstream* arg1);

public:
    node();
    virtual ~node();
  
    
    friend std::ostream &operator<<( std::ostream &out, const node &n );
    void readNode(std::fstream *arg1);
    
};

#endif // NODE_H
