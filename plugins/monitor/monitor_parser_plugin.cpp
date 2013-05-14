#include "monitor_parser_plugin.h"
#include <logog.hpp>

std::shared_ptr< abstract_parsed_world_plugin > monitor_parser_plugin::parseWorld ( const YAML::Node& node )
{

}

std::shared_ptr< abstract_parsed_agent_plugin > monitor_parser_plugin::parseAgent ( const YAML::Node& node )
{
 ag.monitoring=false;

    if ( node.FindValue ( "MONITORING" ) )
    {
        int tmp_mon=0;
        node["MONITORING"]>>tmp_mon;
        if ( tmp_mon==1 )
        {
            ag.monitoring=true;
            if ( node.FindValue ( "KNOWN_BEHAVIORS" ) )
            {
                node["KNOWN_BEHAVIORS"]>>ag.known_behaviors;
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
            ag.monitoring=false;
            if ( tmp_mon!=0 )
            {
                WARN ( "UNRECOGNIZED VALUE FOR MONITORING. SET TO FALSE",NULL );
            }
        }


    }
}

