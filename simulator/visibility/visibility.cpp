#include "visibility.h"
#include "circle.hpp"
#include "map2d.h"
#include <boost/regex.hpp>

std::shared_ptr< visibleArea > visibleArea::createVisibilityFromParsedVisibleArea ( std::string parsed_visibility, index_map map_statename_to_id )
{
  
    
     const boost::regex circle_pattern("CIRCLE\\(\\w+,\\w+,(\\d+\\.{0,1}\\d*)\\)");
     const boost::regex map_pattern("MAP\\(\\w+,\\w+,\\w+)");
     
    if(boost::regex_match(parsed_visibility.begin(),parsed_visibility.end(), circle_pattern)==true){ 
      return std::make_shared<circle>( parsed_visibility,map_statename_to_id );  
    }
     
    if(boost::regex_match(parsed_visibility.begin(),parsed_visibility.end(), map_pattern)==true){
      return std::make_shared<map2d>( parsed_visibility,map_statename_to_id );  
    }
    
    ERR ( "VISIBLE AREA INCORRECTLY SPECIFIED",NULL );
    throw "VISIBLE AREA INCORRECTLY SPECIFIED";
    
  
}






