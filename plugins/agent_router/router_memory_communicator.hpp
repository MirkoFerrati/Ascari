#include <map>
#include <string>
#include <condition_variable>
#include <types/graph_informations.h>
//#include <communication/world_communicator_abstract.h>
//#include <communication/agent_communicator_abstract.h>

class router_memory_manager
{
public:
    router_memory_manager(int num_agents)
    {
//        this->num_agents=num_agents;
//        this->counter=0;//num_agents;
    }
    graph_packet receive()
    {
        std::unique_lock<std::mutex> lck(mtx);
//        if(counter!=0) receiver_lock.wait(lck);
        return data;
    }
    void send(const graph_informations& info)
    {
        std::unique_lock<std::mutex> lck(mtx);
//         if (data[info.id].timestamp<info.timestamp)
//             counter=(counter+1)%num_agents; //another agent has sent new data
        data[info.id]=info;
//         if (counter==0)
//             receiver_lock.notify_all();
    }
private:
    static graph_packet data;
    static std::mutex mtx;
//     static std::condition_variable receiver_lock;
//     static int num_agents;
//     static int counter;
};

