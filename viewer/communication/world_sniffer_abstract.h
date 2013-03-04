#ifndef WORLD_COMMUNICATOR_H
#define WORLD_COMMUNICATOR_H

#include "typedefs.h"




class world_sniffer_abstract
{
public:
	virtual void start_receiving()=0;
	virtual ~world_sniffer_abstract(){};
};

#endif // WORLD_COMMUNICATOR_H