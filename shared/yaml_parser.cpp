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


void operator>> (const YAML::Node& node, Parsed_Behavior& behavior)
{

  
    node["STATES"]>>behavior.state;
    node["CONTROL_COMMANDS"]>>behavior.inputs;
    node["NAME"]>>behavior.name;

    
    
  for (unsigned int i=0;i<behavior.state.size();i++)
    {
        dynamic_expression tmp_exp;
        node["DYNAMIC_MAP"][0][behavior.state[i]]>>tmp_exp;
        behavior.expressions.insert(std::pair<stateVariable,dynamic_expression>(behavior.state[i],tmp_exp));
    
      
    }

    const YAML::Node& controllers = node["CONTROLLERS"];

    string temp;
    for (unsigned int i=0;i<controllers.size();i++) {
        map<int,string> temp1;
        controllers[i]["NAME"]>>temp;

        behavior.controllers.insert(pair<controller_name, map<int,controllerRule> >(temp,temp1));

        for (unsigned int j=0;j<behavior.inputs.size();j++)
        {
            controllers[i][behavior.inputs[j]]>> behavior.controllers[temp][j];
        }
    }

    const YAML::Node& disc_states = node["DISCRETE_STATES"];

    for (unsigned int i=0;i<disc_states.size();i++) {
        string n_state;
        disc_states[i]["NAME"]>>n_state;
        string c_name;
        disc_states[i]["CONTROLLER"]>>c_name;
        behavior.discrete_states.insert(pair<string,string>(n_state,c_name));

    }

    
    const YAML::Node& encoder = node["ENCODER"][0];
    const YAML::Node& topology=encoder["TOPOLOGY"][0];
    topology["TOPOLOGY"]>> behavior.topology;


    for (unsigned int i=0;i<behavior.topology.size();i++) {
        string topology_exp;
        topology[behavior.topology[i]]>>topology_exp;
        behavior.topology_expressions.insert(pair<string,string>(behavior.topology[i],topology_exp));

    }

    const YAML::Node& lambda=encoder["LAMBDA"][0];

    lambda["LAMBDA"]>> behavior.lambda;


    for (unsigned int i=0;i<behavior.lambda.size();i++) {
        string lambda_exp;
        lambda[behavior.lambda[i]]>>lambda_exp;
        behavior.lambda_expressions.insert(pair<string,string>(behavior.lambda[i],lambda_exp));

    }


    const YAML::Node& decoder = node["EVENTS"][0];

    decoder["EVENTS"]>> behavior.events;


    for (unsigned int i=0;i<behavior.events.size();i++) {
        string events_exp;
        decoder[behavior.events[i]]>>events_exp;
        behavior.events_expressions.insert(pair<string,string>(behavior.events[i],events_exp));

    }


    const YAML::Node& automaton = node["AUTOMATON"];
    //automaton[0]["NAME"]>>behavior.name;


    for (unsigned int i=0;i<automaton.size();i++) {
        for (map< discreteState_Name, controller_name >::const_iterator iter=behavior.discrete_states.begin(); iter!=behavior.discrete_states.end();++iter)
        {
            string actual_state;
            actual_state=((*iter).first);
            if (automaton[i].FindValue(actual_state)) {
                map<string,string> temp1;
                behavior.automaton.insert(pair<string, map<string,string> >(actual_state,temp1));

                const YAML::Node& transition = automaton[i][actual_state][0];

                for (map< discreteState_Name, controller_name >::const_iterator iiter=behavior.discrete_states.begin(); iiter!=behavior.discrete_states.end();++iiter)
                {
                    string new_state;
                    new_state=(*iiter).first;

                    if (transition.FindValue(new_state)) {
                        vector<event_name> tran_ev;

                        transition[new_state]>> tran_ev;


                        for (unsigned int j=0; j< tran_ev.size();j++) {
                            if (behavior.automaton[actual_state].count(tran_ev[j]))
                            {
                                ERR("DUPLICATED EVENT %s", tran_ev[j].c_str());
                                throw "DUPLICATED EVENT";
                            }
                            behavior.automaton[actual_state].insert(pair<event_name, discreteState_Name>(tran_ev[j],new_state));
                        }
                    }
                }
            }
        }
    }

  
}


void operator>> (const YAML::Node& node, Parsed_Agent& ag)
{

    node["VISIBLE_AREA"]>>ag.visibility;
    node["COMMUNICATION_AREA"]>>ag.communication;
   
    
    
    
    for (unsigned int i=0;i<ag.behavior.state.size();i++)
    {
        initial_state_value tmp_initial;
        node["INITIAL"][0][ag.behavior.state[i]]>>tmp_initial;
        ag.initial_states.insert(pair<stateVariable,initial_state_value>(ag.behavior.state[i],tmp_initial));
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

   
    for (unsigned int i=0;i<behaviors_nodes.size();i++) {
	
      std::string tmp_beh_name;
      
       if (!behaviors_nodes[i].FindValue("NAME")){
	 
            ERR("BEHAVIOR NAME UNDEFINED: BEHAVIOR NUMBER %d", i);
            throw "BEHAVIOR NAME UNDEFINED";
        }
      
	behaviors_nodes[i]["NAME"]>> tmp_beh_name;
	std::unique_ptr<Parsed_Behavior> tmp_ptr(new Parsed_Behavior());
        behaviors_nodes[i] >> *tmp_ptr;
	wo.behaviors[tmp_beh_name]=std::move(tmp_ptr);
	
    }

    
    const YAML::Node &agent_nodes=node[0]["AGENTS"];

    wo.agents.resize(agent_nodes.size());

    for (unsigned int i=0;i<agent_nodes.size();i++) {
      
      std::string tmp_ag_name;
      agent_nodes[i]["AGENT"]>>tmp_ag_name;
    
     if (!agent_nodes[i].FindValue("BEHAVIOR"))
    {
        ERR("BEHAVIOR NAME UNSPECIFIED FOR AGENT %s", tmp_ag_name.c_str());
        throw "BEHAVIOR NAME UNSPECIFIED FOR AGENT";
    }
    
    std::string tmp_agent_behavior_name;
    agent_nodes[i]["BEHAVIOR"]>>tmp_agent_behavior_name;
    
     if (!wo.behaviors.count(tmp_agent_behavior_name))
    {
        ERR("UNDEFINED BEHAVIOR %s FOR AGENT %s", tmp_agent_behavior_name.c_str(), tmp_ag_name.c_str());
        throw "UNDEFINED BEHAVIOR %s FOR AGENT %s";
    }
	wo.agents[i].name=tmp_ag_name;
	wo.agents[i].behavior=(*wo.behaviors[tmp_agent_behavior_name]);//TODO(Mirko) perchè è una copia?
	wo.agents[i].behavior_name=tmp_agent_behavior_name;
        agent_nodes[i] >> wo.agents[i];
        if ((wo.agents[i].target_list.size()==0 && wo.graphName.compare("UNSET")!=0)|| (wo.agents[i].target_list.size()>0 && wo.graphName.compare("UNSET")==0) ) {

            ERR("GRAPH NAME OR TARGET LIST UNDEFINED", NULL);
            throw "GRAPH NAME OR TARGET LIST UNDEFINED";
        }

    }


}
