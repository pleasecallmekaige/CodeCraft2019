#ifndef _ROAD_H_
#define _ROAD_H_

#include<iostream>
#include <vector>
#include <queue>
#include <map>

#include "mymap.h"
/*
(道路id，道路长度，最高限速，车道数目，起始点id，终点id，是否双向)
 */
//#define ROAD_INDEX 5000

using namespace std;

class Car;

class Road
{
public:
    /*road的id*/
    int _id;
    /*索引*/
    int _index;
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
    Road(vector<int>& oneRoad, int index);

    /*所有道路*/
    static map<int,Road *> roads;

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
    static void initRoads(Map &cityMap);

    static void runAllCarInInitList(Map& cityMap);

    void driveOneChannel(vector<Car *>& oneChannel);

    void driveAllCarJustOnRoadToEndStatus();//处理在道路上的车

    /*获取每个road的优先级第一的车，没有可出来的车就返回NULL*/
    Car* getFirstCar(int16_t curCross);

    /*获取各个车道的尾部能不能加车进来,
    返回-2：就是不能加车进来,下一条路限速过不去，
    返回-1：就是不能加车进来前面有等待车辆，
    返回-3：前面堵死但能前进，只能到当前路口最前面，
    返回1:能加进来
    输入len:在当前路段的distenceToCross
    */
    int canAddToButton(const Car* car);

    void addCarToRoad(Car * car, int lane); //将路口传来的车加到当前道路上

    void outCarToRoad(Car * car);//处理行使出去的车；

    void updateRoadCondition(Map& cityMap);

    float getJams(int _curCross);
private:
    //bool ToRoad(Car* car, int lane); //将车加入道路上
};


#endif
