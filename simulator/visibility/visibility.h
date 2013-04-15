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
#include "../shared/wykobi/wykobi.hpp"

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


class polygon: public visibleArea
{
  wykobi::polygon<double,2> pol;
  
public:
  
    polygon();
    bool isVisible ( const agent_state& me, const agent_state& other )
    {return true;}
    
private:
  void create();
  
};










#endif //VISIBILITY