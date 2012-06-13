#include <iostream>
#include "agent.h"
#include "logog.hpp"

int main(int argc, char **argv) {
    std::vector<Parsed_Agent>Agents=parse_file("esempio1.yaml");
	std::string name=argv[1];
	agent a1(name,false,Agents);
    std::cout << "Hello, world! agent" << std::endl;
    return 0;
}
