#include "monitor_parser_plugin.h"
#include "monitor_parsed_agent.h"
#include <logog.hpp>

#include <yaml-cpp/yaml.h>

bool monitor_parser_plugin::parseWorld ( const YAML::Node& , abstract_parsed_world_plugin* )
{
return true;
}

bool monitor_parser_plugin::parseAgent ( const YAML::Node& node, abstract_parsed_agent_plugin* agent)
{
  assert(agent==0);
  agent=new monitor_parsed_agent();
  auto ag=reinterpret_cast<monitor_parsed_agent*>(agent);
    ag->monitoring=false;

    if ( node.FindValue ( "MONITORING" ) )
    {
        int tmp_mon=0;
        node["MONITORING"]>>tmp_mon;
        if ( tmp_mon==1 )
        {
            ag->monitoring=true;
            if ( node.FindValue ( "KNOWN_BEHAVIORS" ) )
            {
                node["KNOWN_BEHAVIORS"]>>ag->known_behaviors;
            }
            else
            {

                ERR ( "NO KNOWN BEHAVIORS SPECIFIED FOR IDENTIFIER MODULE",NULL );
                return false;
//                 throw "NO KNOWN BEHAVIORS SPECIFIED FOR IDENTIFIER MODULE";
            }
        }
        else
        {
            ag->monitoring=false;
            if ( tmp_mon!=0 )
            {
                WARN ( "UNRECOGNIZED VALUE FOR MONITORING. SET TO FALSE",NULL );
            }
        }


    }
    return true;
}

