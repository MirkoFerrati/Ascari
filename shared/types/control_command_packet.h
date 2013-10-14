#ifndef CONTROL_COMMAND_PACKET_H
#define CONTROL_COMMAND_PACKET_H
#include <types.h>

struct control_command_packet
{
    control_command command;
    std::string identifier;

    template <typename Archive>
    void serialize(Archive& ar, const unsigned int /*version*/)
    {
	ar& command;
        ar& identifier;
    }
};

#endif //CONTROL_COMMAND_PACKET_H