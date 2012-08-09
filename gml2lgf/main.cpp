#include <iostream>
#include <fstream>
#include "graph.h"

using namespace std;

int main(int argc, char **argv) {
  if (true)
  {
      graph g;
      char s[256];
      fstream f;
	  if (argc<2)
	  {
		cout<< "attenzione, inserire il nome del file da convertire"<<endl;
		return 1;
	  }
      string filename=argv[1];
      cout<<filename<<endl;
      f.open(filename.c_str(),ios::in);
      string tmp;
      while (!f.eof())
      {
	f.getline(s,256);
	tmp=s;
	if (tmp.compare("graph")==0) //inizia il testo interessante
	{
	   g.readGraph(&f);
	}
      
      }
      
      f.close();
      
      f.open(filename.append(".lgf").c_str(),ios::out);
      f<<g;
      f.close();
  }
  
  return 0;
}
