#ifndef _CROSS_H_
#define _CROSS_H_
#include<iostream>
#include <string>
#include <vector>
#include <queue>
#include <map>

#include "mymap.h"

using namespace std;

//#define CROSS_INDEX 1

class Road;
class Car;

class Cross
{
public:
    /*所有的路口*/
    //static vector<Cross *> crosses;
    static map<int,Cross *> crosses;

    int _id;
    /*用来查找邻接矩阵*/
    int _index;
    /*一个时间片内调度的次数*/
    int _processNum;

    Road* _Road[4];

    /*升序遍历的road*/
    vector<Road*> _sortRoad;
    /*路口处理池*/
    vector<Car *> _carPool;

 

    /*Cross的构造函数*/
    Cross(vector<int>& oneCross, map<int, Road *>& roads, int index);

    void processEachCross(Map& cityMap);

    int8_t processStartCar(Map &cityMap, Car* car);

    // int8_t outputCarToRoad();

    /*static function*/
    static void initCrosses(Map &cityMap);


    void processEachRoad(Road * road, Map& cityMap);
    void addCarToPool(Car * car, Map& cityMap);
    void delCarFromPool(Car * car);

private:
};

#endif
