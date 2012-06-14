#include <iostream>
#include "agent.h"
#include "logog.hpp"
#include "typedefs.h"
#include "debug_constants.h"

int main(int argc, char **argv) {
    std::vector<Parsed_Agent>Agents=parse_file(FILENAME);
	std::string name=argv[1];
	agent a1(name,false,Agents);
    std::cout << "Hello, world! agent" << std::endl;
    return 0;
}
