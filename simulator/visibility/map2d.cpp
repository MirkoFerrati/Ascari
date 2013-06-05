#include "map2d.h"

#include <istream>

map2d::map2d ( std::string& visibility, const index_map& map_statename )
{
    create ( visibility, map_statename );
}

map2d::map2d( std::string fil )
{
  openfile ( fil );
}


bool map2d::isVisible ( const agent_state& me,const agent_state& other )
{
//   std::cout<<"x="<<me.at ( first_axis )<<" y="<<me.at ( second_axis )<<" other x="<<other.at ( first_axis )<<" other y="<<other.at ( second_axis );
    wykobi::segment<double,2> temp;
    temp[0]=wykobi::make_point ( me.at ( first_axis ),me.at ( second_axis ) );
    temp[1]=wykobi::make_point ( other.at ( first_axis ), other.at ( second_axis ) );

    bool intersect=false;
for ( auto segment:internal_map )
    {
        intersect|=wykobi::intersect ( temp,segment );
    }

    return !intersect;
}

void map2d::create ( std::string& vis, const index_map& map_statename )
{
    {
        std::string vis1= vis.substr ( strlen ( "MAP" ) +1,strlen ( vis.c_str() )-strlen ( "MAP" )-2 );

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


        first_axis=map_statename.at ( result.at ( 0 ) );
        second_axis=map_statename.at ( result.at ( 1 ) );

        openfile ( result.at ( 2 ) );

    }
}

void map2d::openfile ( std::string filename )
{
    gmlreader reader;
    std::ifstream f;
    f.open ( filename.c_str(),std::ios::in );
    if (!f.is_open())
      ERR("Attenzione, impossibile aprire il file %s",filename.c_str());
    g=reader.read ( &f );
    f.close();
    loadfromgraph ( g );

}

void map2d::loadfromgraph ( graph g )
{
        for ( auto arc=g.arcs.begin(); arc!=g.arcs.end(); ++arc )
        {
            wykobi::segment<double,2> temp;
            temp[0]=wykobi::make_point<double> ( g.nodes[arc->first].x,g.nodes[arc->first].y );
            temp[1]=wykobi::make_point<double> ( g.nodes[arc->second].x,g.nodes[arc->second].y );
            internal_map.push_back ( temp );
        }
        first_axis=0;
	second_axis=1;
}
