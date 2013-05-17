#include <time.h>
#include <iostream>
#include <string>
#include <thread>
#include "agent.h"
#include "../shared/logog/include/logog.hpp"
#include "../shared/communication/global.h"
#include "lemon/arg_parser.h"
#include "../plugins/addplugins.h"
#include "../shared/communication/global.h"

int main ( int argc, char** argv )
{
    srand ( time ( NULL ) );
    LOGOG_INITIALIZE();
    std::thread exiting;

    static_zmq::context=new zmq::context_t ( 1 );

    {
        std::cout << argc << argv[0] << argv[1] << argv[2] << std::endl;
        logog::Cout out;
        Parsed_World world;
        lemon::ArgParser ap ( argc, argv );
        int count;
        std::string agent_name;
        std::string filename;
        ap.refOption ( "n", "Number of simulator cycle", count );
        ap.refOption ( "a", "Agent name", agent_name, true );
        ap.refOption ( "f", "Yaml filename", filename, true );
        ap.synonym ( "filename", "f" );
        ap.synonym ( "agent", "a" );
        ap.throwOnProblems();
        try
        {
            ap.parse();
        }
        catch ( lemon::ArgParserException const& ex )
        {
            ERR ( "errore nella lettura dei parametri %s", ex.reason() );
            return 0;
        }
        std::cout << "Parameters of '" << ap.commandName() << "':\n";
        std::cout << "  Value of -a: " << agent_name << std::endl;
        std::cout << "  Value of -f: " << filename << std::endl;

        auto plugins=createPlugins();

        yaml_parser parser;
	for ( auto plugin:plugins )
            parser.addPlugin ( plugin->getParserPlugin() );
        world = parser.parse_file ( filename );
        int myAgent = -1;
	int i=0;
        for ( auto it=world.agents.begin();it!=world.agents.end();i++ )
            if ( it->name.compare ( agent_name ) == 0 )
                myAgent = i;
        if ( myAgent == -1 )
        {
            ERR ( "agent %s not found in configuration file, please check for agent names", agent_name.c_str() );
            throw "agent name not found in configuration file, please check for agent names";
        }


        //TODO(Simone): questo codice NON DEVE STARE QUA
//         std::vector<std::string>tmp_erase_behavior;
//         if ( world.agents.at ( myAgent ).known_behaviors.size() !=0 )
//         {
//             for ( auto behavior=world.behaviors.begin(); behavior!=world.behaviors.end(); behavior++ )
//             {
//                 if ( ! ( find ( world.agents.at ( myAgent ).known_behaviors.begin(),world.agents.at ( myAgent ).known_behaviors.end(),behavior->first ) !=world.agents.at ( myAgent ).known_behaviors.end() ) )
//                     tmp_erase_behavior.push_back ( behavior->first );
//             }
// 
//             for ( unsigned ii=0; ii< tmp_erase_behavior.size(); ii++ )
//             {
//                 world.behaviors.erase ( tmp_erase_behavior.at ( ii ) );
//             }
//         }

	    for ( auto it=world.agents.begin();it!=world.agents.end(); )
            if ( it->name.compare ( agent_name )!=0)
	    {
	      it=world.agents.erase(it);
	    }
	    else
	      it++;

        s_catch_signals();
        agent a1 ( world );
	for ( auto plugin:plugins )
        {
            if ( plugin->getParserPlugin()->enabled )
            {
                if ( !plugin->createAgentPlugin ( &a1 ,&world) )
		                    ERR ( "impossibile creare il plugin %s",plugin->getType().c_str() )
                else
                    a1.addPlugin ( plugin->getAgentPlugin() );
            }
        }
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
