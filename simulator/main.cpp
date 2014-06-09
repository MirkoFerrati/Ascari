#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <boost/program_options.hpp>
#include "debug_constants.h"
#include "simulator.h"
#include "time.h"
#include <communication/global.h>
#include "../plugins/addplugins.h"



using namespace std;


void initialize_communication ( simulator& s,const Parsed_World& world, communicator_types type=communicator_types::REAL_TCP )
{
    s.create_communicator ( type,world );
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
        Parsed_World world;
        boost::program_options::options_description desc;
        boost::program_options::variables_map options;
        desc.add_options()("help,h","Get help");
        
        int count,secSleep;
        std::string filename;
        bool checkCollision=false;
	
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
        
            
        desc.add_options()( "cycles,n",boost::program_options::value<int>(&count),"Number of simulator cycles")
        ( "sleep,s",boost::program_options::value<int>(&secSleep),"Milliseconds sleep")
        ( "check_collision,c",boost::program_options::value<bool>(&checkCollision),"enables collision checking");
        
        
        
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
        
        
        auto plugins=createPlugins();

        yaml_parser parser;
	for ( auto plugin:plugins )
	{
	    plugin->createParserPlugin();
            parser.addPlugin ( plugin->getParserPlugin() );
	}
	world = parser.parse_file ( filename );
     
	for ( auto plugin:plugins )
        {
            if ( plugin->getParserPlugin()->isEnabled() )
            {
                if ( !plugin->createSimulatorPlugin ( &s ,&world) )
		  {ERR ( "failed to create plugin %s",plugin->getType().c_str() );}
                else
                    s.addPlugin ( plugin->getSimulatorPlugin() );
            }
        }
        
	initialize_communication ( s,world );
        s.initialize ( world );
        
        if ( options.count( "sleep" ) )
            s.setPeriod ( secSleep );
        if ( options.count ( "check_collision" ) )
            s.setCheckCollision ( checkCollision );
        if ( options.count ( "cycles" ) )
            s.start_sim ( count );
        else
            s.start_sim();
        
        exiting=std::thread ( []()
        {
            delete ( static_zmq::context );
        } );

    }
    exiting.join();

    LOGOG_SHUTDOWN();
    return 0;
}


