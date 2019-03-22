#ifndef _CROSS_H_
#define _CROSS_H_
#include<iostream>
#include <string>
#include <vector>
#include <queue>

#include "map.h"

using namespace std;

#define CROSS_INDEX 1

class Road;
class Car;

// struct CrossToRoad
// {
//     /*对应路口的id号*/
//     int roadId;

//     queue<Car* > DQueue;
//     queue<Car* > LQueue;
//     queue<Car* > RQueue;
//     queue<Car* > startQueue;
//     queue<Car* > outQueue;

// };

class Cross
{
public:
    /*所有的路口*/
    static vector<Cross *> crosses;

    int _id;

    Road* _Road[4];

    vector<Road*> _sortRoad;
 

    /*Cross的构造函数*/
    Cross(vector<int>& oneCross, vector<Road *>& roads);

    void processEachCross();

    bool addCarToQueue(Car *car);

    int8_t processStartCar(Map &map, Car* car);

    int8_t outputCarToRoad();

    /*static function*/
    static void initCrosses(Map &map, vector<Road *>& roads);

private:
    void processEachRoad();

};

#endif
