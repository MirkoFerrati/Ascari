#ifndef AGENT_COMMUNICATOR_H
#define AGENT_COMMUNICATOR_H


namespace agent_namespace{
class agent_communicator_abstract
{
public:
	virtual void send(bool)=0;
	virtual void startReceive(bool)=0;
	virtual ~agent_communicator_abstract(){};

};
}
#endif // AGENT_COMMUNICATOR_H
