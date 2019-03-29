/*
(路口id,道路id,道路id,道路id,道路id)
 */
#include "cross.h"
#include "road.h"
#include "car.h"
#include "param.h"

map<int,Cross *> Cross::crosses;

void swap(Road*& a, Road*& b)
{
    Road* tmp;
    tmp = a;
    a = b;
    b = tmp;
}

void mycomp(vector<Road*>& eachRoad)
{
    size_t n = eachRoad.size();
    for(size_t i=0u; i<n; ++i)
    {
        if(eachRoad[i] == NULL)
        {
            eachRoad.erase(eachRoad.begin()+i);
            --i;
            --n;
        }
    }
    for(size_t i = 0u; i<n; ++i)
    {
        for(size_t j = 0u; j<n-i-1; ++j)
        {
            if(eachRoad[j]->_id > eachRoad[j+1]->_id)
            {
                swap(eachRoad[j], eachRoad[j+1]);
            }
        }
    }
}

Cross::Cross(vector<int>& oneCross, map<int, Road *>& roads, int index)
   :_id(oneCross[0]),
    _index(index),
    _processNum(0)
{
    _Road[0] = NULL;
    _Road[1] = NULL;
    _Road[2] = NULL;
    _Road[3] = NULL;
    if(oneCross[1] != -1) _Road[0] = roads[oneCross[1]];
    if(oneCross[2] != -1) _Road[1] = roads[oneCross[2]];
    if(oneCross[3] != -1) _Road[2] = roads[oneCross[3]];
    if(oneCross[4] != -1) _Road[3] = roads[oneCross[4]];

    for(int i = 0; i<4; ++i)
    {
        _sortRoad.push_back(_Road[i]);
    }
    mycomp(_sortRoad);
    assert(_carPool.empty());
}

/*static function*/
void Cross::initCrosses(Map &cityMap)
{
    int n = cityMap.cross.size();
    for(int i = 0; i<n; ++i)
    {
        Cross * pcross = new Cross(cityMap.cross[i], Road::roads, i);
        Cross::crosses.insert(map<int, Cross *>::value_type(pcross->_id, pcross));
    }
}

void Cross::addCarToPool(Car * car, Map& cityMap)
{
    /*搜索路径算法放这里*/
    car->searchPath(cityMap);//_nextRoad已经更新为要出发的road  _toturn
    _carPool.push_back(car);
}

void Cross::delCarFromPool(Car * car)
{
    size_t n = _carPool.size();
    for(size_t i = 0u; i<n; ++i)
    {
        if(_carPool[i] == car)
        {
            _carPool.erase(_carPool.begin()+i);
            --i;
            --n;
        }
    }
}

void Cross::processEachRoad(Road* proad, Map& cityMap)
{
    Car* car = NULL;
    vector<vector<Car*>>& roadvector = (_id==proad->_from)?proad->carsInRoadToFrom:proad->carsInRoadFromTo;
    int lane = 0;
    car = proad->getFirstCar(_id);
    while(proad->_numOfWaitCar)
    {
        //car = proad->getFirstCar(_id);
        if(car == NULL)return;//这个路没有第一优先级可以出来的车
        assert(car->getAtRoad()->_id == proad->_id);
        assert(car->_curCross == _id );
        /*处理冲突*/
        for(size_t i=0u; i<_carPool.size(); ++i)
        {
            if(_carPool[i]->_id == car->_id)continue;
            if(car->_turnto == isForward || car->_to == _id)break;
            if(car->_nextRoad == _carPool[i]->_nextRoad && car->_turnto < _carPool[i]->_turnto)
            {//有别的车也去这个车的目标road且优先级比自己高（有冲突）
                assert(_carPool[i]->_nextRoad!=NULL);
                return;
            }
        }
        if(car->_to == _id)//到达终点
        {
            assert(roadvector[car->_atChannel].front() == car);
            car->moveToEnd();
            assert(car->_atChannel < proad->_channel);
            proad->driveOneChannel(roadvector[car->_atChannel]);
            goto out;
        }
        /*下一条路能不能进，有wait状态的车辆挡着，或者三条路都堵死了*/
        lane = car->getNextRoad()->canAddToButton(car);
        if(-3 == lane || -2 == lane)
        {
            car->_distanceToCross = 0;
            assert(car->_isEndStatusOnRoad == false);
            car->_isEndStatusOnRoad = true;
            car->getAtRoad()->delNumOfWaiteCar();
            assert(car->_atChannel < proad->_channel);
            assert(roadvector[car->_atChannel].front() == car);
            proad->driveOneChannel(roadvector[car->_atChannel]);
            goto out;
        }
        else if(-1 == lane)
        {
            return;
        }
        assert(lane>=0 && lane < car->getNextRoad()->_channel);
        
        /*这一辆车调度没有冲突*/
        assert(car->_isEndStatusOnRoad != true);
        car->moveToNextRoad(lane);//里面会更新当前车道
        out:
        /*处理池的更新*/
        delCarFromPool(car);//car已经从road里面删掉了但还要从处理池里面删掉
        car = proad->getFirstCar(_id);
        if(car != NULL)//这条路还能取到车出来的
            addCarToPool(car, cityMap);//把新的优先级最高的car加入到处理池里面
    }
}

