#include "parser.h"
#include "filereader.h"
#include "AgentPath.h"
#include "wallreader.h"

Parser::Parser(int argc,char *argv[])
{
    endtime=0;
    for (int i=1;i<argc;i++)
    {
        string temp;
        temp=argv[i];
        if (temp.rfind(".wal")==temp.length()-4){
            WallReader reader(argv[i]);
            walls=reader.getWalls();
            continue;
        }
        if (temp.rfind(".svg")==temp.length()-4){
            imagePath=temp;
            continue;
        }
        if (temp.rfind(".png")==temp.length()-4){
            imagePath=temp;
            continue;
        }
        if (temp.rfind(".bmp")==temp.length()-4){
            imagePath=temp;
            continue;
        }

        if (temp.rfind("_targets.txt")==temp.length()-string("_targets.txt").length()){
            FileReader reader;
            reader.readTargets(argv[i]);
            string agent_name=temp.substr(temp.find("agent_")+6,-temp.find("agent_")-6+temp.rfind("_targets.txt"));
            if (endtime!=0)
            {
                if (endtime!=reader.getTime()){
                    throw "La lunghezza dei file è diversa";
                }
            }
            else
            {
                endtime=reader.getTime();
            }
            targets.insert(std::pair<string, vector<TargetList> >(agent_name,reader.getTargets()));
            agents_name.insert(agent_name);
            continue;
        }

        if (temp.rfind(".csv")==temp.length()-4){
            FileReader reader;
            string agent_name=temp.substr(temp.find("agent_")+6,-temp.find("agent_")-6+temp.rfind(".csv"));
            reader.readCsv(argv[i]);
            if (endtime!=0)
            {
                if (endtime!=reader.getTime()){
                    throw "La lunghezza dei file è diversa";
                }
            }
            else
            {
                endtime=reader.getTime();
            }
            paths.insert(std::pair<string, AgentPath>(agent_name,reader.getPath()));
            agents_name.insert(agent_name);
            continue;
        }
    }

}
int Parser::getTime()
{
    return endtime;
}



int Parser::GetAgentNumber()
{
    return paths.size();
}

int Parser::getMaxX()
{
    map<string, AgentPath>::iterator it=paths.begin();
    double max=it->second.getMaxX();
    double temp;
    for (;it!=paths.end();it++)
    {
        temp=it->second.getMaxX();
        if (temp>max)
            max=temp;
    }
    return max;
}

int Parser::getMaxY()
{
    map<string, AgentPath>::iterator it=paths.begin();
    double max=it->second.getMaxY();
    double temp;
    for (;it!=paths.end();it++)
    {
        temp=it->second.getMaxY();
        if (temp>max)
            max=temp;
    }
    return max;
}


int Parser::getMinX()
{
     map<string, AgentPath>::iterator it=paths.begin();
    double min=it->second.getMinX();
    double temp;
    for (;it!=paths.end();it++)
    {
        temp=it->second.getMinX();
        if (temp<min)
            min=temp;
    }
    return min;
}

int Parser::getMinY()
{
    map<string, AgentPath>::iterator it=paths.begin();
    double min=it->second.getMinY();
    double temp;
    for (;it!=paths.end();it++)
    {
        temp=it->second.getMinY();
        if (temp<min)
            min=temp;
    }
    return min;
}
