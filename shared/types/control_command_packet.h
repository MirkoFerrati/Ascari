#ifndef CONTROL_COMMAND_PACKET_H
#define CONTROL_COMMAND_PACKET_H
#include <types.h>

struct control_command_packet
{
    std::map<automaton_state ,control_command> commands;
    control_command default_command;
    std::string identifier;

    template <typename Archive>
    void serialize(Archive& ar, const unsigned int /*version*/)
    {
	ar& default_command;
        ar& identifier;
    }
};

#endif //CONTROL_COMMAND_PACKET_H