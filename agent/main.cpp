#include <iostream>
#include "agent.h"
#include "logog.hpp"

int main(int argc, char **argv) {
    agent a1(argv[1]);
    std::cout << "Hello, world! agent" << std::endl;
    return 0;
}
