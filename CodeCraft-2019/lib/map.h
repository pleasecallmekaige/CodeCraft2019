#ifndef _MAP_H_
#define _MAP_H_
#include<iostream>
#include <string>
#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <cassert> 
#include <vector>

using namespace std;
/*
(车辆id，始发地、目的地、最高速度、出发时间)
 */

typedef int MAP_INT;

class Map
{
public:
    /*Map的构造函数*/
    Map(string roadfile, string crossfile);

    /*读入road.txt*/
    void readroad(string file);

    /*读入cross.txt*/
    void readcross(string file);

    /*初始化地图*/
    void initMap();

    vector<vector<int>> road;
    vector<vector<int>> cross;
    vector<vector<MAP_INT>> map;
};
#endif
