#include "zmq_real_world_serial_communicator.h"

using namespace std;
using namespace LibSerial;

// Record the execution time of some code, in milliseconds.



int64_t get_tick_count()
{
    struct timespec tp;
    clock_gettime(CLOCK_MONOTONIC, &tp);
    return (int64_t)tp.tv_sec*1000000000 + tp.tv_nsec;
}

#define DECLARE_TIMING(s)  int64_t timeStart_##s; double timeDiff_##s; double timeTally_##s = 0; int countTally_##s = 0
#define START_TIMING(s)    timeStart_##s = get_tick_count()
#define STOP_TIMING(s) 	   timeDiff_##s = (double)(get_tick_count() - timeStart_##s); timeTally_##s += timeDiff_##s; countTally_##s++
#define GET_TIMING(s) 	   (double)(timeDiff_##s / ( 1e9*1000.0))
#define GET_AVERAGE_TIMING(s)   (double)(countTally_##s ? timeTally_##s/ ((double)countTally_##s *  1e9*1000.0) : 0)
#define CLEAR_AVERAGE_TIMING(s) timeTally_##s = 0; countTally_##s = 0


zmq_real_world_serial_communicator::zmq_real_world_serial_communicator(std::string agent_name, index_map& input_map)
    :map_inputs_name_to_id(input_map)
{
    init(agent_name);

    command_old[map_inputs_name_to_id.at(DEFAULT_VELOCITY_VARIABLE)]=0;
    command_old[map_inputs_name_to_id.at(DEFAULT_OMEGA_VARIABLE)]=0;



    /* Serial Port to Arduino initialization */
    
    serial_port.Open("/dev/ttyACM0");
    assert(serial_port.good());

    serial_port.SetBaudRate( SerialStreamBuf::BAUD_115200 );
    assert(serial_port.good());

    serial_port.SetCharSize( SerialStreamBuf::CHAR_SIZE_8 );
    assert(serial_port.good());

    serial_port.SetParity( SerialStreamBuf::PARITY_NONE );
    assert(serial_port.good());

    serial_port.SetNumOfStopBits( 1 );
    assert(serial_port.good());

    serial_port.SetFlowControl( SerialStreamBuf::FLOW_CONTROL_NONE );
    assert(serial_port.good());

    std::string temp;
    serial_port << 1<<";";//<<","<<packet.default_command.at(map_inputs_name_to_id.at(DEFAULT_VELOCITY_VARIABLE))<<","<<packet.default_command.at(map_inputs_name_to_id.at(DEFAULT_OMEGA_VARIABLE))<<";"<<endl;
//    serial_port>>temp;
    //cout<<temp<<endl;
    serial_port << 2<<";";
//    serial_port>>temp;
   // cout<<temp<<endl;
sleep(1);    
    
}


const world_sim_packet& zmq_real_world_serial_communicator::receive_agents_status()
{
    try {
        agent_sim_packet_receiver tmp=receive_last_one();

        packet_received.state_agents.internal_map.clear();
        packet_received.bonus_variables.swap(tmp.bonus_variables);
        packet_received.time=tmp.time;
        packet_received.object_list.objects.swap(tmp.objects.objects);
        for (auto agent=tmp.state_agents.internal_map.begin(); agent!=tmp.state_agents.internal_map.end(); ++agent) {
            packet_received.state_agents.internal_map[agent->first]=*(agent->second);
        }

        return  packet_received;
    }
    catch (zmq::error_t ex)
    {
        if (zmq_errno()==EINTR)
        {
            ERR("programma terminato",NULL);
            return  packet_received;
        }
        else
            throw ex;
    }
}


void zmq_real_world_serial_communicator::send_control_command(control_command_packet& packet, const target_abstract& /*target*/)
{
  
  cout<<"sending packet to simulator"<<endl;
    send(packet);
    DECLARE_TIMING(myTimer);


        START_TIMING(myTimer);

    cout<<"sending message to serial port"<<endl;
    
        serial_port << setprecision(6)<<ARDUINO_COMMAND_CODE<<","<<
        packet.default_command.at(map_inputs_name_to_id.at(DEFAULT_VELOCITY_VARIABLE))<<","<<
        packet.default_command.at(map_inputs_name_to_id.at(DEFAULT_OMEGA_VARIABLE))<<";";
	serial_port.flush();

	cout << "SerialMessage Sent:" << packet.default_command.at(map_inputs_name_to_id.at(DEFAULT_VELOCITY_VARIABLE))<<","<<packet.default_command.at(map_inputs_name_to_id.at(DEFAULT_OMEGA_VARIABLE))<<  endl;
        STOP_TIMING(myTimer);
       // printf("Execution time: %f ms.\n", GET_TIMING(myTimer) );
        //printf("Average time: %f ms per iteration.\n", GET_AVERAGE_TIMING(myTimer) );

}


zmq_real_world_serial_communicator::~zmq_real_world_serial_communicator()
{

}
