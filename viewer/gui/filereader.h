#ifndef FILEREADER_H
#define FILEREADER_H

#include <cstdlib>
#include <string.h>
#include <math.h>
#include <iostream>
#include <new>
#include <string>
#include <sstream>

#include "AgentPath.h"
#include "TargetList.h"

using namespace std;


class FileReader
{
    string filename;
    AgentPath path;
    vector<TargetList> targets;
    int time;
public:
    FileReader();
    int getTime();
    AgentPath getPath();
    vector<TargetList> getTargets();
    void readCsv(string filename);
    void readTargets(string filename);
};

#endif // FILEREADER_H
