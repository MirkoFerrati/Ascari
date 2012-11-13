#ifndef COLLISIONCHECKER_H
#define COLLISIONCHECKER_H

#include "collisioncheckerabstract.h"

class CollisionChecker : public CollisionCheckerAbstract
{

public:
    virtual ~CollisionChecker();
    CollisionChecker(const agents_name_to_states& states,const std::vector< index_map >& states_to_index);
	bool checkCollisions();
private:
    const std::map<std::string,agent_state_packet>& states;
	
	std::vector<int> indexes_to_check;
};

#endif // COLLISIONCHECKER_H
