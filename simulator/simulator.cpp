#include "simulator.h"
#include <udp_agent_communicator.h>
#include "typedefs.h"
#include "logog.hpp"
#include "debug_constants.h"
#include <udp_agent_router.hpp>
#include "collisionchecker.h"

using namespace std;

void simulator::create_communicator(int communicator_type)
{
    if (communicator_type==1)
    {
        communicator=new udp_agent_communicator(num_agents);
    }
}

simulator::simulator():
topology_router(SIMULATOR_ROUTE_PORT,AGENT_ROUTE_PORT),graph_router(SIMULATOR_GRAPH_PORT,AGENT_GRAPH_PORT)
{
	max_loops=0;
	communicator=0;
	num_agents=0;
	pi=exprtk::details::numeric::constant::pi;
	f_rndom=0;
	secSleep=5000;
	
}

void simulator::setSleep(unsigned secSleep)
{
	this->secSleep=secSleep;
}

void simulator::setCheckCollision(bool checkCollision)
{
	this->checkCollision=checkCollision;
	if (collisionChecker)
		delete collisionChecker;
	if (checkCollision)
	{	
		collisionChecker = new CollisionChecker(sim_packet.state_agents,agent_states_to_index);
	}
	else
	{
		collisionChecker= new CollisionCheckerAbstract(sim_packet.state_agents,agent_states_to_index);
	}
}


void simulator::initialize(const Parsed_World& wo)
{
    initialize_agents(wo.agents);
    bonus_symbol_table.add_constants();
    int i=0;
    for (map<bonusVariable,bonus_expression>::const_iterator it=wo.bonus_expressions.begin();it!=wo.bonus_expressions.end();++it)
    {
        bonusVariables[i]=0;
        map_bonus_variables_to_id.insert(make_pair(it->first,i));
        i++;
    }
    for (index_map::const_iterator it=map_bonus_variables_to_id.begin();it!=map_bonus_variables_to_id.end();++it)
    {
        bonus_symbol_table.add_variable(it->first,bonusVariables[it->second]);
    }
    pi=exprtk::details::numeric::constant::pi;
    bonus_symbol_table.add_variable("PI_GRECO",pi,true);
    f_rndom = new rndom<double>();
    bonus_symbol_table.add_function(f_rndom->name, *f_rndom);
    exprtk::parser<double> parser;
    for (unsigned int i=0;i<wo.bonus_variables.size();i++)
    {
        sim_packet.bonus_variables.insert(make_pair(wo.bonus_variables.at(i),0));
        exprtk::expression<double> expression_tmp;
        expression_tmp.register_symbol_table(bonus_symbol_table);
        string string_tmp_expression=wo.bonus_expressions.at(wo.bonus_variables.at(i));
        if (parser.compile(string_tmp_expression,expression_tmp))
        {
            bonus_expressions.push_back(expression_tmp);
        }
        else
        {
            ERR("impossibile creare l'espressione: %s",string_tmp_expression.c_str());
            throw "impossibile creare l'espressione";
        }

        map_bonus_variables.insert(make_pair(wo.bonus_variables.at(i),i));
    }
    setCheckCollision(false);
}


void simulator::initialize_agents(const vector<Parsed_Agent>& ag)
{
    num_agents=ag.size();
    for (unsigned int i=0; i<num_agents;i++) {
        agents_name_to_index.insert(make_pair(ag.at(i).name,i));
        agent_state_packet agent_packet_tmp;
        control_command_packet command_packet_tmp;
        sim_packet.state_agents.internal_map.insert(make_pair(ag.at(i).name,agent_packet_tmp));
        commands.insert(make_pair(ag.at(i).name,command_packet_tmp));
        agent_state_packet& agent_packet=sim_packet.state_agents.internal_map.at(ag.at(i).name);
        control_command_packet& command_packet=commands.at(ag.at(i).name);
        agent_packet.identifier=ag.at(i).name;
        command_packet.identifier=ag.at(i).name;
        
        index_map commands_to_index_tmp;

        for (unsigned int j=0; j<ag.at(i).state.size();j++)
        {
            agent_packet.state.insert(make_pair(j,ag.at(i).initial_states.at(ag.at(i).state.at(j))));
			agent_states_to_index.insert(make_pair(ag.at(i).state.at(j),j));
            bonus_symbol_table.add_variable(ag.at(i).state.at(j)+ag.at(i).name,agent_packet.state.at(j));
        }

        for (unsigned int j=0; j<ag.at(i).inputs.size();j++)
        {
            command_packet.command.insert(make_pair(j,0));
            commands_to_index_tmp.insert(make_pair(ag.at(i).inputs.at(j),j));
            bonus_symbol_table.add_variable(ag.at(i).inputs.at(j)+ag.at(i).name,command_packet.command.at(j));
        }
        agent_commands_to_index.push_back(commands_to_index_tmp);

        dynamic *d= new dynamic(sim_packet.state_agents.internal_map.at(ag.at(i).name).state, commands.at(ag.at(i).name).command,
                                ag.at(i).expressions, ag.at(i).state,ag.at(i).inputs);

        dynamic_module.push_back(d);


    }


//     for (map<std::string,agent_state_packet>::iterator iter=states_index.internal_map.begin(); iter!=states_index.internal_map.end();iter++)
//     {
//         cout<<iter->first<<endl;
//         for (map<int,double>::iterator iiter=iter->second.state.begin();iiter!=iter->second.state.end();iiter++) {
//             //cout<< iiter->first<<"->"<<iiter->second<<endl;
//         }
//         for (map<string,int>::iterator iiter=agent_states_to_index.at(agent_name_to_index.at(iter->first)).begin();iiter!=agent_states_to_index.at(agent_name_to_index.at(iter->first)).end();iiter++) {
//             cout<< iiter->first<<"->"<<iter->second.state.at(iiter->second)<<endl;
//         }
//     }
}

