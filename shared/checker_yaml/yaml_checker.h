#ifndef YAML_CHECKER_H
#define YAML_CHECKER_H

#include <string>

#include "yaml_parser.h"

class Yaml_checker
{

public:
    Yaml_checker();
    void checkYaml(std::string filename);
    bool checkOk();
private:
    bool failed;
};

#endif // YAML_CHECKER_H