void Cross::processEachCross(Map& cityMap)
{
    int pre=0,cur=0;
    // _carPool.clear();//先清空处理池
    if(_processNum == 0)//每个turntime第一次调度要先构建处理池
    {
        for(size_t i = 0u; i<_sortRoad.size();++i)//每次刚进一个路口就做一个（road的数量）辆车的处理池
        {
            Car *car = _sortRoad[i]->getFirstCar(_id);
            if(car != NULL)//这条路还能取到车出来的
                addCarToPool(car, cityMap);//把新的优先级最高的car加入到处理池里面
        }
    }
    for(size_t i = 0u; i<_sortRoad.size();)//路口id升序遍历
    {
        if(i>0)//保证id升序处理
            assert(_sortRoad[i]->_id>_sortRoad[i-1]->_id);
        processEachRoad(_sortRoad[i], cityMap);
        cur += _sortRoad[i]->_numOfWaitCar;//记录cross每个road的处于wait的车的和
        ++i;
        if(i == _sortRoad.size())//完成一次遍历road
        {
            i = 0;
            /*每遍历完一遍所有road判断一下是否break*/
            if(cur == pre)break;//如果这次遍历完和上次wait的车辆一样说明路口没有可以动的车了。
            else{
                pre = cur;
                cur = 0;
            }
        }
    }
    ++_processNum;//每完成一次路口处理加一
}


int8_t Cross::processStartCar(Map &cityMap, Car* car)
{
    car->_preCross = car->_curCross;
    car->_curCross = car->_nextCross;
    car->searchPath(cityMap);//_nextRoad已经更新为要出发的road  _toturn
    /*判断一下当前路段会不会很堵，jams大的话就不出来*/
    if(car->getNextRoad()->getJams(car->_curCross) > FLOAT_STARTCAR_JAM)
    {
            car->setStatusStop();
            car->_startTime = car->_startTime + 1;
            car->_preCross = car->_from;
            car->_curCross = car->_from;
            car->_nextCross = car->_from;
            car->_atRoad = NULL;
            car->_nextRoad = NULL;
            car->_turnto = isForward;
            return 0;
    }
    int lane = car->getNextRoad()->canAddToButton(car);
    assert(lane != -1);
    assert(lane != -2);
    if (lane == -3)//说明所有的车道都已放满
    {
        if(NULL==car->_atRoad)
        {//起步的车出不去，那么等待下次出去，把状态都恢复到初始化
            car->setStatusStop();
            car->_startTime = car->_startTime + 1;
            car->_preCross = car->_from;
            car->_curCross = car->_from;
            car->_nextCross = car->_from;
            car->_atRoad = NULL;
            car->_nextRoad = NULL;
            car->_turnto = isForward;
        }
        else
        {
            assert(0);
        }
        return 0;
    }
    assert(lane >= 0);
    car->getNextRoad()->addCarToRoad(car, lane);
    return 1;
}
