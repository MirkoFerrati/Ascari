#include "define.h"
#include <mutex>

configuration_reader CONFIG;

std::map<std::string,bool> collisions;
std::map<std::string,bool> should_check;
std::mutex collisions_mutex, should_check_mutex;