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

using namespace std;


typedef vector<string> States;
typedef vector<string> Inputs;
typedef map<string,string> Map_expressions;
typedef map<string,string> Initial_states;
typedef map<int,string> map_int_string;
typedef map<string, map_int_string> Controllers;
typedef map<string, string> Discrete_states;
typedef vector<string> Topology;
typedef map<string, string> Topology_expressions;
typedef vector<string> Lambda;
typedef map<string, string> Lambda_expressions;
typedef vector<string> Events;
typedef map<string, string> Events_expressions;
typedef map<string,string> map_string_string;
typedef map<string, map_string_string> Automaton_table;



class Parsed_Agent {

public:
    string name;
    States states;
    Inputs inputs;
    Map_expressions expressions;
    Initial_states initial_states;
    Controllers controllers;
    Discrete_states discrete_states;
    Topology	topology;
    Topology_expressions topology_expressions;
    Lambda lambda;
    Lambda_expressions lambda_expressions;
    Events events;
    Events_expressions events_expressions;
    Automaton_table automaton;
    
};
    std::vector<Parsed_Agent> parse_file(const char * file_name);
    void operator>>(const YAML::Node& node, Parsed_Agent& ag);
    ostream& operator<<(ostream& os, Parsed_Agent& ag);
    ostream& operator<< (ostream& os, std::vector<Parsed_Agent>& ag);


    
    
    
#endif

