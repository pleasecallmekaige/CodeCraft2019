#include "car.h"
#include "cross.h"
#include "road.h"
#include "param.h"
#include <algorithm>
#include <set>
#include <stdlib.h>
#include <time.h>

extern uint32_t turntime;
extern uint32_t PriCarallAriveEnd;
/*
(车辆id，始发地、目的地、最高速度、出发时间)
 */
const TURN TURNTABLE[4][4]  =   {{ none,     isRight,  isForward,isLeft    },
                                 { isLeft,   none,     isRight,  isForward },
                                 { isForward,isLeft,   none,     isRight   },
                                 { isRight,  isForward,isLeft,   none      }
                                  };

vector<Car *> Car::cars;
vector<Car *> Car::pricars;
map<int, vector<int>> Car::presetCars;

int Car::numStop = 0;
int Car::numRuning = 0;
int Car::numStart = 0;
int Car::numEnd = 0;
int Car::numALL = 0;
int Car::numWait = 0;
int Car::numPresetAndPriRuning = 0;

Car::Car(std::vector<int> &res)//res就是car.txt的一行
       :_id(res[0]),
        _from(res[1]),
        _to(res[2]),
        _maxSpeed(res[3]),
        _planeTime(res[4]),
        _startTime(_planeTime),
        _preCross(res[1]),
        _curCross(res[1]),
        _nextCross(res[1]),
        _turnto(isForward),
        _status(isStop),
        _curSpeed(_maxSpeed),
        _atRoad(NULL),//初始化时没有路，是在路口上
        _nextRoad(NULL),
        _atChannel(0),
        _distanceToCross(0),
        _priority(res[5]),
        _preset(res[6]),
        _isEndStatusOnRoad(true),
        _numOfSchedule(0)
    {     
    }

void Car::setStatusStart()
{
    _status = isStart;
    ++numStart;
    --numStop;
}
void Car::setStatusRuning()
{
    _status = isRuning;
    if(_preset == 1)
        ++numPresetAndPriRuning;
    ++numRuning;
    --numStart;
}
void Car::setStatusEnd()
{
    _status = isEnd;
    --numRuning;
    ++numEnd;
    _numOfSchedule = turntime - _planeTime;
    if(_preset == 1)
        --numPresetAndPriRuning;
    if(_priority == 1 && turntime>PriCarallAriveEnd)
        PriCarallAriveEnd = turntime;
}
void Car::setStatusStop()//用于车子启动失败
{
    assert(_status == isRuning);
    _status = isStop;
    --numRuning;
    ++numStop;
}

TURN Car::whereToTurn(Road* atRoad, Road* nextRoad, vector<int>& nextRoadvector)
{
    int f=-1,t=-1;
    if(NULL == atRoad)
        return isForward;
    for(int i = 1; i<=4; ++i)//第0个是crossid，从第一个开始
    {
        if(atRoad->_id == nextRoadvector[i])
            f = i;
        if(nextRoad->_id == nextRoadvector[i])
            t = i;
    }
    return TURNTABLE[t-1][f-1];
}

void Car::moveToNextRoad(int lane)
{
    Road* pnextRoad = _nextRoad;
    Road* patRoad = _atRoad;
    vector<vector<Car*>>& fromVector = (_curCross==patRoad->_from)?patRoad->carsInRoadToFrom:patRoad->carsInRoadFromTo;
    // vector<vector<Car*>>& toVector = (_curCross==pnextRoad->_from)?pnextRoad->carsInRoadFromTo:pnextRoad->carsInRoadToFrom;
    
    assert(fromVector[this->_atChannel].front() == this);
    patRoad->outCarToRoad(this); 
    patRoad->driveOneChannel(fromVector[this->_atChannel]);  
    pnextRoad->addCarToRoad(this, lane);//更新车的所有状态
}

void Car::moveToEnd()
{
    Road* patRoad = _atRoad;
    patRoad->outCarToRoad(this);
    this->setStatusEnd();
}

