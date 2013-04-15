#include "visibility.h"
#include "circle.hpp"
#include <boost/regex.hpp>

std::shared_ptr< visibleArea > visibleArea::createVisibilityFromParsedVisibleArea ( std::string parsed_visibility, index_map map_statename_to_id )
{
  
    
     const boost::regex circle_pattern("CIRCLE\\(\\w+,\\w+,(\\d+\\.{0,1}\\d*)\\)");
     
     
    if(boost::regex_match(parsed_visibility.begin(),parsed_visibility.end(), circle_pattern)==true){
     
	return std::make_shared<circle>( parsed_visibility,map_statename_to_id );  
      
    }
    
    
    ERR ( "VISIBLE AREA INCORRECTLY SPECIFIED",NULL );
    throw "VISIBLE AREA INCORRECTLY SPECIFIED";
    
  
}





void polygon::create()
{

}


polygon::polygon()
{
pol.push_back(wykobi::make_point(5.0,4.0));
pol.push_back(wykobi::make_point(7.0,9.0));
pol.push_back(wykobi::make_point(9.0,7.0));


}




