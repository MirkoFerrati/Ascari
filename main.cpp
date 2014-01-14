#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <cstdio>
#include <ctime>
#include <boost/program_options.hpp>
#include <logog.hpp>
#include "simulator/simulator.h"
#include "agent/agent.h"
#include <add_simulator_plugins.h>
#include <add_agent_plugins.h>
#include <agent_router/agent_router_parsed_world.h>
#include "communication/memory_communicator.hpp"
#include <lemon/lgf_reader.h>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <random>
/**
Profiling notes:
simulator main loop                                     74%
        -> map<std::string,agent_state_packet>::at      09.6%  maybe we can change agent identifier type with an int? At least inside simulator? How?
        -> map<int,double>::at                          05.8%
        -> map<int,double>::operator=                   04.9%   MAYBE FIXED; MAYBE MUTEX WILL CRASH THE SOFTWARE
    dynamic_getNextState                                25%
        -> map<int,double>::insert                      11% This is a copy made in order to compute the new state x(k+1)=f(x(k)) without changing x(k) during the computation...
        -> map<int,double>::at                          05.4%
    zmq_viewer_communicator
        -> send_target                                  18% EASY TO FIX (disable the call to send_target)
agent_router run plugin                                 10%
        -> logog info speed=                            3.7% Do we really need to cout all that stuff here?
create_agents                                           12%  FIXED -> 02%
        -> yaml_parser                                  1.3%
        -> graph_creator::create_graph                  10%  FIXED
                ->lemon::graphToEps                     10%  FIXED
**/


using namespace std;
lemon::SmartDigraph graph;
std::map<std::string,std::vector<int>> agents_target;
lemon::SmartDigraph::NodeMap<int> coord_x(graph), coord_y(graph);

class FormatterCustom : public logog::FormatterGCC
{
    virtual TOPIC_FLAGS GetTopicFlags( const logog::Topic &topic )
    {
        return ( logog::Formatter::GetTopicFlags( topic ) &
                 ~( TOPIC_FILE_NAME_FLAG | TOPIC_LINE_NUMBER_FLAG ));
    }
};


std::map<std::string,std::vector<int>> randomizeTargets(std::list<Parsed_Agent> agents,std::vector<int> node_ids)
{
    std::map<std::string,std::vector<int>> agents_target;
    //std::default_random_engine generator;
    unsigned seed1 = std::chrono::system_clock::now().time_since_epoch().count();
    std::mt19937 g1 (seed1);
    std::uniform_int_distribution<int> distribution(0,1000000);

for (auto agent:agents)
    {
        int index = distribution(g1)*node_ids.size()/1000000;
        int source=node_ids.at(index);
        node_ids.erase(node_ids.begin()+index);
        index = distribution(g1)*node_ids.size()/1000000;
        int target=node_ids.at(index);
        node_ids.erase(node_ids.begin()+index);
        agents_target[agent.name].push_back(source);
        agents_target[agent.name].push_back(target);
    }
    return agents_target;
}

void createRandomAgents(simulator* s,Parsed_World& world,std::map<std::string,agent*>agents)
{
}

void createSimulator(simulator* s,Parsed_World& world ,std::string filename)
{
    auto plugins=createSimulatorPlugins();

    yaml_parser parser;
for ( auto plugin:plugins )
    {
        plugin->createParserPlugin();
        parser.addPlugin ( plugin->getParserPlugin() );
    }
    world=parser.parse_file ( filename );
for ( auto plugin:plugins )
    {
        if ( plugin->isEnabled() )
        {
            if ( !plugin->createSimulatorPlugin ( s ,&world) )
            {
                ERR ( "failed to create plugin %s",plugin->getType().c_str() );
            }
            else
                s->addPlugin ( plugin->getSimulatorPlugin() );
        }
    }
}

void createAgents(std::map<std::string,agent*>& agents,Parsed_World& world,std::string filename)
{
    
    //Read the number of agents
    int num_agents=world.agents.size();
    std::string graphName=(reinterpret_cast<agent_router_parsed_world*>(world.parsed_items_from_plugins.at(AGENT_ROUTER_NAME)))->graphName;
    boost::algorithm::to_lower ( graphName );
    
    //Read the input graph from world
    lemon::SmartDigraph::ArcMap<int> length(graph);
    //TODO: we cannot use graph_creator, we need to copy the graph_creator.parse method here!!
    try
    {
        lemon::digraphReader ( graph,graphName  ). // read the directed graph into g
        nodeMap ( "coordinates_x", coord_x ).   //read the coordinates of nodes
        nodeMap ( "coordinates_y", coord_y ).   //read the coordinates of nodes
        arcMap ( "length", length ).    // read the 'capacity' arc map into cap
        run();
    }
    catch ( lemon::Exception& er )   // check if there was any error
    {
        ERR ( "parsing exception %s",er.what() );
        assert(false);
    }
    std::vector<int> node_ids;
    for (lemon::SmartDigraph::NodeIt n(graph); n != lemon::INVALID; ++n)
    {
        node_ids.push_back(graph.id(n));
    }
    int graph_node_size=node_ids.size();
    if ( !graph_node_size )
    {
        ERR ( "attenzione, impossibile creare il grafo", 0 );
        return;
    }
    //Randomize starting and ending targets for all agents
    //Save agents target in a map so that createAgents will set their targets accordingly
    agents_target=randomizeTargets(world.agents,node_ids);

    
    //Substitute the call to parserPlugin with a call to setTarget (or call setTarget after plugin was created and overwrite target list)
    //Save starting and ending positions in the log file
    for ( auto it=world.agents.begin(); it!=world.agents.end(); it++)
    {

        auto plugins=createAgentPlugins();

        yaml_parser parser;
        for ( auto plugin:plugins )
        {
            plugin->createParserPlugin();
            parser.addPlugin ( plugin->getParserPlugin());
        }
        auto world_temp = parser.parse_file ( filename );
        agent* temp=new agent(it->name,it->behavior, world_temp,true );
        agents[temp->identifier]=temp;
        for ( auto plugin:plugins )
        {
            if ( plugin->isEnabled() )
            {
                if ( !plugin->createAgentPlugin ( temp ,&world_temp) )
                {
                    ERR ( "impossibile creare il plugin %s",plugin->getType().c_str() );
                }
                else
                    temp->addPlugin ( plugin->getAgentPlugin() );
            }
        }
        if(temp->plugins.empty())
        {
            WARN("Attenzione, nessun plugin è stato caricato, l'agente resterà immobile",NULL);
        }
        reinterpret_cast<agent_router*>(temp->plugins[0])->setTargets(agents_target.at(temp->identifier));
        temp->initialize();
    }
}

