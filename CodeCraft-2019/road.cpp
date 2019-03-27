#include "road.h"
#include "cross.h"
#include "car.h"
#include "param.h"

/*
(道路id，道路长度，最高限速，车道数目，起始点id，终点id，是否双向)
 */

map<int, Road*>  Road::roads;
int Road::numOfCarInRoads = 0;

Road::Road(vector<int>& oneRoad, int index)
    :_id(oneRoad[0]),
     _index(index),
    _length(oneRoad[1]),
    _limitSpeed(oneRoad[2]),
    _channel(oneRoad[3]),
    _from(oneRoad[4]),
    _to(oneRoad[5]),
    _isDuplex(oneRoad[6]),
    _jamsFromTo(0.0f),
    _carNumFromTo(0),
    _jamsToFrom(0.0f),
    _carNumToFrom(0),
    _numOfWaitCar(0)
    {
        carsInRoadFromTo.resize(_channel);
        carsInRoadToFrom.resize(_channel);
    }

void Road::initRoads(Map & cityMap) 
{
    int n = cityMap.road.size();
    for (int i = 0; i < n; ++i) 
    {
        Road * road = new Road(cityMap.road[i], i);
        Road::roads.insert(map<int, Road *>::value_type(road->_id, road));
    }
}

void Road::driveOneChannel(vector<Car *>& oneChannel)
{
    if(oneChannel.empty())return;
    for (size_t i = 0u; i < oneChannel.size(); ++i)
    {
        Car* car = oneChannel[i];
        if(car->_isEndStatusOnRoad == true)continue;
        if(0==i || (car->_distanceToCross - car->_curSpeed) > oneChannel[i-1]->_distanceToCross)//没有阻挡
        {
            if(car->_curSpeed > car->_distanceToCross)//能行驶出路口
            {
                assert(0 == i);
                assert(car->_isEndStatusOnRoad == false);
                car->_isEndStatusOnRoad = false;
            }
            else//不会出路口
            {
                car->_distanceToCross -= car->_curSpeed;
                assert(car->_distanceToCross>=0);
                assert(car->_isEndStatusOnRoad == false);
                car->_isEndStatusOnRoad = true;
                --_numOfWaitCar;
                --Car::numWait;
            }
        }
        else//有车辆阻挡
        {
            if(oneChannel[i-1]->_isEndStatusOnRoad == false)//前方车辆为等待车辆
            {
                assert(car->_isEndStatusOnRoad == false);
                car->_isEndStatusOnRoad = false;
            }
            else//前方车辆为终止车辆
            {
                assert(oneChannel[i-1]->_distanceToCross + 1 <= car->_distanceToCross);
                car->_distanceToCross = oneChannel[i-1]->_distanceToCross + 1;
                assert(car->_distanceToCross<_length);
                assert(car->_isEndStatusOnRoad == false);
                car->_isEndStatusOnRoad = true;
                --_numOfWaitCar;
                --Car::numWait;
            }
        }
    }
}

void Road::driveAllCarJustOnRoadToEndStatus()
{
    //先处理正向行驶的车
    for (int lane = 0; lane < _channel; ++lane)
    {
        driveOneChannel(carsInRoadFromTo[lane]);
    }/*正向处理完*/
    if (!_isDuplex)//单车道只处理正向的
        return;
    //处理反向行驶的车
    for (int lane = 0; lane < _channel; ++lane)
    {
        driveOneChannel(carsInRoadToFrom[lane]);
    }/*反向处理完*/
}


/*获取每个road的优先级第一的车，没有可出来的车就返回NULL*/
Car* Road::getFirstCar(int16_t curCross)
{
    /*选择方向*/
    assert(curCross == _from || curCross == _to);
    vector<vector<Car*>>& roadvector = (curCross==_from)?carsInRoadToFrom:carsInRoadFromTo;
    for (int row = 0; row < _length; ++row)
    {//从第一列开始处理
        for (int lane = 0; lane < _channel; ++lane)
        {
            if(roadvector[lane].empty())continue;
            Car* car = roadvector[lane][0];
            if(car->_distanceToCross == row && car->_isEndStatusOnRoad == false)
            {//按照列优先取到第一辆为wait的车
                    assert(car->_atChannel == lane);
                    return car;
            }
        }
    }
    return NULL;
}

