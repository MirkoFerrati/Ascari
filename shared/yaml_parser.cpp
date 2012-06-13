#include "yaml_parser.h"

using namespace std;

std::vector<Parsed_Agent> parse_file(const char * file_name) {
    std::ifstream fin(file_name);
    std::ostringstream str;
    str<<fin.rdbuf();
    std::string new_str=str.str();
    boost::algorithm::to_upper(new_str);
    std::stringstream new_new_str;
    new_new_str<<new_str;


    YAML::Parser parser(new_new_str);
    YAML::Node doc;
    parser.GetNextDocument(doc);
    std::vector<Parsed_Agent> Agents(doc.size());
    for (unsigned i=0;i<doc.size();i++) {
        doc[i] >> Agents[i];

    }
    return Agents;

}


ostream& operator<< (ostream& os, const Parsed_Agent& ag) {

    os << ag.name << "\n";
    for (unsigned int i =0 ;i<ag.state.size();i++)
    {
        os<< "State:"<<ag.state.at(i)<<endl;
        os<< "Map: "<<ag.expressions.at(ag.state.at(i))<<endl;
        os<< "initial value: "<<ag.initial_states.at(ag.state.at(i))<<endl;
    }

    os << "Controllers" << "\n";
    int i=0;
    for (map< controller_name, controller_MapRules >::const_iterator iter=ag.controllers.begin(); iter!=ag.controllers.end();iter++)
    {
        i++;
        os<< "Controller "<<i<<endl;
        os<< "Name:"<<((*iter).first)<<endl;
        os<< "Values for Command: "<<endl;

        for (unsigned int j=0; j<(*iter).second.size();j++)
        {
            os<< ag.inputs[j]<<": "<<((*iter).second.at(j))<<endl;

        }
    }
    os<< "Discrete States: "<<endl;
    for (map< discreteState_Name, controller_name >::const_iterator iter=ag.discrete_states.begin(); iter!=ag.discrete_states.end();iter++)
    {
        os<< "Name:"<<((*iter).first)<<endl;
        os<< "Controller: "<<((*iter).second)<<endl;
    }

    os<< "Topologies: "<<endl;
    for (map< topology_name, topology_expression >::const_iterator iter=ag.topology_expressions.begin(); iter!=ag.topology_expressions.end();iter++)
    {
        os<< "Name:"<<((*iter).first)<<endl;
        os<< "Expression: "<<((*iter).second)<<endl;
    }

    os<< "Lambda: "<<endl;
    for (map< lambda_name, lambda_expression >::const_iterator iter=ag.lambda_expressions.begin(); iter!=ag.lambda_expressions.end();iter++)
    {
        os<< "Name:"<<((*iter).first)<<endl;
        os<< "Expression: "<<((*iter).second)<<endl;
    }

    os<< "Events: "<<endl;
    for (map< event_name, event_expression >::const_iterator iter=ag.events_expressions.begin(); iter!=ag.events_expressions.end();iter++)
    {
        os<< "Event:"<<((*iter).first)<<endl;
        os<< "Expression: "<<((*iter).second)<<endl;
    }

    os<< "Automaton: "<< ag.automaton_name<<endl;
    for (map< discreteState_Name, map< event_name, discreteState_Name > >::const_iterator iter=ag.automaton.begin(); iter!=ag.automaton.end();iter++)
    {

        string actual_state=((*iter).first);

        os<< "Actual State:"<<actual_state<<endl;


        for (map< event_name, discreteState_Name >::const_iterator iiter=ag.automaton.at(actual_state).begin(); iiter!=ag.automaton.at(actual_state).end();iiter++)
        {
            os<< "Event:"<<((*iiter).first)<<"->"<<((*iiter).second)<<endl;

        }
    }
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


void operator>> (const YAML::Node& node, Parsed_Agent& ag)
{
    node["AGENT"]>>ag.name;
    node["STATES"]>>ag.state;
    node["CONTROL_COMMANDS"]>>ag.inputs;

    
    for (unsigned int i=0;i<ag.state.size();i++)
    {	dynamic_expression tmp_exp;
        node["DYNAMIC_MAP"][0][ag.state[i]]>>tmp_exp;
        ag.expressions.insert(std::pair<stateVariable,dynamic_expression>(ag.state[i],tmp_exp));
	

        initial_state_value tmp_initial;
	node["INITIAL"][0][ag.state[i]]>>tmp_initial;
        ag.initial_states.insert(pair<stateVariable,initial_state_value>(ag.state[i],tmp_initial));
    }

    const YAML::Node& behavior= node["BEHAVIOR"][0];
    const YAML::Node& controllers = behavior["CONTROLLERS"];

    string temp;
    for (unsigned int i=0;i<controllers.size();i++) {
        map<int,string> temp1;
        controllers[i]["NAME"]>>temp;

        ag.controllers.insert(pair<controller_name, map<int,controllerRule> >(temp,temp1));

        for (unsigned int j=0;j<ag.inputs.size();j++)
        {
            string temp1;
            controllers[i][ag.inputs[j]]>> ag.controllers[temp][j];
        }
    }

    const YAML::Node& disc_states = behavior["DISCRETE_STATES"];

    for (unsigned int i=0;i<disc_states.size();i++) {
        string n_state;
        disc_states[i]["NAME"]>>n_state;
        string c_name;
        disc_states[i]["CONTROLLER"]>>c_name;
        ag.discrete_states.insert(pair<string,string>(n_state,c_name));

    }


    const YAML::Node& encoder = behavior["ENCODER"][0];
    const YAML::Node& topology=encoder["TOPOLOGY"][0];
    topology["TOPOLOGY"]>> ag.topology;


    for (unsigned int i=0;i<ag.topology.size();i++) {
        string topology_exp;
        topology[ag.topology[i]]>>topology_exp;
        ag.topology_expressions.insert(pair<string,string>(ag.topology[i],topology_exp));

    }

    const YAML::Node& lambda=encoder["LAMBDA"][0];

    lambda["LAMBDA"]>> ag.lambda;


    for (unsigned int i=0;i<ag.lambda.size();i++) {
        string lambda_exp;
        lambda[ag.lambda[i]]>>lambda_exp;
        ag.lambda_expressions.insert(pair<string,string>(ag.lambda[i],lambda_exp));

    }


    const YAML::Node& decoder = behavior["EVENTS"][0];

    decoder["EVENTS"]>> ag.events;


    for (unsigned int i=0;i<ag.events.size();i++) {
        string events_exp;
        decoder[ag.events[i]]>>events_exp;
        ag.events_expressions.insert(pair<string,string>(ag.events[i],events_exp));

    }


    const YAML::Node& automaton = behavior["AUTOMATON"];
	automaton[0]["NAME"]>>ag.automaton_name;


    for (unsigned int i=0;i<automaton.size();i++) {
        for (map< discreteState_Name, controller_name >::const_iterator iter=ag.discrete_states.begin(); iter!=ag.discrete_states.end();iter++)
        {
            string actual_state;
            actual_state=((*iter).first);
            if (automaton[i].FindValue(actual_state)) {
                map<string,string> temp1;
                ag.automaton.insert(pair<string, map<string,string> >(actual_state,temp1));

                const YAML::Node& transition = automaton[i][actual_state][0];

                for (map< discreteState_Name, controller_name >::const_iterator iiter=ag.discrete_states.begin(); iiter!=ag.discrete_states.end();iiter++)
                {
                    string new_state;
                    new_state=(*iiter).first;

                    if (transition.FindValue(new_state)) {
                        vector<event_name> tran_ev;

                        transition[new_state]>> tran_ev;


                        for (unsigned int j=0; j< tran_ev.size();j++) {
                            if (ag.automaton[actual_state].count(tran_ev[j]))
                            {
                                ERR("DUPLICATED EVENT %s", tran_ev[j].c_str());
                                throw "DUPLICATED EVENT";
                            }
                            ag.automaton[actual_state].insert(pair<event_name, discreteState_Name>(tran_ev[j],new_state));
                        }
                    }
                }
            }
        }
    }

}



