#ifndef VISIBILITY
#define VISIBILITY

 
#include <exprtk.hpp>
#include <iostream>
#include "logog.hpp"
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <fstream>
#include <memory>
#include "../shared/wykobi/wykobi.hpp"
#include <types.h>



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







#endif //VISIBILITY