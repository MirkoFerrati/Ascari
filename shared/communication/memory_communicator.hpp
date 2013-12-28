#include <map>
#include <string>
#include <condition_variable>
#include <communication/world_communicator_abstract.h>
#include <communication/agent_communicator_abstract.h>

template <class Rx,class Tx> 
class memory_manager
{
public:
    void receivedAndElaborated()//threads notify they have sent stuff back and wait for simulator to ack the receive
    {
        //std::cout<<"receivedAndElaborated before lock"<<std::endl;
        std::unique_lock<std::mutex> lck(counter_mtx);
        counter++;
        //std::cout<<"receivedAndElaborated notify counter_cv"<<std::endl;
        counter_cv.notify_one();
        //while (counter) //as long as counter is not resetted by simulator, we wait here
        {   
            //std::cout<<"receivedAndElaborated before wait"<<std::endl;
            simulator_cv.wait(lck);
            //std::cout<<"receivedAndElaborated after wait"<<std::endl;
        }
        //std::cout<<"receivedAndElaborated exiting"<<std::endl;
    }
    void waitForReceive()//threads wait for simulator to send data
    {
        //std::cout<<"waitForReceive before lock"<<std::endl;
        std::unique_lock<std::mutex> lck(counter_mtx);
        while (!ready)
        {
            //std::cout<<"waitForReceive before wait"<<std::endl;
            receiver_lock.wait(lck);
            //std::cout<<"waitForReceive after wait"<<std::endl;
        }
        //std::cout<<"waitForReceive exiting"<<std::endl;
    }
    void sentToThreads()//simulator notifies threads that data is ready
    {
        
        //std::cout<<"sentToThreads before lock counter"<<std::endl;
        std::unique_lock<std::mutex> lck(counter_mtx);
        counter=0;
        //std::cout<<"sentToThreads notify simulator_cv"<<std::endl;
        simulator_cv.notify_all();
        
        //std::cout<<"sentToThreads before lock"<<std::endl;
        //std::unique_lock<std::mutex> lck(mtx);
        ready=true;
        //std::cout<<"sentToThreads notify receiver_lock"<<std::endl;
        receiver_lock.notify_all();
    }
    void waitForSend()//simulator waits for all the threads before locking and sending stuff
    {
        //std::cout<<"waitForSend before lock"<<std::endl;
        std::unique_lock<std::mutex> lck(counter_mtx);
        while(!(counter==num_agents)) 
        {
            //std::cout<<"waitForSend before wait"<<std::endl;
            counter_cv.wait(lck);
            //std::cout<<"waitForSend after wait"<<std::endl;
        }
        //std::cout<<"waitForSend exiting"<<std::endl;
        //ready=false;
        //counter=0;
        //simulator_cv.notify_all();
    }
protected:
    std::vector<Rx> receive_data;
    Tx send_data;
    std::condition_variable receiver_lock;
    std::mutex mtx;
//     std::map<std::string,bool> ready;
//     
//     std::mutex thread_counter_mtx;
    std::mutex counter_mtx;
    std::condition_variable counter_cv;
    std::condition_variable simulator_cv;
    int counter;
    bool ready=false;
//     int thread_counter;
    int num_agents;
};


class memory_full_communicator:
public memory_manager<control_command_packet,world_sim_packet>,
public agent_namespace::world_communicator_abstract,public simulator_namespace::agent_communicator_abstract
{
public:
    memory_full_communicator(int num_agents)
    {
        this->num_agents=num_agents;
        counter=0;//num_agents;
    }
    virtual const world_sim_packet& get_last_received()
    {
        throw "not implemented";
    }
    virtual const world_sim_packet& receive_agents_status()
    {
        waitForReceive(); //locks until simulator has written new agent state
        return send_data;
    }
    virtual std::vector< control_command_packet >& receive_control_commands()
    {
        waitForSend(); //locks until all threads have written new commands
        std::unique_lock<std::mutex> lck(receive_data_mutex);
        return receive_data;
    }
    virtual void send_broadcast(const agent_sim_packet& infos)
    {
        send_data.bonus_variables=infos.bonus_variables; //No threads should be reading this now
        send_data.object_list=infos.objects;
        send_data.time=infos.time;
        for (auto state:infos.state_agents.internal_map)
            send_data.state_agents.internal_map[state.first]=state.second;
        {
        std::unique_lock<std::mutex> lck(receive_data_mutex);
        receive_data.clear();
        }
        sentToThreads(); //Threads can unlock and read from now on
    }
    virtual void send_control_command(control_command_packet& command, const target_abstract& )
    {
        {
        std::unique_lock<std::mutex> lck(receive_data_mutex);
        receive_data.push_back(command); //Nobody should be reading this now
        }
        receivedAndElaborated(); //Simulator can unlock and read from now on
    }
    virtual void send_target(const agent_sim_packet& , const target_abstract& ){
        throw "not implemented";
    }
private:
    std::mutex receive_data_mutex;
};