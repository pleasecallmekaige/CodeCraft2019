#ifndef _CAR_H_
#define _CAR_H_
#include<iostream>
#include <vector>

#include "mymap.h"
#include "road.h"
#include "cross.h"

//#define CAR_INDEX 10000
/*
(车辆id，始发地、目的地、最高速度、出发时间)
 */

using namespace std;

enum TURN {isRight=0, isLeft=1, isForward=2, none=-1};

enum RUNSTATUS {isStop=0, isRuning=1, isEnd=2};

class Road;
class Cross;

class Car
{
public:
/*初始化后所有车的指针数组，
    Car::cars[i]就是第i辆车的指针*/
    static vector<Car *> cars;

    /*还未启动的车数量，当*/
    static int numStop;

    /*已经上路的车数量*/
    static int numRuning;

    /*已经到达目的地车数量*/
    static int numEnd;

    /*所有车数量*/
    static int numALL;

    /*所有处于等待状态车的数量*/
    static int numWait;

    uint32_t _id;
    int16_t _from;
    int16_t _to;
    /*车的最高速度*/
    int8_t _maxSpeed;
    /*计划出发时间*/
    uint32_t _planeTime;

    vector<int16_t> _answerPath;
    /*出发时间*/
    uint32_t _startTime;
    /*上个路口的id*/
    int16_t _preCross;
    /*当前路口的id*/
    int16_t _curCross;
    /*通过搜索算法得到的下个路口的id*/
    int16_t _nextCross;

    /*下个路口的左右拐*/
    TURN _turnto;
    /*车的状态*/
    RUNSTATUS _status;
    /*车的当前速度*/
    int8_t _curSpeed;
    /*当前处于哪条road*/
    Road* _atRoad;
    /*接下来走哪条road*/
    Road* _nextRoad;
    /*当前处于哪条channel*/
    int8_t _atChannel;
    /*当前距离Cross的长度*/
    int16_t _distanceToCross;
    /*最短路径/速度*/
    int _priority;
    
    bool _isEndStatusOnRoad;

    Car(std::vector<int> &res);

    TURN whereToTurn(Road* atRoad, Road* nextRoad, vector<int>& nextRoadvector);

    /*车辆根据地图自己搜索下一个路径
    输入：起始地点和终点以及地图
    输出：下一个cross的id
    更新了_nextRoad _turnto
    */
    int searchPath(Map &cityMap);

    /*把车移动到自己的下一个目标入口，必须确保下一个road更新了*/
    void moveToNextRoad(int lane);

    /*车到达终点*/
    void moveToEnd();

    RUNSTATUS getStatus()
    {
        return _status;
    }

    void setStatusRuning()
    {
        _status = isRuning;
        ++numRuning;
        --numStop;
    }
    void setStatusEnd()
    {
        _status = isEnd;
        --numRuning;
        ++numEnd;
    }
    void setStatusStop()//用于车子启动失败
    {
        assert(_status == isRuning);
        _status = isStop;
        --numRuning;
        ++numStop;
    }

    void setStartTime(uint32_t turntime)
    {
        _startTime = turntime;
    }

    Road* getNextRoad()
    {
        return _nextRoad;
    }
    Road* getAtRoad()
    {
        return _atRoad;
    }
    Cross* getCurCross()
    {
        return Cross::crosses[_curCross];
    }

    static void readCars(string file, Map &cityMap);

    /*初始化所有车辆*/
    static void initCars(string file, Map &cityMap);

    /*车辆调度函数,控制车辆的启动*/
    static void Scheduler(Map &cityMap);

    int getScore(int distance, int _curCross, int nextRoadId);

    int getShortestDistance(Map &cityMap);

};


#endif
