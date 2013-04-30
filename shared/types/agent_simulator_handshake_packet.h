#ifndef AGENT_SIMULATOR_HANDSHAKE_PACKET_H
#define AGENT_SIMULATOR_HANDSHAKE_PACKET_H


struct agent_simulator_handshake_packet{
  bool accepted;
  std::string message;
  
  template <typename Archive>
    void serialize(Archive& ar,const unsigned int /*version*/)
    {
	ar& accepted;
        ar& message;
    }
  
};






#endif //AGENT_SIMULATOR_HANDSHAKE_PACKET_H