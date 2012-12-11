#include "yaml_parser.h"
#include "parserYAML/include/yaml-cpp/node.h"

using namespace std;

Parsed_World parse_file(const char * file_name)
{
    std::string temp(file_name);
    return parse_file(temp);
}


Parsed_World parse_file(string file_name) {
    std::ifstream fin(file_name.c_str());
    std::ostringstream str;
    str<<fin.rdbuf();
    std::string new_str=str.str();
    boost::algorithm::to_upper(new_str);
    std::stringstream new_new_str;
    new_new_str<<new_str;


    YAML::Parser parser(new_new_str);
    YAML::Node doc;
    parser.GetNextDocument(doc);


    Parsed_World World;//(doc[1]["AGENTS"].size());

    doc>>World;


    return World;

}


void operator>> (const YAML::Node& node, Parsed_Behavior& beh)
{

}


void operator>> (const YAML::Node& node, Parsed_Agent& ag)
{

    node["VISIBLE_AREA"]>>ag.visibility;
    node["COMMUNICATION_AREA"]>>ag.communication;
    node["AGENT"]>>ag.name;
    node["STATES"]>>ag.behavior.state;
    node["CONTROL_COMMANDS"]>>ag.behavior.inputs;


    for (unsigned int i=0;i<ag.behavior.state.size();i++)
    {
        dynamic_expression tmp_exp;
        node["DYNAMIC_MAP"][0][ag.behavior.state[i]]>>tmp_exp;
        ag.behavior.expressions.insert(std::pair<stateVariable,dynamic_expression>(ag.behavior.state[i],tmp_exp));


        initial_state_value tmp_initial;
        node["INITIAL"][0][ag.behavior.state[i]]>>tmp_initial;
        ag.initial_states.insert(pair<stateVariable,initial_state_value>(ag.behavior.state[i],tmp_initial));
    }

    const YAML::Node& behavior= node["BEHAVIOR"][0];
    const YAML::Node& controllers = behavior["CONTROLLERS"];

    string temp;
    for (unsigned int i=0;i<controllers.size();i++) {
        map<int,string> temp1;
        controllers[i]["NAME"]>>temp;

        ag.behavior.controllers.insert(pair<controller_name, map<int,controllerRule> >(temp,temp1));

        for (unsigned int j=0;j<ag.behavior.inputs.size();j++)
        {
            controllers[i][ag.behavior.inputs[j]]>> ag.behavior.controllers[temp][j];
        }
    }

    const YAML::Node& disc_states = behavior["DISCRETE_STATES"];

    for (unsigned int i=0;i<disc_states.size();i++) {
        string n_state;
        disc_states[i]["NAME"]>>n_state;
        string c_name;
        disc_states[i]["CONTROLLER"]>>c_name;
        ag.behavior.discrete_states.insert(pair<string,string>(n_state,c_name));

    }

    node["STATE_START"]>> ag.state_start;

    if (node.FindValue("TARGET_LIST")) {
        node["TARGET_LIST"]>> ag.target_list;
    }

    if (!ag.behavior.discrete_states.count(ag.state_start))
    {
        ERR("UNDEFINED START DISCRETE STATE %s", ag.state_start.c_str());
        throw "UNDEFINED DISCRETE START STATE";
    }




    const YAML::Node& encoder = behavior["ENCODER"][0];
    const YAML::Node& topology=encoder["TOPOLOGY"][0];
    topology["TOPOLOGY"]>> ag.behavior.topology;


    for (unsigned int i=0;i<ag.behavior.topology.size();i++) {
        string topology_exp;
        topology[ag.behavior.topology[i]]>>topology_exp;
        ag.behavior.topology_expressions.insert(pair<string,string>(ag.behavior.topology[i],topology_exp));

    }

    const YAML::Node& lambda=encoder["LAMBDA"][0];

    lambda["LAMBDA"]>> ag.behavior.lambda;


    for (unsigned int i=0;i<ag.behavior.lambda.size();i++) {
        string lambda_exp;
        lambda[ag.behavior.lambda[i]]>>lambda_exp;
        ag.behavior.lambda_expressions.insert(pair<string,string>(ag.behavior.lambda[i],lambda_exp));

    }


    const YAML::Node& decoder = behavior["EVENTS"][0];

    decoder["EVENTS"]>> ag.behavior.events;


    for (unsigned int i=0;i<ag.behavior.events.size();i++) {
        string events_exp;
        decoder[ag.behavior.events[i]]>>events_exp;
        ag.behavior.events_expressions.insert(pair<string,string>(ag.behavior.events[i],events_exp));

    }


    const YAML::Node& automaton = behavior["AUTOMATON"];
    automaton[0]["NAME"]>>ag.behavior.name;


    for (unsigned int i=0;i<automaton.size();i++) {
        for (map< discreteState_Name, controller_name >::const_iterator iter=ag.behavior.discrete_states.begin(); iter!=ag.behavior.discrete_states.end();++iter)
        {
            string actual_state;
            actual_state=((*iter).first);
            if (automaton[i].FindValue(actual_state)) {
                map<string,string> temp1;
                ag.behavior.automaton.insert(pair<string, map<string,string> >(actual_state,temp1));

                const YAML::Node& transition = automaton[i][actual_state][0];

                for (map< discreteState_Name, controller_name >::const_iterator iiter=ag.behavior.discrete_states.begin(); iiter!=ag.behavior.discrete_states.end();++iiter)
                {
                    string new_state;
                    new_state=(*iiter).first;

                    if (transition.FindValue(new_state)) {
                        vector<event_name> tran_ev;

                        transition[new_state]>> tran_ev;


                        for (unsigned int j=0; j< tran_ev.size();j++) {
                            if (ag.behavior.automaton[actual_state].count(tran_ev[j]))
                            {
                                ERR("DUPLICATED EVENT %s", tran_ev[j].c_str());
                                throw "DUPLICATED EVENT";
                            }
                            ag.behavior.automaton[actual_state].insert(pair<event_name, discreteState_Name>(tran_ev[j],new_state));
                        }
                    }
                }
            }
        }
    }

}



