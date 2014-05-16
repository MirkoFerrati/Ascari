#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <cstdio>
#include <ctime>
#include <boost/program_options.hpp>
#include <logog.hpp>
#include <communication/global.h>
#include <define.h>
#include <agent.h>
#include <communication/zmq_viewer_communicator.hpp>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <thread>
using namespace std;

int main ( int argc, char **argv )
{

    srand ( time ( NULL ) );
    LOGOG_INITIALIZE();
    std::thread exiting;

    static_zmq::context=new zmq::context_t ( 1 );
    {
        int csleep=5000;
        logog::Cout out;
        std::ifstream ifs;
        world_sim_packet packet;
        boost::program_options::options_description desc;
        boost::program_options::variables_map options;
        desc.add_options()("help,h","Get help");
        zmq_viewer_communicator communicator;
        std::string filename;
        
        desc.add_options()("filename,f",boost::program_options::value<std::string>(&filename)->required(), "Log filename");
        desc.add_options()("sleep useconds,s",boost::program_options::value<int>(&csleep), "useconds to sleep between each simulated timestep");
        for (auto config_value:CONFIG.getMap())
        {
            if (config_value.first!="filename")
                desc.add_options()(config_value.first.c_str(),config_value.second.data().c_str());
        }

        try
        {
            boost::program_options::store(boost::program_options::command_line_parser(argc,argv).options(desc).run(),options);//Overwrite default file values with shell one

            if (options.count("help"))
            {
                std::cout << "Basic Command Line Parameter App" << std::endl
                << desc << std::endl;
                return 0;
            }
            boost::program_options::notify(options);
        }
        catch (boost::program_options::error& e)
        {
            std::cerr << "ERROR: " << e.what() << std::endl << std::endl;
            std::cerr << desc << std::endl;
            return 2;
        }

        //TODO: code here real stuff
        
        ifs.open (filename.c_str(), std::ifstream::in);
        if (!ifs.is_open())
            return 1;
        std::string agent_name,header,temp;
        double time;
        double x,y,theta;
        
        while (ifs.good()) {
            usleep(csleep);
            ifs>>header>>header;
            if (header=="P:")
            {
                ifs>>time;
                ifs>>agent_name;
                ifs>>x;
                ifs>>y;
                ifs>>theta;
                INFO("Position: %lf %s %lf %lf %lf",time,agent_name.c_str(),x,y,theta);
                packet.time=time;
                packet.state_agents.internal_map[agent_name].identifier=agent_name;
                packet.state_agents.internal_map.at(agent_name).state[0]=x;
                packet.state_agents.internal_map.at(agent_name).state[1]=y;
                packet.state_agents.internal_map.at(agent_name).state[2]=theta;
                communicator.send_target ( packet,"viewer" );
                
            }
            if (!ifs.eof())
                std::getline(ifs,temp);
        }
        
    }
    exiting.join();

    LOGOG_SHUTDOWN();
    return 0;
}


