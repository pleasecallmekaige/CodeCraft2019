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
#include "param.h"

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
#if TEST_ANSWER
    Map(string carfile, string roadfile, string crossfile, string presetAnswerPath, string AnswerPath);
#else
    Map(string carfile, string roadfile, string crossfile, string presetAnswerPath);
#endif

    void readcars(string file);

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

    vector<vector<int>> cars;
    vector<vector<int>> road;
    vector<vector<int>> cross;
    vector<vector<int>> presetcar;
#if TEST_ANSWER
    void readAnswer(string file);
	vector<vector<int>> answer;
#else
#endif
    vector<vector<MAP_INT>> mymap;
};


#endif
