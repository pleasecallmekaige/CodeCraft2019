#include "car.h"

extern uint32_t turntime;
/*
(车辆id，始发地、目的地、最高速度、出发时间)
 */

int Car::numStop = 0;
int Car::numRuning = 0;
int Car::numEnd = 0;
int Car::numALL = 0;

int Car::searchPath(Map &map)
{
    vector<int> nextRoad;
    vector<int> trueNextRoad;
    vector<int> nextCross;
    int resCross = 0;
    int resRoad = 0;

    nextRoad = map.cross[curCross-1];
    for(int i = 1; i<=4; ++i)
    {
        if(nextRoad[i] != -1)
        {
            /*if it is Duplex?*/
            if(map.road[nextRoad[i]-5000][6] == 1)
            {
                nextCross.push_back((map.road[nextRoad[i]-5000][4] == curCross)? map.road[nextRoad[i]-5000][5]:map.road[nextRoad[i]-5000][4]);
                trueNextRoad.push_back(nextRoad[i]);
            } 
            else if(map.road[nextRoad[i]-5000][6] == 0 && curCross == map.road[nextRoad[i]-5000][4])
            {
                nextCross.push_back(map.road[nextRoad[i]-5000][5]);
                trueNextRoad.push_back(nextRoad[i]);
            }
        }           
    }
    int distance = INT32_MAX;
    while(!nextCross.empty())
    {
        int tmp = map.map[nextCross.back()][to]; 
        if(tmp < distance) {
            distance = tmp;
            resCross = nextCross.back();
            resRoad = trueNextRoad.back();
        }
        nextCross.pop_back();
        trueNextRoad.pop_back();
    }
    atRoad = resRoad;
    return resCross;
}

void Car::updataCurCross(int nextCross)
{ 
    curCross = nextCross;
    answerPath.push_back(atRoad);
    if(curCross == to)
        setStatusEnd();
}

void Scheduler(vector<Car *>& cars)
{
    static uint32_t j = 1;
    for (int i=0; i<Car::numALL; ++i )
    {
        Car* p = cars[i];
        if(p->planeTime == j && p->getStatus() == isStop && Car::numRuning <100)
        {
            p->setStatusRuning();
            p->setStartTime(turntime);
        }
    }
    if(Car::numRuning < 10)++j;
}
//100 10

