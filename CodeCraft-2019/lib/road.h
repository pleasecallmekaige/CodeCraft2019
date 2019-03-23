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
    /*road的id*/
    int _id;
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

    int _numOfWaitCar;

    std::vector<std::vector<Car *>> carsInRoadFromTo;//存放当前车道上正向行驶的车id
    std::vector<std::vector<Car *>> carsInRoadToFrom;//存放当前道路上反向行驶的车id

    /*Road的构造函数*/
    Road(vector<int>& oneRoad);

    /*所有道路*/
    static vector<Road *> roads;

    static int numOfCarInRoads;

    void addNumOfWaiteCar();

    void delNumOfWaiteCar();

    // void addNumOfCarInRoads()
    // {
    //     ++numOfCarInRoads;    
    // }

    // void outNumOfCarInRoads()
    // {
    //     --numOfCarInRoads;
    // }

    /*static function*/
    static void initRoads(Map &map);

    void driveOneChannel(vector<Car *>& oneChannel);

    void driveAllCarJustOnRoadToEndStatus();//处理在道路上的车

    /*获取每个road的优先级第一的车，没有可出来的车就返回NULL*/
    Car* getFirstCar(int16_t curCross);

    /*获取各个车道的尾部能不能加车进来,
    返回false：就是不能加车进来，
    返回true:能加进来
    输入len:在当前路段的distenceToCross
    */
    bool canAddToButton(Car * car);

    //void processRoad(Map & map);

    void addCarToRoad(Car * car); //将路口传来的车加到当前道路上

    void outCarToRoad(Car * car);//处理行使出去的车；

    void updateRoadCondition();

    float getJams(int _curCross);
private:
    bool ToRoad(Car* car, int lane); //将车加入道路上
};


#endif
