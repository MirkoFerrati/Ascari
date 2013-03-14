#ifndef TESTVISIBILITY_HPP
#define TESVISIBILITY_HPP


#include <vector>
#include <map>
#include <string>
#include <boost/concept_check.hpp>
#include "typedefs.h"
#include "testClass.h"
#include <boost/regex.hpp>
#include "../simulator/visibility/visibility.h"


class testVisibility:public testClass
{
public:
    void test()
    {
	
	
        index_map map_statename;

        map_statename["x"]=0;
        map_statename["y"]=1;

        std::shared_ptr<visibleArea> vis;

        std::map<std::string,bool> vis_area;

        vis_area["CIRCLE(x,y,105)"]=true;
        vis_area["CIRCLE(x,y,10.5)"]=true;
        vis_area["CIRCLE(x,y,a)"]=false;
        vis_area["CIRCLE"]=false;
        vis_area["CIRCLE(x,100)"]=false;
        vis_area["CIRCLE(x,y,10.0.5)"]=false;
        vis_area["CIRCLE()"]=false;


        for ( auto it=vis_area.begin(); it!=vis_area.end(); ++it )
        {
            try
            {	if (it->second==false)
		std::cout<<"Test con Visible Area Non corretta"<<std::endl;
                vis = visibleArea::createVisibilityFromParsedVisibleArea ( it->first,map_statename );
            }
            catch ( const char* e )
            {
                if ( ! ( it->second==false && strcmp ( e,"VISIBLE AREA INCORRECTLY SPECIFIED" ) ==0 ) )
                    throw;
            }
        }

    
    }
};

#endif // TESTREGEX_HPP
