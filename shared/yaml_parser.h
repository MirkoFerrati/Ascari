#ifndef PARSED_AGENT_H
#define PARSED_AGENT_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include "logog.hpp"
#include <map>
#include <yaml-cpp/yaml.h>
#include "../plugins/abstract_parser_plugin.h"
#include <boost/algorithm/string.hpp>
#include <boost/serialization/strong_typedef.hpp>

typedef std::string bonusVariable;
typedef std::string bonus_expression;
typedef std::string stateVariable;
typedef std::string controlVariable;
typedef double initial_state_value;
typedef std::string dynamic_expression;
typedef std::string communication_area;
typedef int marker_type;

class Parsed_Behavior {
public:
  std::string name;
   std::vector<stateVariable> state;
    std::vector<controlVariable> inputs;
    std::map<stateVariable, dynamic_expression> expressions;
    bool load_from_node( const YAML::Node& node);

};

class Parsed_Agent {

public:
	Parsed_Agent(const std::unique_ptr<Parsed_Behavior>& behavior):behavior(behavior)
	{
	}
	friend std::ostream& operator<<(std::ostream& os, const Parsed_Agent& ag );

    std::string name;
   
    std::map<stateVariable,initial_state_value> initial_states;
      
    std::string visibility;
    communication_area communication;

    const std::unique_ptr<Parsed_Behavior>& behavior;
    std::string behavior_name;
    
    marker_type marker;
    bool simulated=true;

    bool load_from_node( const YAML::Node& node);
    std::map<std::string,abstract_parsed_agent_plugin*> parsed_items_from_plugins;
};

class Parsed_World{
  
  public:
  friend std::ostream& operator<<(std::ostream& os, const Parsed_World& wo );
  
  bool parsedSuccessfully;

  std::vector<bonusVariable> bonus_variables;
  std::map<bonusVariable, bonus_expression> bonus_expressions;
  std::list<Parsed_Agent> agents;
  std::map<std::string,std::unique_ptr< Parsed_Behavior>> behaviors;
  std::string mapfilename;
  
  std::vector<abstract_parser_plugin*> plugins; 


  std::map<std::string,abstract_parsed_world_plugin*> parsed_items_from_plugins;
  bool load_from_node(const YAML::Node& node);

};
    

class yaml_parser
{
public:
  yaml_parser();
  yaml_parser( std::vector< abstract_parser_plugin* > plugins );
  void addPlugin(abstract_parser_plugin*);
    Parsed_World parse_file(const char * file_name);
    Parsed_World parse_file(std::string file_name);
private:
  std::vector<abstract_parser_plugin*> plugins;
};
#endif

