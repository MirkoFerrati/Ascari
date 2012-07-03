#include <iostream>
#include "agent.h"
#include "logog.hpp"
#include "typedefs.h"
#include "debug_constants.h"
#include <time.h>
#include "geometry.hpp"
int main(int argc, char **argv) {
	srand(time(NULL));
	LOGOG_INITIALIZE();
	{
		
	model::d2::point_xy<int> p1(1, 1), p2(2, 2);
std::cout << "Distance p1-p2 is: " << distance(p1, p2) << std::endl;

	logog::Cout out;
    Parsed_World World=parse_file(FILENAME);
	std::string name=argv[1];
	agent a1(name,false,World);
    std::cout << "Hello, world! agent" << std::endl;
	}
	return 0;
}
