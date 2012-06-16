#include "filereader.h"
#include <fstream>
#include <iostream>
#include <string>
#include <algorithm>

FileReader::FileReader():time(1)
{

}

void FileReader::readCsv(string filename){
    cout<<filename<<endl;
    ifstream f;
    f.open(filename.c_str());
        string str;
        int x_index(-1),y_index(-1),t_index(-1);
        getline(f,str);
        stringstream ss(str);
        string item;
        vector<string> elems;
        elems.push_back("time");
        while(getline(ss, item, ',')) {
            item.erase (std::remove (item.begin(), item.end(), ' '), item.end());
            elems.push_back(item);
               if  (item.compare("x")==0)
                   x_index=elems.size()-1;
               if  (item.compare("y")==0)
                   y_index=elems.size()-1;
               if  (item.compare("t")==0)
                   t_index=elems.size()-1;
        }
        if (x_index==-1 || y_index==-1 || t_index==-1)
            cout<<"warning, uno dei parametri di stato (x,y,t) non è presente"<<endl;
        double fake=0;
        double x=0;
        double y=0;
        double t=0;
        char v;
        do
        {
            for (unsigned int i=0;i<elems.size();i++)
            {
                if (i!=elems.size())
                    f>>fake>>v;
                else
                {
                    f>>fake;
                    getline(f,str);
                }
                if (i==x_index)
                    x=fake;
                if (i==y_index)
                    y=fake;
                if (i==t_index)
                    t=fake;
            }
            path.a.push_back(t);
            path.x.push_back(x);
            path.y.push_back(y);
            time++;
        }while (!f.eof());
}

void FileReader::readTargets(string filename)
{
    cout<<filename<<endl;
    ifstream f;
    f.open(filename.c_str());
    string str;
    double x=0;
    double y=0;
    double t=0;
    TargetList target;
    stringstream ss;
    do
    {
        t=-1;
        str="";
        target.x.clear();
        target.y.clear();
        f>>t;
        ss.clear();
        if (f.eof()) break;
//        if(time>3996)
//        {
//            cout<<"inizio debug";
//        }
        if ((int)(t*10+0.005)!=time)
            cerr<<"Nel file di target "<<filename<<" manca un tempo"<<endl;
        getline(f,str);
        ss.str(str);

        while (!ss.eof() && !(str.compare("")==0))
        {
            ss>>x>>y;
            target.x.push_back(x);
            target.y.push_back(y);
        }
        targets.push_back(target);
        time++;
    }while (!f.eof());

}

vector<TargetList> FileReader::getTargets()
{
    return targets;
}

int FileReader::getTime()
{
    return time;
}

AgentPath FileReader::getPath()
{
    return path;
}
