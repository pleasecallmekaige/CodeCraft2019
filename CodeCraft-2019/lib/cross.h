#ifndef _CROSS_H_
#define _CROSS_H_
#include<iostream>
#include <string>
#include <vector>
#include <queue>

#include "car.h"

using namespace std;

#define CROSS_INDEX 1

struct CrossToRoad
{
    /*对应路口的id号*/
    int roadId;

    queue<Car* > DQueue;
    queue<Car* > LQueue;
    queue<Car* > RQueue;
    queue<Car* > startQueue;
    queue<Car* > outQueue;

};

class Cross
{
public:
    /*Cross的构造函数*/
    Cross(vector<int>& oneCross);

    int id;

    CrossToRoad outToRoad[4];
    //int roadId[4];

    /*所有的路口*/
    static vector<Cross *> crosses;

    /*static function*/
    static void initCrosses(Map &map);

    int8_t processStartCar(Map &map, Car* car);
    
private:

    

};

#endif
