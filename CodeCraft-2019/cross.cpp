/*
(路口id,道路id,道路id,道路id,道路id)
 */
#include "cross.h"
#include "road.h"

vector<Cross *> Cross::crosses;

Cross::Cross(vector<int>& oneCross)
   :_id(oneCross[0])
{
    _outToRoad[0].roadId = oneCross[1];
    _outToRoad[1].roadId = oneCross[2];
    _outToRoad[2].roadId = oneCross[3];
    _outToRoad[3].roadId = oneCross[4];
}

/*static function*/
void Cross::initCrosses(Map &map)
{
    int n = map.cross.size();
    for(int i = 0; i<n; ++i)
    {
        Cross::crosses.push_back(new Cross(map.cross[i]));
    }
}

bool Cross::addCarToQueue(Car *car)
{
    if(car->_nextRoad == _outToRoad[0].roadId)
    {
        switch(car->_turnto)
        {
            case isForward: _outToRoad[0].DQueue.push(car);  break;
            case isLeft:    _outToRoad[0].LQueue.push(car);  break;
            case isRight:   _outToRoad[0].RQueue.push(car);  break;
            default:
                break;
        }
    }
    else if(car->_nextRoad == _outToRoad[1].roadId)
    {
        switch(car->_turnto)
        {
            case isForward: _outToRoad[1].DQueue.push(car);  break;
            case isLeft:    _outToRoad[1].LQueue.push(car);  break;
            case isRight:   _outToRoad[1].RQueue.push(car);  break;
            default:
                break;
        }
    }
    else if(car->_nextRoad == _outToRoad[2].roadId)
     {
        switch(car->_turnto)
        {
            case isForward: _outToRoad[2].DQueue.push(car);  break;
            case isLeft:    _outToRoad[2].LQueue.push(car);  break;
            case isRight:   _outToRoad[2].RQueue.push(car);  break;
            default:
                break;
        }
    }
    else if(car->_nextRoad == _outToRoad[3].roadId)
    {
        switch(car->_turnto)
        {
            case isForward: _outToRoad[3].DQueue.push(car);  break;
            case isLeft:    _outToRoad[3].LQueue.push(car);  break;
            case isRight:   _outToRoad[3].RQueue.push(car);  break;
            default:
                break;
        }
    }
    else
        return false;//表示没有找到路返回失败
    return true;
}

int8_t Cross::processStartCar(Map &map, Car* car)
{
    car->_preCross = car->_curCross;
    car->_curCross = car->_nextCross;
    car->_nextCross = car->searchPath(map);//_nextRoad已经更新为要出发的road  _toturn

    if(car->_nextRoad == _outToRoad[0].roadId)
        _outToRoad[0].startQueue.push(car);
    else if(car->_nextRoad == _outToRoad[1].roadId)
        _outToRoad[1].startQueue.push(car);
    else if(car->_nextRoad == _outToRoad[2].roadId)
        _outToRoad[2].startQueue.push(car);
    else if(car->_nextRoad == _outToRoad[3].roadId)
        _outToRoad[3].startQueue.push(car);
    else
        return -1;//表示没有找到路返回失败
    return 1;
}


int8_t Cross::outputCar()
{
    for(int i = 0; i<4; ++i)
    {
        Road *p = Road::roads[_outToRoad[i].roadId - ROAD_INDEX];
        int16_t lenToCross = 0;
        while(!_outToRoad[i].DQueue.empty() || !_outToRoad[i].LQueue.empty() || !_outToRoad[i].RQueue.empty())
        {
            while(!_outToRoad[i].DQueue.empty() &&  _outToRoad[i].DQueue.front()->_distanceToCross == lenToCross)//DQueue中第一排的
            {
                _outToRoad[i].outQueue.push(_outToRoad[i].DQueue.front());//先调度直走的第一排的车辆
                _outToRoad[i].DQueue.pop();
            }
            while(!_outToRoad[i].LQueue.empty() &&  _outToRoad[i].LQueue.front()->_distanceToCross == lenToCross)//LQueue中第一排的
            {
                _outToRoad[i].outQueue.push(_outToRoad[i].LQueue.front());//调度左拐的第一排的车辆
                _outToRoad[i].LQueue.pop();
            }
            while(!_outToRoad[i].RQueue.empty() &&  _outToRoad[i].RQueue.front()->_distanceToCross == lenToCross)//RQueue中第一排的
            {
                _outToRoad[i].outQueue.push(_outToRoad[i].RQueue.front());//调度右拐的第一排的车辆
                _outToRoad[i].RQueue.pop();
            }
            ++lenToCross;
        }
        while(!_outToRoad[i].startQueue.empty())
        {
            _outToRoad[i].outQueue.push(_outToRoad[i].startQueue.front());
            _outToRoad[i].startQueue.pop();
        }
        p->addCarsToRoad(_outToRoad[i].outQueue); //把入口内的车输出到对应的road上
    }
    return 0;
}
