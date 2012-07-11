#ifndef PARSED_AGENT_H
#define PARSED_AGENT_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include "logog.hpp"

#include "yaml-cpp/yaml.h"
#include <boost/algorithm/string.hpp>
#include <boost/strong_typedef.hpp>

// using namespace std;

typedef std::string bonusVariable;
typedef std::string bonus_expression;


typedef std::string stateVariable;
typedef std::string controlVariable;
typedef double initial_state_value;
typedef std::string controller_name;
typedef std::string controllerRule;
typedef std::string dynamic_expression;
typedef std::map<int,controllerRule> controller_MapRules;
typedef std::string discreteState_Name;
typedef std::string topology_name;
typedef std::string topology_expression;
typedef std::string lambda_name;
typedef std::string lambda_expression;
// typedef std::map<lambda_name, lambda_expression> Lambda_MapExpressions;
typedef std::string event_name;
typedef std::string event_expression;
typedef std::string visible_area;
typedef std::string communication_area;
typedef int target_id;


// typedef std::map<event_name, event_expression> Events_MapExpressions;



class Parsed_Agent {

public:
    friend std::ostream& operator<<(std::ostream& os, const Parsed_Agent& ag );
    
    std::string name;
    std::vector<stateVariable> state;
    std::vector<controlVariable> inputs;
    std::map<stateVariable, dynamic_expression> expressions;
    std::map<stateVariable,initial_state_value> initial_states;
    std::map<controller_name,controller_MapRules> controllers;
    std::map<discreteState_Name, controller_name> discrete_states;
    discreteState_Name state_start;
    std::vector<topology_name>	topology;
    std::map<topology_name, topology_expression> topology_expressions;
    std::vector<lambda_name> lambda;
    std::map<lambda_name, lambda_expression> lambda_expressions;
	std::vector<event_name> events;
    std::map<event_name, event_expression> events_expressions;
    std::map<discreteState_Name, std::map<event_name,discreteState_Name> > automaton;
	std::string automaton_name;
    visible_area visibility;
    communication_area communication;
    
    std::vector<target_id> target_list;
    

    
};

class Parsed_World{
  
  public:
  friend std::ostream& operator<<(std::ostream& os, const Parsed_World& wo );
  

  std::vector<bonusVariable> bonus_variables;
  std::map<bonusVariable, bonus_expression> bonus_expressions;
  std::vector<Parsed_Agent> agents;
  

//   Parsed_World(int num_agents):agents(num_agents){}
   
};



    Parsed_World parse_file(const char * file_name);
    
    void operator>>(const YAML::Node& node, Parsed_Agent& ag);
    void operator>>(const YAML::Node& node, Parsed_World& wo);
    
    std::ostream& operator<< (std::ostream& , const std::vector<Parsed_Agent>& );
    


    
    
    
#endif

