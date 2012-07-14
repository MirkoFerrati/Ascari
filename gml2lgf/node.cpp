#include "node.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <sstream>
#include "utils.h"

node::node()
{

}


void node::readGraphics(fstream* f)
{
  char s[256];
  f->getline(s,256);
  string tmp;
  tmp=s;
  tmp.erase(remove(tmp.begin(), tmp.end(), ' '),tmp.end());
  tmp.erase(remove(tmp.begin(), tmp.end(), '\t'),tmp.end());
  if (tmp.compare("[")==0) //deve esserci subito una quadra
  {
    while(!f->eof()) //leggiamo le proprietà del nodo
    {
      f->getline(s,256);
      tmp=s;
      tmp.erase(remove(tmp.begin(), tmp.end(), ' '),tmp.end());
      tmp.erase(remove(tmp.begin(), tmp.end(), '\t'),tmp.end());
      if (tmp.find("x")==0)
      {
	stringstream in;
	in<<tmp.substr(1,tmp.length()-1);
	in>>x;
// 	cout<<"coordinata x del nodo: "<<x<<endl;
	continue;
      }
      if (tmp.find("y")==0)
      {
	stringstream in;
	in<<tmp.substr(1,tmp.length()-1);
	in>>y;
// 	cout<<"coordinata y del nodo: "<<y<<endl;
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
    cerr<<"attenzione, non trovata parentesi di apertura della sezione graphics"<<endl;
}



void node::readNode(fstream* f)
{
  char s[256];
  f->getline(s,256);
  string tmp=s;
  int par=1;
  tmp.erase(remove(tmp.begin(), tmp.end(), ' '),tmp.end());
  tmp.erase(remove(tmp.begin(), tmp.end(), '\t'),tmp.end());
  if (tmp.compare("[")==0) //deve esserci subito una quadra
  {
    while(!f->eof()) //leggiamo le proprietà del nodo
    {
      f->getline(s,256);
      tmp=s;
      tmp.erase(remove(tmp.begin(), tmp.end(), ' '),tmp.end());
      tmp.erase(remove(tmp.begin(), tmp.end(), '\t'),tmp.end());

      if (tmp.find("id")==0)
      {
	name=tmp.substr(2,tmp.length()-2);
// 	cout<<"identificativo del nodo: "<<name<<endl;
	continue;
      }
      if (tmp.compare("graphics")==0)
      {
	readGraphics(f);
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
    cerr<<"attenzione, impossibile trovare la quadra di apertura del nodo"<<endl;
}


ostream &operator<<( ostream &out, const node &n ){
  out<<n.name<<"\t"<<n.x/2<<"\t"<<n.y/2;
}

node::~node()
{

}


