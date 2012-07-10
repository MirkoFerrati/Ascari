#ifndef FAKE_AGENT_FOR_UDP_TESTER_HPP
#define FAKE_AGENT_FOR_UDP_TESTER_HPP
#include <../agent/communication/udp_agent_communicator.h>
#include <boost/signals2/mutex.hpp>
#include <../shared/communication/udp_receiver.hpp>
#include <boost/thread.hpp>
class fake_agent_for_udp_tester
{
public:
    fake_agent_for_udp_tester(topology_packet* data,boost::asio::io_service& io_service1,boost::asio::io_service& io_service):data(data), communicator(mutex,data,io_service),
            start_sync(io_service1,boost::asio::ip::address::from_string("0.0.0.0"),
                       boost::asio::ip::address::from_string(MULTICAST_ADDRESS),MULTICAST_PORT)
    {
		start_sync.receive();
		printMap();
        communicator.startReceive();
    }
	
	void start()
	{
		std::cout<<"starting sending thread"<<std::endl;
		int i=0;
		while (i<7)
		{
		communicator.send();
		sleep(0);
        printMap();
		}
    }
private:
    void printMap()
    {
        mutex.lock();
        for (std::map<agent_name,behavior_topology>::iterator it=data->data.begin();it!=data->data.end();it++)
        {
            for (behavior_topology::iterator itt=it->second.begin();itt!=it->second.end();itt++)
            {
                for (topology_values::iterator ittt=itt->second.begin();ittt!=itt->second.end();ittt++)
                {
                    std::cout<<it->first<<" "<<itt->first<<" "<<ittt->first<<" "<<ittt->second<<",";
                }
            }
        }
        std::cout<<std::endl;
        mutex.unlock();
    }
    boost::signals2::mutex mutex;
    udp_agent_communicator communicator;
    topology_packet* data;
    udp_receiver<int> start_sync;

};

static boost::asio::io_service io_service;
static bool should_run;

void service_thread(void) {
    while (should_run) {
		std::cout<<"starting thread"<<std::endl;
        io_service.run();
		std::cout<<"running"<<std::endl;
        io_service.reset();
    }
}

int main(int argc, char **argv) {
    srand(time(NULL));
    should_run=true;
	boost::asio::io_service io_service1;
    topology_packet data;
    if (argc<1)
        return 0;
	std::string temp=argv[1];
    if (temp.compare("1")==0)
    {
        data.data["pippo"]["re"]["t1"]=1;
        data.data["pluto"]["gnomo"]["t1"]=-1;
        data.data["mario"]["re"]["t1"]=1;
    }
    if (temp.compare("2")==0)
    {
        data.data["pippo"]["gnomo"]["t1"]=1;
        data.data["pluto"]["gnomo"]["t2"]=1;
        data.data["mario"]["re"]["t1"]=1;
    }
    if (temp.compare("3")==0)
    {
        data.data["pippo"]["gnomo"]["t2"]=1;
        data.data["pluto"]["gnomo"]["t2"]=-1;
        data.data["mario"]["re"]["t1"]=-1;
    }
    if (temp.compare("4")==0)
    {
        data.data["pippo"]["re"]["t1"]=-1;
        data.data["pluto"]["re"]["t2"]=1;
        data.data["mario"]["gnomo"]["t2"]=0;
    }
    fake_agent_for_udp_tester f(&data,io_service1,io_service);
	boost::thread   t(service_thread);
	f.start();
	should_run=false;
	t.join();
}

	


#endif // FAKE_AGENT_FOR_UDP_TESTER_HPP
