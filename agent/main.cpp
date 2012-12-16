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
		std::cout<<argc<<argv[0]<<argv[1]<<argv[2]<<std::endl;
		logog::Cout out;
		Parsed_World world;
		lemon::ArgParser ap(argc,argv);
		int count;
		std::string agent_name;
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
		catch (lemon::ArgParserException const& ex){
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
		world = parse_file(filename); 
		int myAgent=-1;
		for (unsigned int i =0;i<world.agents.size();i++)
			if (world.agents.at(i).name.compare(agent_name)==0)
				myAgent=i;
			if (myAgent==-1)
			{
				ERR("agent %s not found in configuration file, please check for agent names",name.c_str());
				throw "agent name not found in configuration file, please check for agent names";
			}
			
        agent a1(myAgent,false,world);
		a1.start();
        std::cout << "Hello, world! agent" << std::endl;
	}
	LOGOG_SHUTDOWN();
	return 0;
}
