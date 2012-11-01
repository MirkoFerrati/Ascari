#include <time.h>
#include <iostream>
#include <string>
#include "agent.h"
#include "../shared/logog/include/logog.hpp"
#include "../shared/typedefs.h"

#include "lemon/arg_parser.h"



int main(int argc, char **argv) {
    srand(time(NULL));
    LOGOG_INITIALIZE();
    {
		cout<<argc<<argv[0]<<argv[1]<<argv[2]<<endl;
		logog::Cout out;
		Parsed_World World;
		lemon::ArgParser ap(argc,argv);
		int count;
		string agent_name;
		std::string filename;
		ap.refOption("n","Number of simulator cycle",count);
		ap.refOption("a","Agent name",agent_name,true);		
		ap.refOption("f","Yaml filename",filename,true);
		ap.synonym("filename","f");
		ap.synonym("agent","a");
		ap.throwOnProblems();
		try{
			ap.parse();
		}
		catch (lemon::ArgParserException ex){
			ERR("errore nella lettura dei parametri %s",ex.reason());
			return 0;
		}
		std::cout << "Parameters of '" << ap.commandName() << "':\n";
		std::cout << "  Value of -a: " << agent_name << std::endl;
		std::cout << "  Value of -f: " << filename << std::endl;
/*		if (ap.given("s"))
			s.setSleep(secSleep);
		if (ap.given("n"))
			s.start_sim(count);
		else
			s.start_sim();
*/		
		World = parse_file(filename); 
        agent a1(agent_name,false,World);
		a1.start();
        std::cout << "Hello, world! agent" << std::endl;
	}
	LOGOG_SHUTDOWN();
	return 0;
}
