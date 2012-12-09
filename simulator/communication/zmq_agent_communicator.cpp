#include "zmq_agent_communicator.h"
#include <zmq.hpp>
#include <boost/lexical_cast.hpp>
#include <vector>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <iomanip>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/map.hpp>
#include <iostream>
#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/archive/binary_iarchive.hpp>

zmq_agent_communicator::zmq_agent_communicator(int num_agents): agent_communicator_abstract(),context(1)
        ,publisher(context, ZMQ_PUB),syncservice(context, ZMQ_REP)
{
    publisher.bind("tcp://*:5561");
    syncservice.bind("tcp://*:5562");
    this->subscribers_expected=num_agents;

    int subscribers = 0;
    while (subscribers < subscribers_expected) {

        //  - wait for synchronization request
        zmq::message_t message_tmp;
        syncservice.recv(&message_tmp);
        std::string nome(static_cast<char*>(message_tmp.data()), message_tmp.size());

        std::string string = "one more agent connected to server: "; //non so chi si sia connesso, sono tutti uguali
        //  - send synchronization reply
        string.append(nome);
        zmq::message_t message(string.size());
        memcpy(message.data(), string.data(), string.size());

        bool rc = syncservice.send(message);

        subscribers++;
    }
}

std::vector< control_command_packet > zmq_agent_communicator::receive_control_commands()
{

    int subscribers = 0;
    control_command_packet packet;
    std::vector<control_command_packet> temp;
    while (subscribers < subscribers_expected) {

        zmq::message_t message_tmp;
		
        syncservice.recv(&message_tmp);
		boost::iostreams::stream_buffer<boost::iostreams::basic_array_source<char> > buffer( (char*)message_tmp.data(), message_tmp.size());
		boost::archive::binary_iarchive archive(buffer, boost::archive::no_header);
        archive >> packet;
        temp.push_back(packet);
    }

}


void zmq_agent_communicator::send_broadcast(const world_sim_packet& infos)
{
    std::ostringstream archive_stream;
    boost::archive::text_oarchive archive(archive_stream);
    archive << infos;
    zmq::message_t message(archive_stream.str().length());
    memcpy(message.data(), archive_stream.str().data(),archive_stream.str().length());
    publisher.send(message);
}


void zmq_agent_communicator::send_target(const world_sim_packet& infos, const target_abstract* target)
{
    throw "not implemented";
}


