#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <cstdio>
#include <ctime>
#include <boost/program_options.hpp>
#include <boost/concept_check.hpp>
#include <logog.hpp>
#include <define.h>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <thread>
#include <agent_router/agent_router_logging.h>
using namespace std;

#define TOLERANCE 0.1

struct agent_info
{
    int best_length;
    int time_of_arrival;
    std::vector<std::pair<double,double>> speed_profile;
    std::string name;
    int target;
    int source;
    double x;
    double y;
};

int main ( int argc, char **argv )
{

    LOGOG_INITIALIZE();
    try
    {
        logog::Cout out;
        std::ifstream ifs;
        double fake=0;
        agent_router_logging parser(fake,"");
        std::vector<std::string> filenames;
        boost::program_options::options_description desc;
        boost::program_options::variables_map options;
        desc.add_options()("help,h","Get help");

        desc.add_options()("files,f",boost::program_options::value<std::vector<std::string>>(&filenames)->required(), "Log files");
        boost::program_options::positional_options_description p;
        p.add("files",-1);

        try
        {
            boost::program_options::store(boost::program_options::command_line_parser(argc,argv).options(desc).positional(p).run(),options);//Overwrite default file values with shell one

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

        for (auto filename:filenames)
        {
            ifs.open (filename.c_str(), std::ifstream::in);
            if (!ifs.is_open())
                return 1;
            std::string line_to_parse,type;
            int future_collisions=0;
            int real_collisions=0;
            std::map<std::string,agent_info> infos;
            while (ifs.good()) {
                ifs>>type;
                if (!ifs.eof())
                    std::getline(ifs,line_to_parse);

                if (type=="info:")
                {
                    if (line_to_parse.find("Position:")!=string::npos)
                    {
                        auto position=parser.parsePosition(line_to_parse);
//                         std::cout<<"Position: "<<position.name<<" "<<position.time<<" "<<position.x<<" "<<position.y<<" "<<position.theta<<std::endl;
                    }
                    if (line_to_parse.find("Speed:")!=string::npos)
                    {
                        auto speed=parser.parseSpeed(line_to_parse);
                        if (abs(infos[speed.name].speed_profile.back().second-speed.speed)>TOLERANCE)
                        {
                            infos[speed.name].speed_profile.push_back(std::make_pair(speed.time,speed.speed));
                        }
                    }
                    if (line_to_parse.find("StartInfo:")!=string::npos)
                    {
                        auto start_info=parser.parseStartInfo(line_to_parse);
                        infos[start_info.name].speed_profile.push_back(std::make_pair(0.0,0.0));
                        infos[start_info.name].source=start_info.source;
                        infos[start_info.name].target=start_info.target;
                        infos[start_info.name].x=start_info.startx;
                        infos[start_info.name].y=start_info.starty;
                    }
                    if (line_to_parse.find("FutureCollision:")!=string::npos)
                    {
                        auto collision=parser.parseFutureCollision(line_to_parse);
                        future_collisions++;
                    }
                    if (line_to_parse.find("BestLength:")!=string::npos)
                    {
                        auto best_length=parser.parseBestLength(line_to_parse);
                        infos[best_length.first].best_length=best_length.second;
                    }
                    if (line_to_parse.find("Target_reached:")!=string::npos)
                    {
                        auto target_reached=parser.parseFinalTarget(line_to_parse);
                        infos[target_reached.first].time_of_arrival=target_reached.second;
                    }
                }
                else if (type=="warning:")
                {
                    if (line_to_parse.find("Collision:")!=string::npos)
                    {
                        auto collision=agent_router_logging::parseCollision(line_to_parse);
                        real_collisions++;
                    }


                }

            }
            std::cout<<"FutureCollisions: "<<future_collisions<<std::endl;
            std::cout<<"RealCollisions: "<<real_collisions<<std::endl;
            std::cout<<"Speed profile agent "<<infos.begin()->first<<std::endl;
            for (auto speed:infos.begin()->second.speed_profile)
            {
                //std::cout<<speed.first<<" "<<speed.second<<std::endl;
            }
            for (auto agent:infos)
            {
                std::cout<<"Agent "<<agent.first<<" BestLength: "<<agent.second.best_length<<" Time of arrival: "<<agent.second.time_of_arrival<<" source:"<<agent.second.source<<" target:"<<agent.second.target<<std::endl;
            }
            ifs.close();
            //TODO save single filename result
        }
    }
    catch(const char* ex)
    {
        std::cout<<ex<<std::endl;
    }
    LOGOG_SHUTDOWN()
    return 0;
}


