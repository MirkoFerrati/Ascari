#ifndef PARSED_AGENT_H
#define PARSED_AGENT_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include "logog.hpp"
#include <map>
#include "yaml-cpp/yaml.h"
#include <boost/algorithm/string.hpp>
#include <boost/strong_typedef.hpp>
//written by Alessandro Settimi
#include "typedefs.h"
//written by Alessandro Settimi

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
typedef std::string graph_name;


typedef int target_id;

// typedef std::map<event_name, event_expression> Events_MapExpressions;

class Parsed_Behavior {
public:
  std::string name;
   std::vector<stateVariable> state;
    std::vector<controlVariable> inputs;
    std::map<stateVariable, dynamic_expression> expressions;
   std::map<controller_name,controller_MapRules> controllers;
    std::map<discreteState_Name, controller_name> discrete_states;
   std::vector<topology_name>	topology;
    std::map<topology_name, topology_expression> topology_expressions;
    std::vector<lambda_name> lambda;
    std::map<lambda_name, lambda_expression> lambda_expressions;
	std::vector<event_name> events;
    std::map<event_name, event_expression> events_expressions;
    std::map<discreteState_Name, std::map<event_name,discreteState_Name> > automaton;
	
};

class Parsed_Agent {

public:
	Parsed_Agent(const std::unique_ptr<Parsed_Behavior>& behavior):behavior(behavior)
	{
		
	}
	friend std::ostream& operator<<(std::ostream& os, const Parsed_Agent& ag );
    
    std::string name;
   
    std::map<stateVariable,initial_state_value> initial_states;
   
    discreteState_Name state_start;
   
    visible_area visibility;
    communication_area communication;
    
    std::vector<target_id> target_list;
    
    const std::unique_ptr<Parsed_Behavior>& behavior;
    std::string behavior_name;
    
    //written by Alessandro Settimi
    task_assignment_namespace::task_cost_vector agent_task_cost_vector;
    //written by Alessandro Settimi
    
};

class Parsed_World{
  
  public:
  friend std::ostream& operator<<(std::ostream& os, const Parsed_World& wo );
  

  std::vector<bonusVariable> bonus_variables;
  std::map<bonusVariable, bonus_expression> bonus_expressions;
  std::vector<Parsed_Agent> agents;
  std::map<std::string,std::unique_ptr< Parsed_Behavior>> behaviors;
  std::string graphName;
  
  //written by Alessandro Settimi
  task_assignment_namespace::task_list task_list;
  task_assignment_namespace::task_assignment_algorithm task_assignment_algorithm;
  //written by Alessandro Settimi

//   Parsed_World(int num_agents):agents(num_agents){}
   
};


    
    Parsed_World parse_file(const char * file_name);
    Parsed_World parse_file(std::string file_name);
    void operator>>(const YAML::Node& node, std::unique_ptr< Parsed_Behavior >& behavior);
    void operator>>(const YAML::Node& node, Parsed_World& wo);
    void operator>>(const YAML::Node& node, Parsed_Behavior& beh);
    std::ostream& operator<< (std::ostream& , const std::vector<Parsed_Agent>& );
    


    
    
    
#endif

