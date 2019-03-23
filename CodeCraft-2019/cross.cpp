/*
(路口id,道路id,道路id,道路id,道路id)
 */
#include "cross.h"
#include "road.h"
#include "car.h"

vector<Cross *> Cross::crosses;

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

Cross::Cross(vector<int>& oneCross, vector<Road *>& roads)
   :_id(oneCross[0]),
    _processNum(0)
{
    _Road[0] = NULL;
    _Road[1] = NULL;
    _Road[2] = NULL;
    _Road[3] = NULL;
    if(oneCross[1] != -1) _Road[0] = roads[oneCross[1] - ROAD_INDEX];
    if(oneCross[2] != -1) _Road[1] = roads[oneCross[2] - ROAD_INDEX];
    if(oneCross[3] != -1) _Road[2] = roads[oneCross[3] - ROAD_INDEX];
    if(oneCross[4] != -1) _Road[3] = roads[oneCross[4] - ROAD_INDEX];

    for(int i = 0; i<4; ++i)
    {
        _sortRoad.push_back(_Road[i]);
    }
    mycomp(_sortRoad);
}

/*static function*/
void Cross::initCrosses(Map &map, vector<Road *>& roads)
{
    int n = map.cross.size();
    for(int i = 0; i<n; ++i)
    {
        Cross::crosses.push_back(new Cross(map.cross[i], roads));
    }
}

void Cross::processEachCross(Map& map)
{
    int pre=0,cur=0;
    if(_id == 31)
        int a = 9;
    // _carPool.clear();//先清空处理池
    if(_processNum == 0)//每个turntime第一次调度要先构建处理池
    {
        for(size_t i = 0u; i<_sortRoad.size();++i)//每次刚进一个路口就做一个（road的数量）辆车的处理池
        {
            Car *car = _sortRoad[i]->getFirstCar(_id);
            if(car != NULL)//这条路还能取到车出来的
                addCarToPool(car, map);//把新的优先级最高的car加入到处理池里面
        }
    }
    for(size_t i = 0u; i<_sortRoad.size();)//路口id升序遍历
    {
        processEachRoad(_sortRoad[i], map);
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


void Cross::processEachRoad(Road* proad, Map& map)
{
    Car* car = NULL;
    vector<vector<Car*>>& roadvector = (_id==proad->_from)?proad->carsInRoadToFrom:proad->carsInRoadFromTo;
    car = proad->getFirstCar(_id);
    while(1)
    {
        //car = proad->getFirstCar(_id);
        if(car == NULL)return;//这个路没有第一优先级可以出来的车
        if(car->_curCross != _id)
            int b = 9;
        assert(car->_atRoad == proad->_id);
        assert(car->_curCross == _id );
        if(car->_to == _id)//到达终点
        {
            car->moveToEnd();
            proad->driveOneChannel(roadvector[car->_atChannel]);//调整一次车道
            /*处理池的更新*/
            delCarFromPool(car);//car已经从road里面删掉了但还要从处理池里面删掉
            car = proad->getFirstCar(_id);
            if(car != NULL)//这条路还能取到车出来的
                addCarToPool(car, map);//把新的优先级最高的car加入到处理池里面
            continue;
        }
        /*处理冲突*/
        for(size_t i=0u; i<_carPool.size(); ++i)
        {
            if(_carPool[i]->_id != car->_id)
            {
                if(car->_nextRoad == _carPool[i]->_nextRoad && car->_turnto < _carPool[i]->_turnto)
                {//有别的车也去这个车的目标road且优先级比自己高（有冲突）
                    return;
                }
            }
        }
        /*下一条路能不能进，有wait状态的车辆挡着，或者三条路都堵死了*/
        if(!car->getNextRoad()->canAddToButton(car))
            return;
        
        /*处理任务书中10.8-(5)Table1中的计算规则*/
        int dif = min(car->getNextRoad()->_limitSpeed, car->_maxSpeed) - car->_distanceToCross;
        if(dif<=0)
        {
            car->_distanceToCross = 0;
            car->_isEndStatusOnRoad = true;
            car->getAtRoad()->delNumOfWaiteCar();
            proad->driveOneChannel(roadvector[car->_atChannel]);
        }
        /*这一辆车调度没有冲突*/
        car->moveToNextRoad();
        proad->driveOneChannel(roadvector[car->_atChannel]);
        /*处理池的更新*/
        delCarFromPool(car);//car已经从road里面删掉了但还要从处理池里面删掉
        car = proad->getFirstCar(_id);
        if(car != NULL)//这条路还能取到车出来的
            addCarToPool(car, map);//把新的优先级最高的car加入到处理池里面
    }
}

void Cross::addCarToPool(Car * car, Map& map)
{
    /*搜索路径算法放这里*/
    car->_nextCross = car->searchPath(map);//_nextRoad已经更新为要出发的road  _toturn
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


int8_t Cross::processStartCar(Map &map, Car* car)
{
    car->_preCross = car->_curCross;
    car->_curCross = car->_nextCross;
    car->_nextCross = car->searchPath(map);//_nextRoad已经更新为要出发的road  _toturn
    /*判断一下当前路段会不会很度堵，jams大的话就不出来*/
    if(Road::roads[car->_nextRoad - ROAD_INDEX]->getJams(car->_curCross) > 0.5f)
    {
            car->setStatusStop();
            car->_startTime = car->_startTime + 1;
            car->_preCross = car->_from;
            car->_curCross = car->_from;
            car->_nextCross = car->_from;
            car->_atRoad = -1;
            car->_nextRoad = -1;
            car->_turnto = isForward;
            return 0;
    }
    Road::roads[car->_nextRoad - ROAD_INDEX]->addNumOfWaiteCar();//因为起步车辆加入road会使numWait凭空-1;所以得先+1；
    Road::roads[car->_nextRoad - ROAD_INDEX]->addCarToRoad(car);

    return 1;
}
