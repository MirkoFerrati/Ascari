#ifndef CIRCLE_HPP
#define CIRCLE_HPP


#include "visibility.h" 


#define max_circle_visibility_radius 500

class circle:public visibleArea
{
    double radius;
    int first_axis;
    int second_axis;
public:
    circle(std::string& vis, const index_map& map_statename){
      
      create(vis,map_statename);
      
    }
    
     bool isVisible (const agent_state& me,const agent_state& other ){
//   std::cout<<"x="<<me.at ( first_axis )<<" y="<<me.at ( second_axis )<<" other x="<<other.at ( first_axis )<<" other y="<<other.at ( second_axis );
      bool temp= pow ( me.at ( first_axis )-other.at ( first_axis ),2 ) +pow ( me.at ( second_axis )-other.at ( second_axis ),2 ) <pow ( radius,2 );
//    std::cout<<"visible="<<temp<<std::endl;
   return temp;
  }
  
private:
    void create ( std::string& vis, const index_map& map_statename )
    {
      
      
	std::string vis1= vis.substr(strlen("CIRCLE")+1,strlen(vis.c_str())-strlen("CIRCLE")-2);
    
        std::vector<std::string> result;

        std::string line;

        std::stringstream  data ( vis1 );

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

        if ( atof ( result.at ( 2 ).c_str() ) <=0 || atof ( result.at ( 2 ).c_str() ) >max_circle_visibility_radius )
        {
            ERR ( "INVALID VALUE FOR RADIUS",NULL );
            throw "INVALID VALUE FOR RADIUS";
        }

        first_axis=map_statename.at ( result.at ( 0 ) );
        second_axis=map_statename.at ( result.at ( 1 ) );
        radius=atof ( result.at ( 2 ).c_str() ) ;
	

    }



};


#endif //CIRCLE_HPP
