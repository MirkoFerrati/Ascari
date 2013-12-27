#include <time.h>
#include <iostream>
#include <string>
#include <thread>
#include <boost/program_options.hpp>
#include "agent.h"
#include "logog.hpp"
#include "../shared/communication/global.h"
#include "../plugins/add_agent_plugins.h"
#include "../shared/communication/global.h"
#include <define.h>

int main ( int argc, char** argv )
{
    srand ( time ( NULL ) );
    LOGOG_INITIALIZE();
    std::thread exiting;

    static_zmq::context=new zmq::context_t ( 1 );

    {
        logog::Cout out;
        Parsed_World world;
        boost::program_options::options_description desc;
        boost::program_options::variables_map options;
        desc.add_options()("help,h","Get help");
        
        //boost::program_options::variables_map& options=CONFIG.file_map; //we already parsed the file, let's use the values found there
        std::string agent_name;
        std::string filename;
              
        
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
        
        if (CONFIG.exists("AGENT"))
        {
            agent_name=CONFIG.getValue("AGENT");
            desc.add_options()( "agent,a",boost::program_options::value<std::string>(&agent_name), "Agent name");
        }
        else if (CONFIG.exists("agent"))
        {
            agent_name=CONFIG.getValue("agent");            
            desc.add_options()( "agent,a",boost::program_options::value<std::string>(&agent_name), "Agent name");
        }
        else
            desc.add_options()( "agent,a",boost::program_options::value<std::string>(&agent_name)->required(), "Agent name");
            
        for (auto config_value:CONFIG.getMap())
        {
            if ((config_value.first!="agent")&&(config_value.first!="filename"))
            desc.add_options()(config_value.first.c_str(),boost::program_options::value<std::string>()->default_value(config_value.second.data().c_str()),"");
        }
        
        
        boost::program_options::positional_options_description p;
        p.add("agent",-1);
        
        try
        {
            boost::program_options::store(boost::program_options::command_line_parser(argc,argv).options(desc).positional(p).run(),options);//Overwrite default file values with shell one
            
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
        std::cout << "  Value of -a: " << agent_name << std::endl;
        std::cout<< "   Value of -f: " <<filename <<std::endl;
        
       /* if (ap.given("filename"))
	  std::cout << "  Value of -f: " << filename << std::endl;
        else
	{
	  filename=CONFIG.getValue("FILENAME");
  	  INFO("Using %s as filename, read from config file",filename.c_str());
	}*/
	
       for (auto config_value:options)
       {
           CONFIG.getMap().put(config_value.first,config_value.second.as<std::string>());
       }
       
        auto plugins=createAgentPlugins();

        yaml_parser parser;
	for ( auto plugin:plugins )
	{
            parser.addPlugin ( plugin->getParserPlugin());
	}
        world = parser.parse_file ( filename );
        int myAgent = -1;
	int i=0;
        for ( auto it=world.agents.begin();it!=world.agents.end();it++,i++ )
            if ( it->name.compare ( agent_name ) == 0 )
                myAgent = i;
        if ( myAgent == -1 )
        {
            ERR ( "agent %s not found in configuration file, please check for agent names", agent_name.c_str() );
            throw "agent name not found in configuration file, please check for agent names";
        }

        for ( auto it=world.agents.begin();it!=world.agents.end(); )
        {
            if ( it->name.compare ( agent_name )!=0)
            {
                it=world.agents.erase(it);
            }
            else
                it++;
        }
        
        s_catch_signals();
        agent a1 ( world );
	for ( auto plugin:plugins )
        {
            if ( plugin->isEnabled() )
            {
                if ( !plugin->createAgentPlugin ( &a1 ,&world) )
		{ERR ( "impossibile creare il plugin %s",plugin->getType().c_str() );}
                else
                    a1.addPlugin ( plugin->getAgentPlugin() );
            }
        }
        if(a1.plugins.empty())
	{
	  WARN("Attenzione, nessun plugin è stato caricato, l'agente resterà immobile",NULL);
	}
        a1.initialize();
        a1.start();
        std::cout << agent_name <<" e' terminato" << std::endl;
        exiting=std::thread ( []()
        {
            delete ( static_zmq::context );
        } );

    }
    LOGOG_SHUTDOWN();
    exiting.join();

    return 0;
}
