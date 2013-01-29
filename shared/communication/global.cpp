#include "global.h"
namespace static_zmq
{
	zmq::context_t context(1);
}

int s_interrupted=0;