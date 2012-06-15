#include <iostream>
#include "agent.h"
#include "logog.hpp"
#include "typedefs.h"
#include "debug_constants.h"

int main(int argc, char **argv) {
    Parsed_World World=parse_file(FILENAME);
	std::string name=argv[1];
	agent a1(name,false,World.agents);
    std::cout << "Hello, world! agent" << std::endl;
    return 0;
}
