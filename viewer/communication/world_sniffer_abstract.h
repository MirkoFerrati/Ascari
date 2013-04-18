#ifndef WORLD_COMMUNICATOR_H
#define WORLD_COMMUNICATOR_H






class world_sniffer_abstract
{
public:
	virtual void start_receiving()=0;
	virtual void stop_receiving()=0;
//	{
//	        std::cout<<"chiamato stop_receiving di abstract sniffer"<<std::endl;

//	};
	virtual ~world_sniffer_abstract(){};
};

#endif // WORLD_COMMUNICATOR_H