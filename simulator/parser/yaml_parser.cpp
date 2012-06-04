#include "yaml_parser.h"

void parse_file(const char * file_name){
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
	  cout<<Agents[i];
	  cout<<endl;
	  
           
        }
  
}
    
ostream& operator<< (ostream& os, Parsed_Agent& ag){
  
	    cout << ag.name << "\n";
            for (int i =0 ;i<ag.states.size();i++)
            {
                cout<< "State:"<<ag.states[i]<<endl;
                cout<< "Map: "<<ag.expressions[ag.states[i]]<<endl;
                cout<< "initial value: "<<ag.initial_states[ag.states[i]]<<endl;
            }

            std::cout << "Controllers" << "\n";
            int i=0;
            for (map<string, map_int_string>::iterator iter=ag.controllers.begin(); iter!=ag.controllers.end();iter++)
            {
                i++;
                cout<< "Controller "<<i<<endl;
                cout<< "Name:"<<((*iter).first)<<endl;
                cout<< "Values for Command: "<<endl;

                for (int j=0; j<(*iter).second.size();j++)
                {
                    cout<< ag.inputs[j]<<": "<<((*iter).second[j])<<endl;

                }
            }
            cout<< "Discrete States: "<<endl;
            for (map<string, string>::iterator iter=ag.discrete_states.begin(); iter!=ag.discrete_states.end();iter++)
            {
                cout<< "Name:"<<((*iter).first)<<endl;
                cout<< "Controller: "<<((*iter).second)<<endl;
            }

            cout<< "Topologies: "<<endl;
            for (map<string, string>::iterator iter=ag.topology_expressions.begin(); iter!=ag.topology_expressions.end();iter++)
            {
                cout<< "Name:"<<((*iter).first)<<endl;
                cout<< "Expression: "<<((*iter).second)<<endl;
            }

            cout<< "Lambda: "<<endl;
            for (map<string, string>::iterator iter=ag.lambda_expressions.begin(); iter!=ag.lambda_expressions.end();iter++)
            {
                cout<< "Name:"<<((*iter).first)<<endl;
                cout<< "Expression: "<<((*iter).second)<<endl;
            }

            cout<< "Events: "<<endl;
            for (map<string, string>::iterator iter=ag.events_expressions.begin(); iter!=ag.events_expressions.end();iter++)
            {
                cout<< "Event:"<<((*iter).first)<<endl;
                cout<< "Expression: "<<((*iter).second)<<endl;
            }

            cout<< "Automaton: "<<endl;
            for (map<string, map_string_string>::iterator iter=ag.automaton.begin(); iter!=ag.automaton.end();iter++)
            {

                string actual_state=((*iter).first);

                cout<< "Actual State:"<<actual_state<<endl;


                for (map<string,string>::iterator iiter=ag.automaton[actual_state].begin(); iiter!=ag.automaton[actual_state].end();iiter++)
                {
                    cout<< "Event:"<<((*iiter).first)<<"->"<<((*iiter).second)<<endl;

                }
            }

}




void operator>> (const YAML::Node& node, Parsed_Agent& ag)
{
    node["AGENT"]>>ag.name;
    node["STATES"]>>ag.states;
    node["CONTROL_COMMANDS"]>>ag.inputs;
    
    string temp;
    for (int i=0;i<ag.states.size();i++)
    {
        node["DYNAMIC_MAP"][0][ag.states[i]]>>temp;
        ag.expressions.insert(std::make_pair<string,string>(ag.states[i],temp));

        node["INITIAL"][0][ag.states[i]]>>temp;
        ag.initial_states.insert(std::make_pair<string,string>(ag.states[i],temp));
    }
    
    const YAML::Node& behavior= node["BEHAVIOR"][0];
    const YAML::Node& controllers = behavior["CONTROLLERS"];


    for (int i=0;i<controllers.size();i++) {
        map_int_string temp1;
        controllers[i]["NAME"]>>temp;

        ag.controllers.insert(make_pair<string, map_int_string>(temp,temp1));

        for (int j=0;j<ag.inputs.size();j++)
        {
            string temp1;
            controllers[i][ag.inputs[j]]>> temp1;

            ag.controllers[temp][j]=temp1;


        }
    }

    const YAML::Node& disc_states = behavior["DISCRETE_STATES"];

    for (int i=0;i<disc_states.size();i++) {
        string n_state;
        disc_states[i]["NAME"]>>n_state;
        string c_name;
        disc_states[i]["CONTROLLER"]>>c_name;
        ag.discrete_states.insert(make_pair<string,string>(n_state,c_name));

    }


    const YAML::Node& encoder = behavior["ENCODER"][0];

    encoder["TOPOLOGY"]>> ag.topology;


    for (int i=0;i<ag.topology.size();i++) {
        string topology_exp;
        encoder[ag.topology[i]]>>topology_exp;
        ag.topology_expressions.insert(make_pair<string,string>(ag.topology[i],topology_exp));

    }

    encoder["LAMBDA"]>> ag.lambda;


    for (int i=0;i<ag.lambda.size();i++) {
        string lambda_exp;
        encoder[ag.lambda[i]]>>lambda_exp;
        ag.lambda_expressions.insert(make_pair<string,string>(ag.lambda[i],lambda_exp));

    }


    const YAML::Node& decoder = behavior["EVENTS"][0];

    decoder["EVENTS"]>> ag.events;


    for (int i=0;i<ag.events.size();i++) {
        string events_exp;
        decoder[ag.events[i]]>>events_exp;
        ag.events_expressions.insert(make_pair<string,string>(ag.events[i],events_exp));

    }


    const YAML::Node& automaton = behavior["AUTOMATON"];


    for (int i=0;i<automaton.size();i++) {
        for (map<string, string>::iterator iter=ag.discrete_states.begin(); iter!=ag.discrete_states.end();iter++)
        {
            string actual_state;
            actual_state=((*iter).first);
            if (automaton[i].FindValue(actual_state)) {
                map_string_string temp1;
                ag.automaton.insert(make_pair<string, map_string_string>(actual_state,temp1));

                const YAML::Node& transition = automaton[i][actual_state][0];

                for (map<string, string>::iterator iiter=ag.discrete_states.begin(); iiter!=ag.discrete_states.end();iiter++)
                {
                    string new_state;
                    new_state=(*iiter).first;

                    if (transition.FindValue(new_state)) {
                        vector<string> tran_ev;

                        transition[new_state]>> tran_ev;


                        for (int j=0; j< tran_ev.size();j++) {
                            if (ag.automaton[actual_state].count(tran_ev[j]))
                            {
                                ERR("DUPLICATED EVENT %s", tran_ev[j].c_str());
                                throw "DUPLICATED EVENT";
                            }
                            ag.automaton[actual_state].insert(make_pair<string, string>(tran_ev[j],new_state));
                        }
                    }
                }
            }
        }
    }

}



