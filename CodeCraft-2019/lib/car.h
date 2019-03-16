#ifndef _CAR_H_
#define _CAR_H_
#include<iostream>
#include <vector>

#include "map.h"
/*
(车辆id，始发地、目的地、最高速度、出发时间)
 */

using namespace std;

enum TURN {isLeft=0, isForward=1, isRight=2};

enum RUNSTATUS {isStop=0, isRuning=1, isEnd=2};

class Car
{
public:
    Car(std::vector<int> &res)
       :id(res[0]),
        from(res[1]),
        to(res[2]),
        maxSpeed(res[3]),
        planeTime(res[4]),
        curCross(res[1]),
        turnto(isForward),
        status(isStop)
        {
            answerPath.push_back(from);
        }
    Car(uint32_t _id, int8_t _from, int8_t _to, int8_t _maxSpeed, uint32_t _planeTime)
       :id(_id),
        from(_from),
        to(_to),
        maxSpeed(_maxSpeed),
        planeTime(_planeTime),
        curCross(_from),
        turnto(isForward),
        status(isStop)
        {}
    ~Car(){}

    static void Scheduler(vector<Car *>& cars);

    /*车辆根据地图自己搜索下一个路径
    输入：起始地点和终点以及地图
    输出：下一个路口的id
    */
    int searchPath(Map &map);

    /*更新当前岔路口*/
    void updataCurCross(int nextCross);

    RUNSTATUS getStatus()
    {
        return status;
    }
    void setStatusRuning()
    {
        status = isRuning;
        ++numRuning;
        --numStop;
    }
    void setStatusEnd()
    {
        status = isEnd;
        --numRuning;
        ++numEnd;
    }

    void setStartTime(uint32_t time)
    {
        startTime = time;
    }


    /*还未启动的车数量，当*/
    static uint32_t numStop;

    /*已经上路的车数量*/
    static uint32_t numRuning;

    /*已经到达目的地车数量*/
    static uint32_t numEnd;

    static uint32_t numALL;

    uint32_t id;
    int16_t from;
    int16_t to;
    int8_t maxSpeed; /*车的最高速度*/
    uint32_t planeTime;/*出发时间*/

    vector<int16_t> answerPath;
    uint32_t startTime;

private:

    int16_t curCross;
    /*下个路口的左右拐*/
    TURN turnto;
    RUNSTATUS status;/*车的状态*/

    // int8_t curSpeed;/*车的当前速度*/
    // int atRoad;/*当前处于那条road*/
    // int8_t atChannel;/*当前处于哪条channel*/
    // int atCross[2];/*当前处于那个cross*/


};
#endif
