#ifndef _MYMAP_H_
#define _MYMAP_H_
#include<iostream>
#include <string>
#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <cassert> 
#include <vector>
#include <map>

using namespace std;
/*
(车辆id，始发地、目的地、最高速度、出发时间)
 */

typedef int MAP_INT;

class Cross;
class Road;

class Map
{
public:
    /*Map的构造函数*/
    Map(string roadfile, string crossfile, string presetAnswerPath);

    /*读入road.txt*/
    void readroad(string file);

    /*读入cross.txt*/
    void readcross(string file);

    /*初始化地图*/
    static void initMap(Map & cityMap, map<int, Cross*>& crosses);

    void updataOneRoad(Road* road);

    void updateMatrix();

    int getDistance(int cross1_id, int cross2_id);

    void readPresetAnswer(string file);

    vector<vector<int>> road;
    vector<vector<int>> cross;
    vector<vector<int>> presetcar;
    vector<vector<MAP_INT>> mymap;
};


#endif