void Car::searchPath(Map &cityMap)
{
    vector<int> nextRoad;
    vector<int> trueNextRoad;
    vector<int> nextCross;
    int resCross = -1;
    int resRoad = -1;
    if(_nextRoad != _atRoad)return;//每个路口第一次寻路的时候_nextRoad == _atRoad的，这里是保证每个路口不寻路两次
    nextRoad = cityMap.cross[Cross::crosses[_curCross]->_index];
#if TEST_ANSWER
    if(1)
#else
    if(_preset == 1)//处理预置车辆的寻路
#endif
    {
        if(_atRoad == NULL)
            _nextRoad = Road::roads[_answerPath[0]];
        else
        {
            auto it = find(_answerPath.begin(),_answerPath.end(),_atRoad->_id);
            _nextRoad = Road::roads[*(++it)];
        }
        
        /*更新turnto*/
        _turnto = whereToTurn(_atRoad, _nextRoad, nextRoad);
        _nextCross = (_curCross == _nextRoad->_from)?_nextRoad->_to:_nextRoad->_from;
        return;
    }
    for(int i = 1; i<=4; ++i)//第0个是crossid，从第一个开始
    {
        if(nextRoad[i] != -1)
        {
            /*处理单向通行的road*/
            if(cityMap.road[Road::roads[nextRoad[i]]->_index][6] == 1)
            {//查找下一个相邻的路口
                nextCross.push_back((cityMap.road[Road::roads[nextRoad[i]]->_index][4] == _curCross)? cityMap.road[Road::roads[nextRoad[i]]->_index][5]:cityMap.road[Road::roads[nextRoad[i]]->_index][4]);
                trueNextRoad.push_back(nextRoad[i]);
            }
            else if(cityMap.road[Road::roads[nextRoad[i]]->_index][6] == 0 && _curCross == cityMap.road[Road::roads[nextRoad[i]]->_index][4])
            {
                nextCross.push_back(cityMap.road[Road::roads[nextRoad[i]]->_index][5]);
                trueNextRoad.push_back(nextRoad[i]);
            }
        }
    }
    int score = INT32_MAX;
    for(size_t i = 0u; i<nextCross.size(); ++i)
    {
        int distance = cityMap.getDistance(nextCross[i], _to);// + cityMap.getDistance(_curCross, nextCross[i]); 
        distance = getScore(distance, _curCross, trueNextRoad[i]);
        if(distance < score && nextCross[i] != _preCross)
        {//出现距离更小的路，且没有掉头返回上一个路口（车辆不允许掉头）
            score = distance;
            resCross = nextCross[i];
            resRoad = trueNextRoad[i];
        }
    }
    /*更新目标road*/
    assert(resRoad != -1);//没有找到下个路
    assert(resCross != -1);//没有找到下个路口
    if(_curCross == _to)//特殊处理一下到达终点的车辆的搜索 判定为直行，
    {
        int f = 0;
        for(int i = 1; i<=4; ++i)//第0个是crossid，从第一个开始
        {
            if(_atRoad->_id == nextRoad[i])
            {
                f = i;//获取到atRoad的索引号
                break;
            }
        }
        f = (f+1)%4+1; //获取对面Road的索引号
        if(nextRoad[f] == -1)
            _nextRoad = NULL;
        else
            _nextRoad = Road::roads[nextRoad[f]];
        _turnto = isForward;
        _nextCross = _curCross;
        return;
    }
    _nextRoad = Road::roads[resRoad];

    /*更新turnto*/
    _turnto = whereToTurn(_atRoad, _nextRoad, nextRoad);
    _nextCross = resCross;
    // return resCross;
}

int Car::getScore(int distance, int _curCross, int nextRoadId)
{
    Road* proad = Road::roads[nextRoadId];
    float jams = proad->getJams(_curCross);
    assert(jams<1);
    //if(jams<0.2)jams=0;
    if(jams>=0.9)jams=0.9;
    int a = distance/(1.0-jams);
    return a;
}

bool comp1(Car* i, Car* j){return i->_id < j->_id;}
bool comp2(Car* i, Car* j){return i->_planeTime < j->_planeTime;}
/*static function*/
void Car::initCars(string file, Map &cityMap)//这里排序可能会造成发车不能按ID升序发车
{
    Car * car;
    set<int> carOf10persent;
    int numof10persent = cityMap.presetcar.size()/10;
    sort(cityMap.presetcar.begin(),cityMap.presetcar.end(),[](vector<int>a, vector<int>b){return a[1]>b[1];});
    for(int i=0; i<numof10persent; ++i)
    {  
        carOf10persent.insert(cityMap.presetcar[i][0]);
    } 
    for(size_t i=0u; i<cityMap.cars.size(); ++i)
    {
        car = new Car(cityMap.cars[i]);
        if(car->_priority == 1)
            pricars.push_back(car);
        else
		    cars.push_back(car);
        if(car->_preset == 1)
        {
            if(carOf10persent.count(car->_id)>0)
            {
                car->_preset = 0;
                continue;
            }
            car->_startTime = presetCars[car->_id][1];//预置车的出发时间
            for(size_t i=2u; i<presetCars[car->_id].size(); ++i)
            {
                car->_answerPath.push_back(presetCars[car->_id][i]);
            }
        }
#if TEST_ANSWER
        if(car->_preset == 0)
        {
            for(size_t i=0u; i<cityMap.answer.size(); ++i)
            {
                if(cityMap.answer[i][0] == (int)car->_id)
                {
                    car->_startTime = cityMap.answer[i][1];
                    for(size_t j=2u; j<cityMap.answer[i].size(); ++j)
                    {
                        car->_answerPath.push_back(cityMap.answer[i][j]);
                    }
                    goto out;
                }
            }
            assert(0);
            out:;
        }
#else
#endif
    }

    sort(cars.begin(),cars.end(),comp1);
    // stable_sort(cars.begin(),cars.end(),comp2);
    sort(pricars.begin(),pricars.end(),comp1);
    // stable_sort(pricars.begin(),pricars.end(),comp2);
}