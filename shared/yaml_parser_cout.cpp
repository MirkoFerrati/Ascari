
#include "yaml_parser.h"
#include "parserYAML/include/yaml-cpp/node.h"

#include "streams_utils.h"

using namespace std;

//TODO(Simone): bisogna aggiornare il cout con le ultime cose lette dal file yaml (i behavior)

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
    for (map< controller_name, controller_MapRules >::const_iterator iter=ag.behavior->controllers.begin(); iter!=ag.behavior->controllers.end();++iter)
    {
        i++;
        os<< "Controller "<<i<<endl;
        os<< "Name:"<<((*iter).first)<<endl;
        os<< "Values for Command: "<<endl;

        for (unsigned int j=0; j<(*iter).second.size();j++)
        {
            os<< ag.behavior->inputs[j]<<": "<<((*iter).second.at(j))<<endl;

        }
    }
    os<< "Discrete States: "<<endl;
    for (map< discreteState_Name, controller_name >::const_iterator iter=ag.behavior->discrete_states.begin(); iter!=ag.behavior->discrete_states.end();++iter)
    {
        os<< "Name:"<<((*iter).first)<<endl;
        os<< "Controller: "<<((*iter).second)<<endl;
    }

    os<< "Topologies: "<<endl;
    for (map< topology_name, topology_expression >::const_iterator iter=ag.behavior->topology_expressions.begin(); iter!=ag.behavior->topology_expressions.end();++iter)
    {
        os<< "Name:"<<((*iter).first)<<endl;
        os<< "Expression: "<<((*iter).second)<<endl;
    }

    os<< "Lambda: "<<endl;
    for (map< lambda_name, lambda_expression >::const_iterator iter=ag.behavior->lambda_expressions.begin(); iter!=ag.behavior->lambda_expressions.end();++iter)
    {
        os<< "Name:"<<((*iter).first)<<endl;
        os<< "Expression: "<<((*iter).second)<<endl;
    }

    os<< "Events: "<<endl;
    for (map< event_name, event_expression >::const_iterator iter=ag.behavior->events_expressions.begin(); iter!=ag.behavior->events_expressions.end();++iter)
    {
        os<< "Event:"<<((*iter).first)<<endl;
        os<< "Expression: "<<((*iter).second)<<endl;
    }

    os<< "Automaton: "<< ag.behavior->name<<endl;
    for (map< discreteState_Name, map< event_name, discreteState_Name > >::const_iterator iter=ag.behavior->automaton.begin(); iter!=ag.behavior->automaton.end();++iter)
    {

        string actual_state=((*iter).first);

        os<< "Actual State:"<<actual_state<<endl;


        for (map< event_name, discreteState_Name >::const_iterator iiter=ag.behavior->automaton.at(actual_state).begin(); iiter!=ag.behavior->automaton.at(actual_state).end();++iiter)
        {
            os<< "Event:"<<((*iiter).first)<<"->"<<((*iiter).second)<<endl;

        }
    }

    os<< "Target List: TODO";
//     for (std::vector<target_id>::const_iterator iter=ag.target_list.begin(); iter!=ag.target_list.end();++iter)
//     {
//         os<< " "<<*iter;
//     }
    os<<endl;

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

