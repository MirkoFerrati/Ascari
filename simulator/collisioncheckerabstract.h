#ifndef COLLISIONCHECKERABSTRACT_H
#define COLLISIONCHECKERABSTRACT_H
#include <types/agents_name_to_states.h>
#include <types.h>
 

class CollisionCheckerAbstract
{

public:
	CollisionCheckerAbstract(const agents_name_to_states& /*states*/, const  index_map & /*states_to_index*/){};
	virtual ~CollisionCheckerAbstract(){};
	virtual bool checkCollisions(simulation_time& time)
	{
		return false;
	};
};

#endif // COLLISIONCHECKERABSTRACT_H
