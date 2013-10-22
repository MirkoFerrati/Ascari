#ifndef FAKE_SIMULATOR_H
#define FAKE_SIMULATOR_H
#include <boost/iterator/iterator_concepts.hpp>
#include <yaml_parser.h>
#include <communication/zmq_localization_communicator.hpp>
#include <types/agent_sim_packet.h>
#include <types/world_sim_packet.h>
#include <types/control_command_packet.h>
#include <dynamic_module_abstract.h>
#include "zmq_agent_communicator.h"

class fake_simulator
{
public:
    void create_communicator(int type,const Parsed_World& world);
    void initialize(const Parsed_World& world,std::string agent_name);
    void initialize_agents ( const std::list<Parsed_Agent>& agents,std::string agent_name );
    void setPeriod(unsigned period);
    void start_sim(int count=1000);
    ~fake_simulator();
    fake_simulator();

private:
    zmq_agent_communicator* communicator;
    unsigned int cycle_period_millisec;


    int max_loops;
    std::vector<dynamic_module_abstract*> dynamic_modules;
    zmq_localization_communicator_receiver localization_receiver;
    agent_sim_packet agent_packet;
    world_sim_packet sim_packet;
    index_map agent_states_to_index;
    std::vector<index_map> agent_commands_to_index;
    index_map agents_name_to_index;
    std::map<int,double> bonusVariables;
    std::map<std::string,int> map_bonus_variables_to_id;
    std::map<std::string,control_command_packet> commands;
    void main_loop();
    simulation_time time;
    std::vector<exprtk::expression<double> > bonus_expressions;
    exprtk::symbol_table<double> bonus_symbol_table;
    std::map<std::string,int> map_bonus_variables;
    std::string agent_name;

};

#endif // FAKE_SIMULATOR_H
