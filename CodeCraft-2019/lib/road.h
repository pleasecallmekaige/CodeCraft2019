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

    void processCarInRoad(Map & map);//处理在道路上的车

    void addCarToRoad(uint32_t carId, int lane); //将车加入道路上

    void addCarsToRoad(std::queue<Car*> waiting_cars);//将路口传来的车加到当前道路上；

    bool isFullLane(int lane); //判断当前道路线上是否能再放车；

    void backCar(uint32_t car_id);//将车退回来的road


    std::vector<std::vector<uint32_t>> carsInRoadFromTo;//存放当前车道上正向行驶的车id
    std::vector<std::vector<uint32_t>> carsInRoadToFrom;//存放当前道路上反向行驶的车id
    std::vector<std::vector<Car*>> content; //存放当前车道上的车辆的id
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