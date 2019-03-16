#include<iostream>
#include <vector>
/*
(车辆id，始发地、目的地、最高速度、出发时间)
 */

class car
{
public:
    car(std::vector<int> &res)
       :id(res[0]),
        start(res[1]),
        end(res[2]),
        maxSpeed(res[3]),
        planeTime(res[4]),
        status(isStop),
        turnto(forward)
        {}
    car(uint32_t _id, int8_t _start, int8_t _end, int8_t _maxSpeed, uint32_t _planeTime)
       :id(_id),
        start(_start),
        end(_end),
        maxSpeed(_maxSpeed),
        planeTime(_planeTime),
        status(isStop),
        turnto(forward)
        {}
    ~car(){}
    /**/
    enum TURN {left=0, forward=1, right=2,};

    enum RUNSTATUS {isStop=0, isRuning=1, isEnd=2};

    /*还未启动的车数量，当*/
    static uint32_t numStop;

    /*已经上路的车数量*/
    static uint32_t numRuning;

    /*已经到达目的地车数量*/
    static uint32_t numEnd;
private:
    uint32_t id;
    int8_t start;
    int8_t end;
    int8_t maxSpeed; /*车的最高速度*/
    uint32_t planeTime;/*出发时间*/

    /*下个路口的左右拐*/
    TURN turnto;
    RUNSTATUS status;/*车的状态*/

    int8_t curSpeed;/*车的当前速度*/
    int atRoad;/*当前处于那条road*/
    int8_t atChannel;/*当前处于哪条channel*/
    int atCross[2];/*当前处于那个cross*/

    uint32_t curTime;/*当前时间*/
};