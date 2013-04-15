#include "visibility.h"
#include <boost/regex.hpp>

std::shared_ptr< visibleArea > visibleArea::createVisibilityFromParsedVisibleArea ( std::string parsed_visibility, index_map map_statename_to_id )
{
  
    
     const boost::regex pattern("\\w+\\(\\w+,\\w+,(\\d+\\.{0,1}\\d*)\\)");
     
     
    if(boost::regex_match(parsed_visibility.begin(),parsed_visibility.end(), pattern)==false){
     
	  ERR ( "VISIBLE AREA INCORRECTLY SPECIFIED",NULL );
	  throw "VISIBLE AREA INCORRECTLY SPECIFIED";
      
    }
    
    
  
    std::vector<std::string> result;
    std::stringstream  data ( parsed_visibility );
    std::string line;
    while ( std::getline ( data,line,'(' ) )
    {
        result.push_back ( line );
    }
    if ( result.at ( 0 ) =="CIRCLE" )
    {
       return std::make_shared<circle>( result.at ( 1 ),map_statename_to_id );
    }
    else
    {
        ERR ( "UNKNOWN SPECIFIED VISIBLE AREA",NULL );
         throw "UNKNOWN SPECIFIED VISIBLE AREA";
    }
}



circle::circle ( std::string& vis, const index_map& map_statename )
{
create(vis,map_statename);
}


bool circle::isVisible ( const agent_state& me, const agent_state& other )
{
//   std::cout<<"x="<<me.at ( first_axis )<<" y="<<me.at ( second_axis )<<" other x="<<other.at ( first_axis )<<" other y="<<other.at ( second_axis );
      bool temp= pow ( me.at ( first_axis )-other.at ( first_axis ),2 ) +pow ( me.at ( second_axis )-other.at ( second_axis ),2 ) <pow ( radius,2 );
//    std::cout<<"visible="<<temp<<std::endl;
   return temp;
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




