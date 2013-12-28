#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <boost/program_options.hpp>
#include <logog.hpp>
#include "simulator/simulator.h"
#include "agent/agent.h"
#include <add_simulator_plugins.h>
#include <add_agent_plugins.h>
#include "communication/memory_communicator.hpp"

using namespace std;



void createSimulator(simulator& s,Parsed_World& world ,std::string filename)
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
            if ( !plugin->createSimulatorPlugin ( &s ,&world) )
            {
                ERR ( "failed to create plugin %s",plugin->getType().c_str() );
            }
            else
                s.addPlugin ( plugin->getSimulatorPlugin() );
        }
    }
}

void createAgents(std::map<std::string,agent*>& agents,Parsed_World& world,std::string filename)
{
    for ( auto it=world.agents.begin();it!=world.agents.end();it++)
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
                {ERR ( "impossibile creare il plugin %s",plugin->getType().c_str() );}
                else
                    temp->addPlugin ( plugin->getAgentPlugin() );
            }
        }
        if(temp->plugins.empty())
        {
            WARN("Attenzione, nessun plugin è stato caricato, l'agente resterà immobile",NULL);
        }
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
        logog::Cout out;
        simulator s;
        std::map<std::string,agent*> agents;
        boost::program_options::options_description desc;
        boost::program_options::variables_map options;
        desc.add_options()("help,h","Get help");
        
        std::string filename;
        int count=1000; //TODO set this in the right way
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
        createSimulator(s,world,filename); //Costruire i plugin di simulator (come nel main di simulator)
        s.setPeriod(0);
        createAgents(agents,world,filename);//Costruire i plugin di agent (come nel main di agent)
        
        std::shared_ptr<memory_full_communicator> communicator=std::make_shared<memory_full_communicator>(agents.size());
        std::shared_ptr<agent_namespace::world_communicator_abstract>agent_communicator=communicator;
        
        //TODO: initialize_communication for the simulator
        s.set_communicator(communicator);
        s.initialize ( world );

        
        //TODO: initialize_communication for the agents
        for (auto a:agents)
            a.second->set_communicator(agent_communicator);
      
        std::map<std::string,std::thread> threads;
        
        
        std::thread sim(&simulator::start_sim,std::ref(s),count );//TODO: start this in a thread
        
        for (auto a:agents)
        {
            threads.insert(std::make_pair(a.first,
                                          std::thread(&agent::start,std::ref(*a.second))
                                         ));//TODO: start in a thread
        }
        sim.join();
        
        exiting=std::thread ( []()
        {
            delete ( static_zmq::context );
        } );
        
    }
    LOGOG_SHUTDOWN();
    return 0;
}