void simulator::main_loop()
{
    try {
        sim_packet.time=0;
		int clock=0;
        while (1) {
			clock++;
			if ((clock%10)!=0)
			{
				cout<<".";
				flush(cout);
			}
			else
				cout<<endl<<sim_packet.time;
			sim_packet.time=((simulation_time)clock)/10.0;
//             communicator->send_broadcast(time++);
            update_bonus_variables();
            //communicator->send_broadcast(sim_packet.state_agents);
            communicator->send_broadcast(sim_packet);
// 	    cout<<"inviato pacchetto con gli stati"<<endl;

            agent_state state_tmp;
            for (int i=0;i<10;i++)//TODO(Mirko): this is 1 second/(sampling time of dynamic)
            {
                for (index_map::const_iterator iter=agents_name_to_index.begin(); iter!=agents_name_to_index.end();++iter) {
                    state_tmp=dynamic_module.at(iter->second)->getNextState();
                    for (agent_state::const_iterator iiter=state_tmp.begin();iiter!=state_tmp.end();++iiter) {
                        sim_packet.state_agents.internal_map.at(iter->first).state.at(iiter->first)=iiter->second; //metto il nuovo stato nel posto giusto senza copiare i vettori

                    }
                }
            }
            collisionChecker->checkCollisions();
            usleep(secSleep);
            vector<control_command_packet> temp=communicator->receive_control_commands();
//             cout<<"ricevuto pacchetto con i controlli"<<endl;
            for (unsigned i=0; i< temp.size();i++) {

                for (map<int,double>::iterator it=commands.at(temp.at(i).identifier).command.begin(); it!=commands.at(temp.at(i).identifier).command.end();++it) 
				{
                    it->second=temp.at(i).command.at(it->first);
                }

            }
// 		if (abs(states_index.internal_map.at("AGENTE1").state.at(0))>30)
// 				break;
			if (sim_packet.time>max_loops)
                break;
        }
		graph_router.set_run(false);
    }
    catch (const char* e)
    {
        ERR("%s",e);
    }
}



simulator::~simulator()
{
	sim_packet.time=-10;
	communicator->send_broadcast(sim_packet);
	
	graph_router.join_thread();
    delete communicator;

    for (unsigned int i=0; i< map_bonus_variables.size();i++)
        bonus_expressions.at(i).~expression();

    for (unsigned int i=0;i<dynamic_module.size();i++)
        delete dynamic_module[i];

    delete f_rndom;
}

void simulator::start_sim(int max_loops)
{
	this->max_loops=max_loops;
    time=0;
    //communicator->send_broadcast(time);
	graph_router.start_thread();
    main_loop();
}


void simulator::update_bonus_variables()
{
    for (map<std::string,double>::iterator it=sim_packet.bonus_variables.begin();it!=sim_packet.bonus_variables.end();++it) {
        it->second=bonus_expressions.at(map_bonus_variables.at(it->first)).value();
    }
//Copio i risultati alla fine per avere coerenza all'interno dello stesso clock
    for (map<string,double>::iterator it=sim_packet.bonus_variables.begin();it!=sim_packet.bonus_variables.end();++it)
    {
        bonusVariables.at(map_bonus_variables_to_id.at(it->first))=it->second;
    }

}

