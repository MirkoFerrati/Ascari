#include <iostream>
#include "agent.h"
#include "logog.hpp"

int main(int argc, char **argv) {
    vector<Parsed_Agent>Agents=parse_file("rob_soc.yaml");
	agent a1(argv[1],false,Agents);
    std::cout << "Hello, world! agent" << std::endl;
    return 0;
}
