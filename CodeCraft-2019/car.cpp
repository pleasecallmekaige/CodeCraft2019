#include "car.h"

extern uint32_t time;
/*
(车辆id，始发地、目的地、最高速度、出发时间)
 */

uint32_t Car::numStop = 0;
uint32_t Car::numRuning = 0;
uint32_t Car::numEnd = 0;
uint32_t Car::numALL = 0;

int Car::searchPath(Map &map)
{
    vector<int> nextRoad;
    vector<int> nextCross;
    int res = 0;
    nextRoad = map.cross[curCross-1];
    for(int i = 1; i<=4; ++i)
    {
        if(nextRoad[i] != -1)
            nextCross.push_back((map.road[nextRoad[i]-5000][4] == curCross)? map.road[nextRoad[i]-5000][5]:map.road[nextRoad[i]-5000][4]);
    }
    int distance = INT32_MAX;
    while(!nextCross.empty())
    {
        int tmp = map.map[nextCross.back()][to]; 
        if(tmp < distance) {
            distance = tmp;
            res =nextCross.back();
        }
        nextCross.pop_back();
    }
    return res;
}

void Car::updataCurCross(int nextCross)
{ 
    curCross = nextCross;
    answerPath.push_back(curCross);
    if(curCross == to)
        setStatusEnd();
}

void Scheduler(vector<Car *>& cars)
{
    static int j = 1;
    for (int i=0; i<Car::numALL; ++i )
    {
        Car* p = cars[i];
        if(p->planeTime == j && p->getStatus() == isStop && Car::numRuning <10)
        {
            p->setStatusRuning();
            p->setStartTime(time);
        }
    }
    if(Car::numRuning < 2)++j;
}
