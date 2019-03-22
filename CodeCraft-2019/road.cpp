#include "road.h"
#include "cross.h"
#include "car.h"

int deadCarnum = 0;

/*
(道路id，道路长度，最高限速，车道数目，起始点id，终点id，是否双向)
 */

vector<Road *> Road::roads;
int Road::numOfCarInRoads = 0;

Road::Road(vector<int>& oneRoad)
    :_id(oneRoad[0]),
    _length(oneRoad[1]),
    _limitSpeed(oneRoad[2]),
    _channel(oneRoad[3]),
    _from(oneRoad[4]),
    _to(oneRoad[5]),
    _isDuplex(oneRoad[6])
    {
        carsInRoadFromTo.resize(_channel);
        carsInRoadToFrom.resize(_channel);
    }

void Road::initRoads(Map & map) 
{
    int n = map.road.size();
    for (int i = 0; i < n; ++i) 
    {
        Road::roads.push_back(new Road(map.road[i]));
    }
}

void Road::processCarInRoad(Map & map) //还没考虑拐不过去的情况
{
    //先处理正向行驶的车
    for (int row = 0; row < _length; ++row)
    {//从第一列开始处理
        for (int lane = 0; lane < _channel; ++lane)
        {
            if(carsInRoadFromTo[lane].empty())continue;
            Car* car = carsInRoadFromTo[lane][0];
            if(car->_distanceToCross == row)
            {
                if(car->_curSpeed > car->_distanceToCross)//看当前行的第一个能否出去
                {
                    if(Cross::crosses[car->_curCross - CROSS_INDEX]->_id == car->_to)//到达终点的车
                    {
                        car->setStatusEnd();
                        carsInRoadFromTo[lane].erase(carsInRoadFromTo[lane].begin());//删除第一个元素
                        outnumOfCarinRoads();
                        --_carNumFromTo;
                        continue;
                    }
                    //车即将驶出当前road，交给路口处理
                    car->_nextCross = car->searchPath(map);
                    int dif = min(roads[car->_nextRoad - ROAD_INDEX]->_limitSpeed, car->_maxSpeed) - car->_distanceToCross;
                    if (dif <= 0)//根据任务书中10.8-(5)Table1中的计算规则，车可能会被退回
                    {
                        car->_distanceToCross = 0;
                    }
                    else
                    {
                        /*把需要进入入口的车加入到队列中*/
                        if(Cross::crosses[car->_curCross - CROSS_INDEX]->addCarToQueue(car))
                        {
                            carsInRoadFromTo[lane].erase(carsInRoadFromTo[lane].begin());//删除第一个元素
                            outnumOfCarinRoads();
                            --_carNumFromTo;
                        }
                        else
                        {
                            assert(0);
                        }
                    }
                }
            }
        }
    }
    for(int i = 0; i<_channel; ++i)
    {
        if(carsInRoadFromTo[i].empty())continue;
        if(carsInRoadFromTo[i][0]->_distanceToCross != 0)//出现下一个路过不去（根据任务书中10.8-(5)Table1中的计算规则）==0的情况就放第一个
            carsInRoadFromTo[i][0]->_distanceToCross -= carsInRoadFromTo[i][0]->_curSpeed;
        for(size_t j = 1u; j<carsInRoadFromTo[i].size(); ++j)
        {
            Car* car_j = carsInRoadFromTo[i][j];
            car_j->_distanceToCross -= car_j->_curSpeed;
            //后面的车不能超过前面的车
            car_j->_distanceToCross = max(car_j->_distanceToCross, int16_t(carsInRoadFromTo[i][j-1]->_distanceToCross + 1));
        }
    }


    if (!_isDuplex)
        return;
    //处理反向行驶的车
    for (int row = 0; row < _length; ++row)
    {//从第一列开始处理
        for (int lane = 0; lane < _channel; ++lane)
        {
            if(carsInRoadToFrom[lane].empty())continue;
            Car* car = carsInRoadToFrom[lane][0];
            if(car->_distanceToCross == row)
            {
                if(car->_curSpeed > car->_distanceToCross)
                {
                    if(Cross::crosses[car->_curCross - CROSS_INDEX]->_id == car->_to)//到达终点的车
                    {
                        car->setStatusEnd();
                        carsInRoadToFrom[lane].erase(carsInRoadToFrom[lane].begin());//删除第一个元素
                        outnumOfCarinRoads();
                        --_carNumToFrom;
                        continue;
                    }
                    //车即将驶出当前road，交给路口处理
                    car->_nextCross = car->searchPath(map);
                    int dif = min(roads[car->_nextRoad - ROAD_INDEX]->_limitSpeed, car->_maxSpeed) - car->_distanceToCross;
                    if (dif <= 0)//根据任务书中10.8-(5)Table1中的计算规则，车可能会被退回
                    {
                        car->_distanceToCross = 0;
                    }
                    else
                    {
                        /*把需要进入入口的车加入到队列中*/
                        if(Cross::crosses[car->_curCross - CROSS_INDEX]->addCarToQueue(car))
                        {
                            carsInRoadToFrom[lane].erase(carsInRoadToFrom[lane].begin());//删除第一个元素
                            outnumOfCarinRoads();
                            --_carNumToFrom;
                        }
                        else
                        {
                            assert(0);
                        }
                    }
                }
            }
        }
    }
    for(int i = 0; i<_channel; ++i)
    {
        if(carsInRoadToFrom[i].empty())continue;
        if(carsInRoadToFrom[i][0]->_distanceToCross != 0)//出现下一个路过不去（根据任务书中10.8-(5)Table1中的计算规则）==0的情况就放第一个
            carsInRoadToFrom[i][0]->_distanceToCross -= carsInRoadToFrom[i][0]->_curSpeed;
        for(size_t j = 1u; j<carsInRoadToFrom[i].size(); ++j)
        {
            Car* car_j = carsInRoadToFrom[i][j];
            car_j->_distanceToCross -= car_j->_curSpeed;
            //后面的车不能超过前面的车
            car_j->_distanceToCross = max(car_j->_distanceToCross, int16_t(carsInRoadToFrom[i][j-1]->_distanceToCross + 1));
        }
    }
}

