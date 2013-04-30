#ifndef MAP2D_H
#define MAP2D_H

#include "visibility.h"
#include "../gml2lgf/gmlreader.hpp"

class map2d: public visibleArea
{
    std::vector<wykobi::segment<double,2>> internal_map;
    int first_axis;
    int second_axis;

public:

    map2d ( std::string& vis, const index_map& map_statename );

    bool isVisible ( const agent_state& me, const agent_state& other );

private:
    void openfile ( std::string filename );
  
    void create ( std::string& vis, const index_map& map_statename );
    void loadfromgraph ( graph g );
    

};





#endif
