
#include "yaml_parser.h"
#include "parserYAML/include/yaml-cpp/node.h"

#include "streams_utils.h"

using namespace std;


ostream& operator<< (ostream& os, const Parsed_Agent& ag) {

    os << ag.name << "\n";
    for (unsigned int i =0 ;i<ag.behavior->state.size();i++)
    {
        os<< "State:"<<ag.behavior->state.at(i)<<endl;
        os<< "Map: "<<ag.behavior->expressions.at(ag.behavior->state.at(i))<<endl;
        os<< "initial value: "<<ag.initial_states.at(ag.behavior->state.at(i))<<endl;
    }

    os << "Controllers" << "\n";
    int i=0;

    os<< "Visibility:"<<ag.visibility<<endl;
    os<< "Communication:"<<ag.communication<<endl;

    return os;
}




ostream& operator<< (ostream& os, const std::vector<Parsed_Agent>& ag) {
    for (unsigned int i=0; i<ag.size();i++)
    {
        os<<ag.at(i);
        os<<endl;
    }
    return os;
}



ostream& operator<<(std::ostream& os, const Parsed_World& wo)
{
    os<<wo.agents;
    return os;
}