bool Road::addCarToRoad(Car* car, int lane) 
{
    int dif = min(car->_maxSpeed,_limitSpeed) - car->_distanceToCross;//根据任务书中10.8-(5)Table1中的计算规则
    assert(dif>0);//必须大于零才正常
    assert(dif<_length);
    assert(car->_curCross == _from || car->_curCross == _to);
    if(car->_curCross == _from)
    {
        if(carsInRoadFromTo[lane].empty())
        {
            carsInRoadFromTo[lane].push_back(car);
            ++_carNumFromTo;
            car->_distanceToCross = _length - dif;

        }
        else if((_length - 1) != carsInRoadFromTo[lane].back()->_distanceToCross)
        {
            if(_length - dif <= carsInRoadFromTo[lane].back()->_distanceToCross)
                car->_distanceToCross = carsInRoadFromTo[lane].back()->_distanceToCross + 1;
            else 
                car->_distanceToCross = _length - dif;
            carsInRoadFromTo[lane].push_back(car);
            ++_carNumFromTo;
        }
        else
        {
            return false;
        }
    }
    else
    {
        if(carsInRoadToFrom[lane].empty())
        {
            carsInRoadToFrom[lane].push_back(car);
            ++_carNumToFrom;
            car->_distanceToCross = _length - dif;
        }
        else if((_length - 1) != carsInRoadToFrom[lane].back()->_distanceToCross)
        {
            if(_length - dif <= carsInRoadToFrom[lane].back()->_distanceToCross)
                car->_distanceToCross = carsInRoadToFrom[lane].back()->_distanceToCross + 1;
            else 
                car->_distanceToCross = _length - dif;
            carsInRoadToFrom[lane].push_back(car);
            ++_carNumToFrom;
        }
        else
        {
            return false;
        }
    }
    addnumOfCarinRoads();
    car->_atRoad = _id;
    car->_atChannel = lane;
    car->_preCross = car->_curCross;
    car->_curCross = car->_nextCross;
    car->_answerPath.push_back(_id);
    car->_curSpeed = min(car->_maxSpeed,_limitSpeed);
    return true;
}

void Road::addCarsToRoad(queue<Car *>& waiting_cars) {
    int lane = 0;
    while (!waiting_cars.empty()) {
        Car *car = waiting_cars.front();

        if (addCarToRoad(car, lane)) 
        {
            waiting_cars.pop();
        }
        else//如果放当前车道放满了就放下一个车道
        {
            ++lane;
        }

        if (lane == _channel)//说明所有的车道都已放满
            break;
    }
    while (!waiting_cars.empty()) {//将车退回原road上，并将到路口的距离设为0，等待下一次启动
        Car *car = waiting_cars.front();
        waiting_cars.pop();
        //car->setStatusEnd();
        if(-1==car->_atRoad)
        {//起步的车出不去，那么等待下次出去，把状态都恢复到初始化
            car->setStatusStop();
            car->_startTime = car->_startTime + 1;
            car->_preCross = car->_from;
            car->_curCross = car->_from;
            car->_nextCross = car->_from;
            car->_nextRoad = -1;
            car->_turnto = isForward;
        }
        else
        {
            //assert(0);
            ++deadCarnum;
            car->setStatusEnd();
        }
    }
}

void Road::updateRoadCondition()
{
    assert(_channel*_length != 0);
    _jamsFromTo = (float)_carNumFromTo/(float)((int)_channel*(int)_length);
    _jamsToFrom = (float)_carNumToFrom/(float)((int)_channel*(int)_length);
}


// void Road::backCar(uint32_t car_id) {
//     Car::cars[car_id - CAR_INDEX]->_distanceToCross = 0;
//     //还要其它信息...
// }



