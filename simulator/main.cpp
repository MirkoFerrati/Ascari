#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include "typedefs.h"

#include "simulator.h"

  



void initialize_communication(simulator& s)
{
    s.create_communicator(1);
}




int main(int argc, char **argv) {
  
    LOGOG_INITIALIZE();
    {
        logog::Cout out;
        WARN("Foo is over %d!  Current value is %d.", 3, 5 );
        simulator s(argc, argv);
        initialize_communication(s);
        //s.start_sim();
        std::cout << "Hello, world! simulator" << std::endl;
	
        vector<Parsed_Agent>Agents=parse_file("rob_soc.yaml");
	cout<<Agents;
    }
    LOGOG_SHUTDOWN();
    return 0;
}


