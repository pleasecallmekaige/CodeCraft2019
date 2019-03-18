#ifndef _CROSS_H_
#define _CROSS_H_
#include<iostream>
#include <string>
#include <vector>
#include <queue>

#include "car.h"

using namespace std;

class CrossToRoad
{


    queue<Car* > DQueue;
    queue<Car* > LQueue;
    queue<Car* > RQueue;
    queue<Car* > outQueue;
};

class Cross
{
public:
    /*Cross的构造函数*/
    Cross(Map &map);

    /*读入road.txt*/
    void readroad(string file);

    /*读入cross.txt*/
    void readcross(string file);

    /*初始化地图*/
    void initMap();

private:

    CrossToRoad outToRoad;

};

#endif
