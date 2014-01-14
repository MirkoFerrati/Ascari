#ifndef COLLISIONCHECKER_H
#define COLLISIONCHECKER_H

#include "collisioncheckerabstract.h"
#include <types/agents_name_to_states.h>
#include <types/agent_state_packet.h>
#include <vector>

class CollisionChecker : public CollisionCheckerAbstract
{

public:
    virtual ~CollisionChecker();
    CollisionChecker(const agents_name_to_states& states,const index_map & states_to_index);
	bool checkCollisions(simulation_time& time);
private:
    const std::map<std::string,agent_state_packet>& states;
	
	std::vector<int> indexes_to_check;
};

#endif // COLLISIONCHECKER_H
