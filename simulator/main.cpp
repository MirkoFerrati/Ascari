#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "logog.hpp"

#include "yaml-cpp/yaml.h"

using namespace std;

struct Agent {
    std::string name;
	vector<string> states;
	map<string,string> expressions;

};

void operator >> (const YAML::Node& node, Agent& ag)
{
		node["agent"]>>ag.name;
		const YAML::Node& dynamics = node["DYNAMICS"][0];
   //for(unsigned i=0;i<dynamics.size();i++) {
      dynamics["states"]>>ag.states;
	  string temp;
	  for (int i=0;i<ag.states.size();i++)
	  {
		  dynamics[ag.states[i]]>>temp;
		  ag.expressions.insert(std::make_pair<string,string>(ag.states[i],temp));
		  //ag.states[i]>>expression.variable;
		  //dynamics[ag.states[i]]>>expression.value;
		  //ag.expressions.push_back(expression);
	  }
		  //ag.expressions.push_back(expression);
   
}




int main(int argc, char **argv) {
    LOGOG_INITIALIZE();
    {
        logog::Cout out;
        WARN("Foo is over %d!  Current value is %d.", 3, 5 );
        std::cout << "Hello, world! simulator" << std::endl;
        std::ifstream fin("monsters.yaml");
        YAML::Parser parser(fin);
        YAML::Node doc;
        parser.GetNextDocument(doc);
        for (unsigned i=0;i<doc.size();i++) {
            Agent ag;
            doc[i] >> ag;
			std::cout << ag.name << "\n";
			for (int i =0 ;i<ag.states.size();i++)
			{
				cout<<ag.states[i]<<endl;
				cout<<ag.expressions[ag.states[i]]<<endl;
			}
        }
    }
    LOGOG_SHUTDOWN();
    return 0;
}
