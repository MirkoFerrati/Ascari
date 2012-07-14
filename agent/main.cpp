#include <iostream>
#include "agent.h"
#include "logog.hpp"
#include "typedefs.h"
#include "debug_constants.h"
#include <time.h>
#include <lgf_parser.h>



int main(int argc, char **argv) {
    srand(time(NULL));
    LOGOG_INITIALIZE();
    {
        logog::Cout out;
        Parsed_World World=parse_file(FILENAME);
		Parsed_Graph graph;
		parseGraph(GRAPHNAME,graph);
        std::string name=argv[1];
        agent a1(name,false,World);
        std::cout << "Hello, world! agent" << std::endl;
    }
    return 0;
}
