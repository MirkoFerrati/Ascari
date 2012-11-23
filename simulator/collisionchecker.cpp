#include "collisionchecker.h"
#include <logog.hpp>

#define COLLISION_RANGE 5

bool CollisionChecker::checkCollisions()
{
	bool ret=false;
    for (std::map<std::string,agent_state_packet>::const_iterator it=states.begin();it!=states.end();++it) {
        for (std::map<std::string,agent_state_packet>::const_iterator itt=states.begin();itt!=states.end();++itt) {
            if (it.operator!=(itt))
            {
                bool collision=true;
                for (unsigned int i=0;i<indexes_to_check.size();i++)
                {
					if (it->second.state.count(i) && itt->second.state.count(i))
						collision=collision &&  (abs(it->second.state.at(i)-itt->second.state.at(i))<COLLISION_RANGE);
                }
                if (collision)
                {
                    ERR("Attenzione, c'e' stata una collisione tra gli agenti %s e %s",it->first.c_str(),itt->first.c_str());
					ret=true;
                }
            }
        }
    }
    return ret;
}


CollisionChecker::~CollisionChecker()
{

}

CollisionChecker::CollisionChecker(const agents_name_to_states& states, const index_map& states_to_index)
        :CollisionCheckerAbstract(states,states_to_index),states(states.internal_map)
{
    if (!(states_to_index.count("X") && states_to_index.count("Y")))
	{	
		ERR("impossibile effettuare il controllo sugli stati x e y perche' nessun agente possiede quegli stati",NULL);
		throw "impossibile effettuare il controllo sugli stati x e y perche' nessun agente possiede quegli stati";
	}
	if (states_to_index.count("X")) //TODO(Mirko): IssueID #50
	   indexes_to_check.push_back(states_to_index.at("X"));
	if (states_to_index.count("Y"))
	   indexes_to_check.push_back(states_to_index.at("Y"));
}
