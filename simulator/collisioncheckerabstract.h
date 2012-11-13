#ifndef COLLISIONCHECKERABSTRACT_H
#define COLLISIONCHECKERABSTRACT_H
#include <typedefs.h>

class CollisionCheckerAbstract
{

public:
	CollisionCheckerAbstract(const agents_name_to_states& /*states*/, const std::vector< index_map >& /*states_to_index*/){};
	virtual ~CollisionCheckerAbstract(){};
	virtual bool checkCollisions()
	{
		return false;
	};
};

#endif // COLLISIONCHECKERABSTRACT_H
