#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "logog.hpp"
#include "simulator.h"
#include "yaml-cpp/yaml.h"


using namespace std;

void initialize_communication(simulator& s)
{
	s.create_communicator(1);
}

int main(int argc, char **argv) {
    LOGOG_INITIALIZE();
    {
        logog::Cout out;
        WARN("Foo is over %d!  Current value is %d.", 3, 5 );
		simulator s;
		initialize_communication(s);
        std::cout << "Hello, world! simulator" << std::endl;
        std::ifstream fin("monsters.yaml");
    }
    LOGOG_SHUTDOWN();
    return 0;
}


