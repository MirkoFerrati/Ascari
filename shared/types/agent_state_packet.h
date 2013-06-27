#ifndef AGENT_STATE_PACKET_H
#define AGENT_STATE_PACKET_H
#include <string>
#include <map>
#include <boost/serialization/map.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/serialization.hpp>


struct agent_state_packet
{
    std::map<int,double> state;
    std::string identifier;

    template <typename Archive>
    void serialize(Archive& ar, const unsigned int /*version*/)
    {
        ar& state;
        ar& identifier;
    }
};

#endif // AGENT_STATE_PACKET_H
