#include "collisionchecker.h"
#include <logog.hpp>

#define COLLISION_RANGE 5

bool CollisionChecker::checkCollisions()
{
    for (std::map<std::string,agent_state_packet>::const_iterator it=states.begin();it!=states.end();++it) {
        for (std::map<std::string,agent_state_packet>::const_iterator itt=states.begin();itt!=states.end();++itt) {
            if (it.operator!=(itt))
            {
                bool collision=true;
                for (unsigned int i=0;i<indexes_to_check.size();i++)
                {
                    collision=collision &&  (abs(it->second.state.at(i)-itt->second.state.at(i))<COLLISION_RANGE);
                }
                if (collision)
                {
                    ERR("Attenzione, c'è stata una collisione tra gli agenti %s e %s",it->first.c_str(),itt->first.c_str());
                    return true;
                }
            }
        }
    }
    return false;
}


CollisionChecker::~CollisionChecker()
{

}

CollisionChecker::CollisionChecker(const agents_name_to_states& states, const std::vector< index_map >& states_to_index)
        :CollisionCheckerAbstract(states,states_to_index),states(states.internal_map)
{
    if (states_to_index[0].count("X")) //TODO(Mirko): IssueID #50
        indexes_to_check.push_back(states_to_index[0].at("X"));
    if (states_to_index[0].count("Y"))
        indexes_to_check.push_back(states_to_index[0].at("Y"));
}
