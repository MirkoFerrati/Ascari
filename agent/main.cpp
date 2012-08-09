#include <iostream>
#include "agent.h"
#include "logog.hpp"
#include "typedefs.h"
#include "debug_constants.h"
#include <time.h>



int main(int argc, char **argv) {
    srand(time(NULL));
    LOGOG_INITIALIZE();
    {
		Parsed_World World;
        logog::Cout out;
		if (argc<2)
		{
			ERR("%s","please insert the name of the agent");
			return 1;
		}
		if (argc>2)
		{
			World=parse_file(argv[2]);
		}
		else
		{
			World=parse_file(FILENAME);
		}
        std::string name=argv[1];
        agent a1(name,false,World);
        std::cout << "Hello, world! agent" << std::endl;
    }
    return 0;
}
