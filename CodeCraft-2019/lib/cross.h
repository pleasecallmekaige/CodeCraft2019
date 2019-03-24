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
    /*一个时间片内调度的次数*/
    int _processNum;

    Road* _Road[4];

    /*升序遍历的road*/
    vector<Road*> _sortRoad;
    /*路口处理池*/
    vector<Car *> _carPool;

 

    /*Cross的构造函数*/
    Cross(vector<int>& oneCross, vector<Road *>& roads);

    void processEachCross(Map& map);

    int8_t processStartCar(Map &map, Car* car);

    // int8_t outputCarToRoad();

    /*static function*/
    static void initCrosses(Map &map, vector<Road *>& roads);


    void processEachRoad(Road * road, Map& map);
    void addCarToPool(Car * car, Map& map);
    void delCarFromPool(Car * car);

private:
};

#endif