int main ( int argc, char **argv )
{

    srand ( time ( NULL ) );
    LOGOG_INITIALIZE();
    std::thread exiting;

    static_zmq::context=new zmq::context_t ( 1 );
    {
        char buf[85];
        std::time_t rawtime;
        std::tm* timeinfo;
        char buffer [80];

        std::time(&rawtime);
        timeinfo = std::localtime(&rawtime);

        std::strftime(buffer,80,"%Y-%m-%d-%H-%M-%S",timeinfo);
        std::puts(buffer);

        sprintf(buf,"log_%s",buffer);
        logog::LogFile out(buf);
        logog::LogBuffer out_buffer(&out);
        FormatterCustom customFormat;
        out_buffer.SetFormatter( customFormat );
        out.SetFormatter(customFormat);
        simulator* s=new simulator();
        std::map<std::string,agent*> agents;
        boost::program_options::options_description desc;
        boost::program_options::variables_map options;
        desc.add_options()("help,h","Get help");

        std::string filename;
        int count=500; //TODO set this in the right way
        if (CONFIG.exists("FILENAME"))
        {
            filename=CONFIG.getValue("FILENAME");
            desc.add_options()("filename,f",boost::program_options::value<std::string>(&filename), "Yaml filename");
            INFO("Using %s as filename, read from config file",filename.c_str());
        }
        else if (CONFIG.exists("filename"))
        {
            filename=CONFIG.getValue("filename");
            desc.add_options()("filename,f",boost::program_options::value<std::string>(&filename), "Yaml filename");
            INFO("Using %s as filename, read from config file",filename.c_str());
        }
        else
            desc.add_options()("filename,f",boost::program_options::value<std::string>(&filename)->required(), "Yaml filename");

for (auto config_value:CONFIG.getMap())
        {
            if (config_value.first!="filename")
                desc.add_options()(config_value.first.c_str(),config_value.second.data().c_str());
        }

        try
        {
            boost::program_options::store(boost::program_options::command_line_parser(argc,argv).options(desc).run(),options);//Overwrite default file values with shell one

            if (options.count("help"))
            {
                std::cout << "Basic Command Line Parameter App" << std::endl
                << desc << std::endl;
                return 0;
            }
            boost::program_options::notify(options);
        }
        catch (boost::program_options::error& e)
        {
            std::cerr << "ERROR: " << e.what() << std::endl << std::endl;
            std::cerr << desc << std::endl;
            return 2;
        }


        /**
         * Costruire il comunicatore MEMORY ODNE
         * Costruire i plugin di simulator (come nel main di simulator) DONE
         * Costruire i plugin di agent (come nel main di agent) DONE
         * Settare i comunicatori di agent e simulator DONE
         * Avviare un thread per simulator con un limite di cicli (count)
         * Avviare N threads per agent
         * Sleep(tempomassimo)
         * Killare tutto e ripartire...come?
         */
        Parsed_World world;
        s->setPeriod(0);

        createSimulator(s,world,filename); //Costruire i plugin di simulator (come nel main di simulator)
        createAgents(agents,world,filename);//Costruire i plugin di agent (come nel main di agent)
        

        std::shared_ptr<memory_full_communicator> communicator=std::make_shared<memory_full_communicator>(agents.size());
        std::shared_ptr<agent_namespace::world_communicator_abstract>agent_communicator=communicator;

        s->set_communicator(communicator);
        s->initialize ( world );
        //Place agents in the right X,Y point of the map (ignore yaml coordinates)
        for (auto agent:agents)
        {
            s->sim_packet.state_agents.internal_map[agent.first].state.at(s->agent_states_to_index["X"])=coord_x[graph.nodeFromId(agents_target.at(agent.first).at(0))];
            s->sim_packet.state_agents.internal_map[agent.first].state.at(s->agent_states_to_index["Y"])=coord_y[graph.nodeFromId(agents_target.at(agent.first).at(0))];
        }
        //TODO: initialize_communication for the agents
        for (auto a:agents)
            a.second->set_communicator(agent_communicator);

        std::map<std::string,std::thread> threads;


        std::thread sim(&simulator::start_sim,std::ref(*s),count );

        for (auto a:agents)
        {
            threads.insert(std::make_pair(a.first,
                                          std::thread(&agent::start,std::ref(*a.second))
                                         ));
        }
        sim.join();
        exiting=std::thread ( []()
        {
            delete ( static_zmq::context );
        } );
        delete(s);

        std::cout<<"simulator distrutto, agenti running"<<std::endl;

for (auto& t:threads)
        {
            t.second.join();
        }


    }
    exiting.join();

    LOGOG_SHUTDOWN();
    return 0;
}


