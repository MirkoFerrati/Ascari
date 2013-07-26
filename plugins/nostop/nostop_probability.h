#ifndef NOSTOP_PROBABILIY_H
#define NOSTOP_PROBABILIY_H
namespace NoStop 
{
	/// Return value between 0 and maxValue-1
	int getRandomValue(int maxValue);

	bool agentHasToExperiments(double _explorationRate);
}

#endif // NOSTOP_PROBABILIY_H