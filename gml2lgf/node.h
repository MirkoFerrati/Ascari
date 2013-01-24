#ifndef NODE_H
#define NODE_H

#include <string>

using namespace std;

class node
{
  
int x,y;
string name;
string label;
    void readGraphics(fstream* arg1);
    void readTextGraphics(fstream* arg1);

public:
    node();
    virtual ~node();
  
    
    friend ostream &operator<<( ostream &out, const node &n );
    void readNode(fstream *arg1);
    
};

#endif // NODE_H
