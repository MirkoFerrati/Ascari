#ifndef VISIBILITY
#define VISIBILITY

#include <typedefs.h>
#include <exprtk.hpp>
#include <iostream>
#include "logog.hpp"
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <fstream>

#define max_visibility_radius 500

class visibleArea
{
  public:
    visibleArea()
    {
    };

    virtual bool isVisible ( const agent_state& me,const agent_state& other )=0;

    virtual ~visibleArea()
    {
    };
    
    static std::shared_ptr<visibleArea> createVisibilityFromParsedVisibleArea ( std::string parsed_visibility, index_map map_statename_to_id );

    
};

class circle:public visibleArea
{
    double radius;
    int first_axis;
    int second_axis;
public:
    bool isVisible (const agent_state& me,const agent_state& other );


  circle(std::string& vis, const index_map& map_statename);
  
private:
    void create ( std::string& vis, const index_map& map_statename )
    {
        std::vector<std::string> result;

        std::string line;

        std::stringstream  data ( vis );

        while ( std::getline ( data,line,',' ) )
        {
            result.push_back ( line );
        }

        if ( result.size() !=3 )
        {
            ERR ( "WRONG NUMBER OF PARAMETERS SPECIFIED",NULL );
            throw "WRONG NUMBER OF PARAMETERS SPECIFIED";
        }



        if ( map_statename.find ( result.at ( 0 ) ) ==map_statename.end() )
        {
            ERR ( "UNKNOWN SPECIFIED FIRST AXIS",NULL );
            throw "UNKNOWN SPECIFIED FIRST AXIS";
        }


        if ( map_statename.find ( result.at ( 1 ) ) ==map_statename.end() )
        {
            ERR ( "UNKNOWN SPECIFIED SECOND AXIS",NULL );
            throw "UNKNOWN SPECIFIED SECOND AXIS";
        }

        if ( atof ( result.at ( 2 ).c_str() ) <=0 || atof ( result.at ( 2 ).c_str() ) >max_visibility_radius )
        {
            ERR ( "INVALID VALUE FOR RADIUS",NULL );
            throw "INVALID VALUE FOR RADIUS";
        }

        first_axis=map_statename.at ( result.at ( 0 ) );
        second_axis=map_statename.at ( result.at ( 1 ) );
        radius=atof ( result.at ( 2 ).c_str() ) ;
	

    }



};





#endif //VISIBILITY