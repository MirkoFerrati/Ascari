#ifndef MONITOR_RESULT_H
#define MONITOR_RESULT_H


struct monitor_result
{
	std::string agent_id;
	std::string behavior_name;
	bool ok;
	
	template <typename Archive>
	void serialize(Archive& ar, const unsigned int /*version*/)
	{
		ar& behavior_name;
		ar& ok;
		ar& agent_id;
	}
	
};


#endif // MONITOR_RESULT_H
