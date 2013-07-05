#include "zmq_real_world_serial_communicator.h"

using namespace std;
using namespace LibSerial;

zmq_real_world_serial_communicator::zmq_real_world_serial_communicator(std::string agent_name, index_map& input_map)
:map_inputs_name_to_id(input_map)
{
	init(agent_name);
	
	command_old[map_inputs_name_to_id.at(DEFAULT_VELOCITY_VARIABLE)]=0;
	command_old[map_inputs_name_to_id.at(DEFAULT_OMEGA_VARIABLE)]=0;
	
	
	
	/* Serial Port to Arduino initialization */
	/*
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

	*/
}


const world_sim_packet& zmq_real_world_serial_communicator::receive_agents_status()
{
  try{
    agent_sim_packet_receiver& tmp=receive().front(); //TODO(Mirko): check if working
    packet_received.state_agents.internal_map.clear();
    packet_received.bonus_variables=tmp.bonus_variables;
    packet_received.time=tmp.time;
    packet_received.object_list.objects.swap(tmp.objects.objects);
    for (auto agent=tmp.state_agents.internal_map.begin();agent!=tmp.state_agents.internal_map.end();++agent){
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
		send(packet);
				
		if (abs(packet.default_command.at(map_inputs_name_to_id.at(DEFAULT_VELOCITY_VARIABLE))-command_old.at(map_inputs_name_to_id.at(DEFAULT_VELOCITY_VARIABLE)))>0.01 || abs(packet.default_command.at(map_inputs_name_to_id.at(DEFAULT_OMEGA_VARIABLE))-command_old.at(map_inputs_name_to_id.at(DEFAULT_OMEGA_VARIABLE)))>0.01)
		{
		  //serial_port << setprecision(6)<<ARDUINO_COMMAND_CODE<<","<<packet.default_command.at(map_inputs_name_to_id.at(DEFAULT_VELOCITY_VARIABLE))<<","<<packet.default_command.at(map_inputs_name_to_id.at(DEFAULT_OMEGA_VARIABLE))<<";"<<endl;
		cout << "SerialMessage Sent:" << packet.default_command.at(map_inputs_name_to_id.at(DEFAULT_VELOCITY_VARIABLE))<<","<<packet.default_command.at(map_inputs_name_to_id.at(DEFAULT_OMEGA_VARIABLE))<<  endl;

		}
		command_old.at(map_inputs_name_to_id.at(DEFAULT_VELOCITY_VARIABLE))=packet.default_command.at(map_inputs_name_to_id.at(DEFAULT_VELOCITY_VARIABLE));
		command_old.at(map_inputs_name_to_id.at(DEFAULT_OMEGA_VARIABLE))=packet.default_command.at(map_inputs_name_to_id.at(DEFAULT_OMEGA_VARIABLE));
		
}


zmq_real_world_serial_communicator::~zmq_real_world_serial_communicator()
{

}
