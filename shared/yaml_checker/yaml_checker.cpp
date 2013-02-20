#include "yaml_checker.h"
#include "../simulator/simulator.h"
#include "../agent/agent.h"
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
  
   for (unsigned int i = 0; i < World.agents.size(); i++)
   {
        agent a1 (i, World,true);
	if (!a1.initialized) 
	{
	  failed=true;
	}
   }
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