void operator>>(const YAML::Node& node, Parsed_World& wo)
{
	wo.graphName="UNSET";
	
    if (node[0].FindValue("WORLD"))
    {
        if (node[0]["WORLD"].size()>0)
        {
			
            if (node[0]["WORLD"][0].FindValue("BONUS_VARIABLES"))
            {
                const YAML::Node &world_node=node[0]["WORLD"][0];
                world_node["BONUS_VARIABLES"]>> wo.bonus_variables;
                for (unsigned int i=0;i<wo.bonus_variables.size();i++) {
                    string bonus_exp;
                    world_node[wo.bonus_variables[i]]>>bonus_exp;
                    wo.bonus_expressions.insert(make_pair(wo.bonus_variables.at(i),bonus_exp));
                }
            }

            if (node[0]["WORLD"][0].FindValue("GRAPH_NAME"))
            {
                const YAML::Node &world_node=node[0]["WORLD"][0];

                world_node["GRAPH_NAME"]>> wo.graphName;
            }

        }
    }

      const YAML::Node &behaviors_nodes=node[0]["BEHAVIORS"];

    wo.behaviors.resize(behaviors_nodes.size());

    for (unsigned int i=0;i<behaviors_nodes.size();i++) {

        behaviors_nodes[i] >> wo.behaviors[i];
    }

    
    const YAML::Node &agent_nodes=node[0]["AGENTS"];

    wo.agents.resize(agent_nodes.size());

    for (unsigned int i=0;i<agent_nodes.size();i++) {

        agent_nodes[i] >> wo.agents[i];
        if ((wo.agents[i].target_list.size()==0 && wo.graphName.compare("UNSET")!=0)|| (wo.agents[i].target_list.size()>0 && wo.graphName.compare("UNSET")==0) ) {

            ERR("GRAPH NAME OR TARGET LIST UNDEFINED", NULL);
            throw "GRAPH NAME OR TARGET LIST UNDEFINED";
        }

    }


}
