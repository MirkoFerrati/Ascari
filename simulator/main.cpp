#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include "typedefs.h"

#include "simulator.h"

  
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
	
        
        vector<Parsed_Agent>Agents=parse_file("esempio1.yaml");
	//cout<<Agents;
	s.initialize(Agents);
	s.start_sim();
        std::cout << "Hello, world! simulator" << std::endl;
	
    }
    LOGOG_SHUTDOWN();
    return 0;
}


