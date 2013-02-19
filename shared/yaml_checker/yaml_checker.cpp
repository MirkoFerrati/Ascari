#include "yaml_checker.h"
#include <simulator/simulator.h>

Yaml_checker::Yaml_checker()
{

}

void Yaml_checker::checkYaml (std::string filename)
{
  failed=false;
  Parsed_World World;
  try
  {
  World=parse_file(filename);
  simulator s;
  s.initialize(World);
  }catch (std::exception& ex)
  {
    failed=true;
    std::cout<<ex.what()<<std::endl;
  }
}

bool Yaml_checker::checkOk()
{
  return !failed;
}
