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
using namespace std;



void createSimulator(simulator& s, Parsed_World& world, std::string filename)
{
    auto plugins=createSimulatorPlugins();
    
    yaml_parser parser;
    for ( auto plugin:plugins )
    {
        plugin->createParserPlugin();
        parser.addPlugin ( plugin->getParserPlugin() );
    }
    world = parser.parse_file ( filename );
    
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
            parser.addPlugin ( plugin->getParserPlugin());
        }
        world = parser.parse_file ( filename );
        
        agent* temp=new agent( world );
        agents[temp->identifier]=temp;
        for ( auto plugin:plugins )
        {
            if ( plugin->isEnabled() )
            {
                if ( !plugin->createAgentPlugin ( temp ,&world) )
                {ERR ( "impossibile creare il plugin %s",plugin->getType().c_str() );}
                else
                    temp->addPlugin ( plugin->getAgentPlugin() );
            }
        }
        if(temp->plugins.empty())
        {
            WARN("Attenzione, nessun plugin è stato caricato, l'agente resterà immobile",NULL);
        }
    }
    
}

int main ( int argc, char **argv )
{
    
    srand ( time ( NULL ) );
    LOGOG_INITIALIZE();
    {
        logog::Cout out;
        simulator s;
        std::map<std::string,agent*> agents;
        Parsed_World world;
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
         * Costruire il comunicatore MEMORY
         * Costruire i plugin di simulator (come nel main di simulator) DONE
         * Costruire i plugin di agent (come nel main di agent) DONE
         * Settare i comunicatori di agent e simulator
         * Avviare un thread per simulator con un limite di cicli (count)
         * Avviare N threads per agent
         * Sleep(tempomassimo)
         * Killare tutto e ripartire...come?
         */
        
        createSimulator(s,world,filename); //Costruire i plugin di simulator (come nel main di simulator)
        
        createAgents(agents,world,filename);//Costruire i plugin di agent (come nel main di agent)
        
        
        //TODO: initialize_communication for the simulator
        
        s.initialize ( world );
        
        s.start_sim ( count );//TODO: start this in a thread
        
        //TODO: initialize_communication for the agents
        
        for (auto a:agents)
            a.second->initialize();
        for (auto a:agents)
            a.second->start();//TODO: start in a thread
        
        
     
    }
    LOGOG_SHUTDOWN();
    return 0;
}


