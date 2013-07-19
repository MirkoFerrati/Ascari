#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <thread>
#include "debug_constants.h"
#include "fake_simulator.h"
#include "time.h"
#include "lemon/arg_parser.h"
#include <communication/global.h>
#include <logog.hpp>
#include <define.h>
#include <yaml_parser.h>

using namespace std;


void initialize_communication ( fake_simulator& s,const Parsed_World& world )
{
    s.create_communicator ( 0,world );
}



int main ( int argc, char **argv )
{

    srand ( time ( NULL ) );
    LOGOG_INITIALIZE();
    std::thread exiting;
    static_zmq::context=new zmq::context_t ( 1 );
    {
        logog::Cout out;
        fake_simulator s;
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
        if (!ap.given("f"))
	{
	  filename=CONFIG.getValue("FILENAME");
  	  INFO("Using %s as filename, read from config file",filename.c_str());
	}

        yaml_parser parser;
	world = parser.parse_file ( filename );
     
	
        s.initialize ( world );
        initialize_communication ( s,world );
        if ( ap.given ( "s" ) )
            s.setPeriod ( secSleep );
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


