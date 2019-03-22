#ifndef _ROAD_H_
#define _ROAD_H_

#include<iostream>
#include <vector>
#include <queue>

#include "map.h"
/*
(道路id，道路长度，最高限速，车道数目，起始点id，终点id，是否双向)
 */
#define ROAD_INDEX 5000

using namespace std;

class Car;

class Road
{
public:
    /*Road的构造函数*/
    Road(vector<int>& oneRoad);

    /*所有道路*/
    static vector<Road *> roads;

    static int numOfCarInRoads;

    void addnumOfCarinRoads()
    {
        ++numOfCarInRoads;    
    }

    void outnumOfCarinRoads()
    {
        --numOfCarInRoads;
    }

    /*static function*/
    static void initRoads(Map &map);

    void processCarInRoad(Map & map);//处理在道路上的车

    bool addCarToRoad(Car* car, int lane); //将车加入道路上

    void addCarsToRoad(queue<Car *> &waiting_cars); //将路口传来的车加到当前道路上；

    void updateRoadCondition();

    void backCar(uint32_t car_id);//将车退回来的road


    std::vector<std::vector<Car *>> carsInRoadFromTo;//存放当前车道上正向行驶的车id
    std::vector<std::vector<Car *>> carsInRoadToFrom;//存放当前道路上反向行驶的车id
    //std::vector<std::vector< Car *>> content; //存放当前车道上的车辆的id
    /*road的id*/
    uint32_t _id;
    /*road的长度*/
    int16_t _length;
    /*当前道路车的最高速度*/
    int8_t _limitSpeed; 
    /*车道数目*/
    int8_t _channel;
    int _from;
    int _to;
    bool _isDuplex;

    /*road的from->to的拥堵情况*/
    float _jamsFromTo;
    int _carNumFromTo;

    /*road的to->from的拥堵情况*/
    float _jamsToFrom;
    int _carNumToFrom;

    /*获取各个车道的车辆数目，以及能否进入*/
};


#endif
