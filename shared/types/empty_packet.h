#ifndef EMPTY_PACKET_H
#define EMPTY_PACKET_H

struct empty_packet
{
	std::string agent_id;
	template <typename Archive>
	void serialize(Archive& ar, const unsigned int /*version*/)
	{
		ar& agent_id;
	}
};

#endif //empty_packet