void Road::addNumOfWaiteCar()
{
    ++_numOfWaitCar;
    ++Car::numWait;
}
void Road::delNumOfWaiteCar()
{
    --_numOfWaitCar;
    --Car::numWait;
}

/*会更新车的大量状态*/
void Road::addCarToRoad(Car* car, int lane) {
    int dif = min(car->_maxSpeed,_limitSpeed) - car->_distanceToCross;//根据任务书中10.8-(5)Table1中的计算规则
    assert(dif>0);//必须大于零才正常
    assert(dif<_length);
    assert(car->_curCross == _from || car->_curCross == _to);
    vector<vector<Car*>>& toVector = (car->_curCross==_from)?carsInRoadFromTo:carsInRoadToFrom;

    if(toVector[lane].empty())
    {
        toVector[lane].push_back(car);
        car->_distanceToCross = _length - dif;
    }
    else if((_length - 1) != toVector[lane].back()->_distanceToCross)
    {
        if(_length - dif <= toVector[lane].back()->_distanceToCross)
        {
            assert(toVector[lane].back()->_isEndStatusOnRoad == true);
            car->_distanceToCross = toVector[lane].back()->_distanceToCross + 1;
        }
        else 
            car->_distanceToCross = _length - dif;
        toVector[lane].push_back(car);
    }
    else
    {
        return;
    }

    if(car->_curCross==_from)
        ++_carNumFromTo;
    else
        ++_carNumToFrom;
    //addNumOfCarInRoads();
    car->_atRoad = this;
    assert(lane < _channel);
    car->_atChannel = lane;
    car->_preCross = car->_curCross;
    car->_curCross = car->_nextCross;
    car->_answerPath.push_back(_id);
    car->_curSpeed = min(car->_maxSpeed,_limitSpeed);
}

void Road::outCarToRoad(Car * car)
{
    vector<vector<Car*>>& toVector = (car->_curCross==_from)?carsInRoadToFrom:carsInRoadFromTo;
    if(car->_curCross==_from)
        --_carNumToFrom;
    else
        --_carNumFromTo;
    assert(toVector[car->_atChannel][0] == car);
    toVector[car->_atChannel].erase(toVector[car->_atChannel].begin());//删除第一个元素
    assert(car->_isEndStatusOnRoad == false);
    car->_isEndStatusOnRoad = true;
    --_numOfWaitCar;
    --Car::numWait;
    //outNumOfCarInRoads();
}

int Road::canAddToButton(const Car* car)
{
    vector<vector<Car*>>& toVector = (car->_curCross==_from)?carsInRoadFromTo:carsInRoadToFrom;
    int dif = min(car->_maxSpeed,_limitSpeed) - car->_distanceToCross;//根据任务书中10.8-(5)Table1中的计算规则
    if(dif<=0)return -2;
    for(int lane = 0; lane<_channel; ++lane)
    {
        if(toVector[lane].empty())return lane;
        Car * backCar = toVector[lane].back();
        if(_length - dif <= backCar->_distanceToCross)//有阻挡
        {
            if(backCar->_isEndStatusOnRoad == false)//阻挡车辆处于wait状态
                return -1;
            else if(backCar->_distanceToCross == _length-1)//阻挡车辆处于end状态但在最后一行
                continue;//换行
            else//阻挡车辆处于end状态且不在最后一行
                return lane;
        }
        else
        {
            return lane;
        }
        assert(0);
    }
    return -3;
}



void Road::updateRoadCondition()
{
    assert(_channel*_length != 0);
    _jamsFromTo = (float)_carNumFromTo/(float)((int)_channel*(int)_length);
    _jamsToFrom = (float)_carNumToFrom/(float)((int)_channel*(int)_length);
}

float Road::getJams(int _curCross)
{
    assert(_curCross==_from || _curCross==_to);
    float jams = (_curCross == _from)?_jamsFromTo:_jamsToFrom;
    assert(jams<=1);
    if(jams > 0.99)
        jams = 0.99;
    return jams;
}





