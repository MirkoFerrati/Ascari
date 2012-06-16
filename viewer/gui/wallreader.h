#ifndef WALLREADER_H
#define WALLREADER_H

#include "wall.h"
#include "vector"
#include <cstdlib>
#include <string.h>
#include <math.h>
#include <iostream>
#include <new>
#include <string>
#include <sstream>

using namespace std;

class WallReader
{
    vector<wallState> walls;
public:
    WallReader(string filename);
    vector<wallState> getWalls();
};

#endif // WALLREADER_H
