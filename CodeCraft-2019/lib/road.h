#include<iostream>
#include <vector>
#include "car.h"
#include "map.h"
#include "cross.h"
/*
(道路id，道路长度，最高限速，车道数目，起始点id，终点id，是否双向)
 */
#define ROAD_INDEX 5000
using namespace std;

class Road
{
public:
    /*Road的构造函数*/
    Road(vector<int>& oneRoad);

    /*所有道路*/
    static vector<Road *> roads;

    /*static function*/
    static void initRoads(Map &map);

    void processCarInRoad();//处理在道路上的车

    void addCarToRoad(queue<Car *>& inputCarQueue); //将车加入道路上

    


    std::vector<std::vector<Car*>> _carsInRoad;//存放当前车道上的车
    std::vector<std::vector<Car*>> _content; //存放当前车道上的车辆的id
    uint32_t _id;
    int8_t _length;
    int8_t _speed; //道路的线路
    int8_t _limitSpeed; /*车的最高速度*/
    uint32_t _channel;/*车道数目*/
    int _from;
    int _to;
    bool _isDuplex;

    /*获取各个车道的车辆数目，以及能否进入*/
};