#include "nostop_probability.h"

#include <random>
#include <ctime>
namespace NoStop
{
	//////////////////////////////////////////////////////////////////////////
	int getRandomValue(int maxValue)
	{
		//srand( (unsigned int) time(NULL) );

		return rand() % maxValue;
	}

	//////////////////////////////////////////////////////////////////////////
	bool agentHasToExperiments(double _explorationRate)
	{
		//srand( (unsigned int) time(NULL) );

		double val = (double)rand() / RAND_MAX;

		if (val < _explorationRate)
			return true;
		else
			return false;
	}
}
