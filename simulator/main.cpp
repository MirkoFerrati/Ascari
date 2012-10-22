#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include "typedefs.h"
#include "debug_constants.h"
#include "simulator.h"
#include "time.h"
#include "lemon/arg_parser.h"
using namespace std;


void initialize_communication(simulator& s)
{
    s.create_communicator(1);
}



int main(int argc, char **argv) {
  
	srand(time(NULL));
    LOGOG_INITIALIZE();	
    {
        logog::Cout out;
        WARN("Foo is over %d!  Current value is %d.", 3, 5 );
        simulator s;
		Parsed_World World;
		lemon::ArgParser ap(argc,argv);
		int count,secSleep;
		ap.refOption("n","Number of simulator cycle",count);
		ap.refOption("s","Milliseconds sleep",secSleep);
		std::string filename;
		ap.refOption("f","Yaml filename",filename);
		ap.synonym("filename","f");
		ap.synonym("sleep","s");
		ap.throwOnProblems();
		try{
			ap.parse();
		}
		catch (lemon::ArgParserException ex){
			ERR("errore nella lettura dei parametri %s",ex.reason());
			return 0;
		}
		World=parse_file(filename);
 	cout<<World;
	s.initialize(World);
	initialize_communication(s);
	if (ap.given("s"))
		s.setSleep(secSleep);
	if (ap.given("n"))
		s.start_sim(count);
	else
		s.start_sim();
    
	std::cout << "Hello, world! simulator" << std::endl;
	
    }
    LOGOG_SHUTDOWN();
    return 0;
}


