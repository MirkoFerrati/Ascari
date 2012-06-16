#ifndef AGENTPATH_H
#define AGENTPATH_H

#include <vector>
#include <string>
#include <stdio.h>

#include <cmath>
using namespace std;

struct AgentPath{
    vector<double> x;
    vector<double> y;
    vector<double> a;

    int getMax(vector<double>& v)
    {
        double max=v.at(0);
        for (unsigned int i = 0; i<v.size();i++)
            if (v.at(i)>max)
                max=v.at(i);
        return max;
    }

    int getMin(vector<double>& v)
    {
        double min=v.at(0);
        for (unsigned int i = 0; i<v.size();i++)
            if (v.at(i)<min)
                min=v.at(i);
        return min;
    }

    int getMaxX()
    {
        return getMax(x);
    }

    int getMaxY()
    {
        return getMax(y);
    }

    int getMinX()
    {
        return getMin(x);
    }

    int getMinY()
    {
        return getMin(y);
    }


    string toString()
    {
        string result="";
        for (unsigned i=0;i<x.size();i++)
        {
            char temp[100];
            sprintf(temp,"%f,%f,%f",x[i],y[i],a[i]);
            result.append(temp);
            result.append("\n");

        }
        return result;
    }

};

#endif // AGENTPATH_H
