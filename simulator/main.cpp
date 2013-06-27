#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>

#include "debug_constants.h"
#include "simulator.h"
#include "time.h"
#include "lemon/arg_parser.h"
#include <communication/global.h>
#include "../plugins/addplugins.h"



using namespace std;


void initialize_communication ( simulator& s )
{
    s.create_communicator ( 2 );
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
        lemon::ArgParser ap ( argc,argv );
        int count,secSleep;
        std::string filename;
        bool checkCollision=false;
	bool filename_obbl=true;
	if (CONFIG.exists("FILENAME"))
	  filename_obbl=false;
	
        ap.refOption ( "n","Number of simulator cycle",count );
        ap.refOption ( "s","Milliseconds sleep",secSleep );
        ap.refOption ( "f","Yaml filename",filename,filename_obbl );
        ap.refOption ( "check_collision","enables collision checking",checkCollision );
        ap.synonym ( "filename","f" );
        ap.synonym ( "sleep","s" );
        ap.throwOnProblems();
        try
        {
            ap.parse();
        }
        catch ( lemon::ArgParserException const& ex )
        {
            ERR ( "errore nella lettura dei parametri %s",ex.what() );
            return 0;
        }
        if (!ap.given("filename"))
	{
	  filename=CONFIG.getValue("FILENAME");
  	  INFO("Using %s as filename, read from config file",filename.c_str());
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
                    ERR ( "impossibile creare il plugin %s",plugin->getType().c_str() )
                else
                    s.addPlugin ( plugin->getSimulatorPlugin() );
            }
        }
	
        s.initialize ( world );
        initialize_communication ( s );
        if ( ap.given ( "s" ) )
            s.setSleep ( secSleep );
        if ( ap.given ( "check_collision" ) )
            s.setCheckCollision ( checkCollision );
        if ( ap.given ( "n" ) )
            s.start_sim ( count );
        else
            s.start_sim();

        std::cout << "Hello, world! simulator" << std::endl;
        exiting=std::thread ( []()
        {
            delete ( static_zmq::context );
        } );

    }
    exiting.join();

    LOGOG_SHUTDOWN();
    return 0;
}


