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
   :_id(oneCross[0])
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

void Cross::processEachCross()
{

    for(size_t i = 0; i<_sortRoad.size(); ++i)
    {
        
    }
    //processEachRoad();
}



// bool Cross::addCarToQueue(Car *car)
// {
//     // if(car->_to == _id)
//     // {
//     //     car->setStatusEnd();
//     //     return true;
//     // }
//     if(car->_nextRoad == _outToRoad[0].roadId)
//     {
//         switch(car->_turnto)
//         {
//             case isForward: _outToRoad[0].DQueue.push(car);  break;
//             case isLeft:    _outToRoad[0].LQueue.push(car);  break;
//             case isRight:   _outToRoad[0].RQueue.push(car);  break;
//             default: assert(0); break;
//         }
//     }
//     else if(car->_nextRoad == _outToRoad[1].roadId)
//     {
//         switch(car->_turnto)
//         {
//             case isForward: _outToRoad[1].DQueue.push(car);  break;
//             case isLeft:    _outToRoad[1].LQueue.push(car);  break;
//             case isRight:   _outToRoad[1].RQueue.push(car);  break;
//             default: assert(0); break;
//         }
//     }
//     else if(car->_nextRoad == _outToRoad[2].roadId)
//      {
//         switch(car->_turnto)
//         {
//             case isForward: _outToRoad[2].DQueue.push(car);  break;
//             case isLeft:    _outToRoad[2].LQueue.push(car);  break;
//             case isRight:   _outToRoad[2].RQueue.push(car);  break;
//             default: assert(0);  break;
//         }
//     }
//     else if(car->_nextRoad == _outToRoad[3].roadId)
//     {
//         switch(car->_turnto)
//         {
//             case isForward: _outToRoad[3].DQueue.push(car);  break;
//             case isLeft:    _outToRoad[3].LQueue.push(car);  break;
//             case isRight:   _outToRoad[3].RQueue.push(car);  break;
//             default: assert(0); break;
//         }
//     }
//     else
//     {
//         assert(0);
//         return false;//表示没有找到路返回失败
//     }
//     return true;
// }

// int8_t Cross::processStartCar(Map &map, Car* car)
// {
//     car->_preCross = car->_curCross;
//     car->_curCross = car->_nextCross;
//     car->_nextCross = car->searchPath(map);//_nextRoad已经更新为要出发的road  _toturn
//     /*判断一下当前路段会不会很度堵，jams大的话就不出来*/
//     if(Road::roads[car->_nextRoad - ROAD_INDEX]->getJams(car->_curCross) > 0.3f)
//     {
//             car->setStatusStop();
//             car->_startTime = car->_startTime + 1;
//             car->_preCross = car->_from;
//             car->_curCross = car->_from;
//             car->_nextCross = car->_from;
//             car->_nextRoad = -1;
//             car->_turnto = isForward;
//             return 0;
//     }
//     if(car->_nextRoad == _outToRoad[0].roadId)
//         _outToRoad[0].startQueue.push(car);
//     else if(car->_nextRoad == _outToRoad[1].roadId)
//         _outToRoad[1].startQueue.push(car);
//     else if(car->_nextRoad == _outToRoad[2].roadId)
//         _outToRoad[2].startQueue.push(car);
//     else if(car->_nextRoad == _outToRoad[3].roadId)
//         _outToRoad[3].startQueue.push(car);
//     else
//     {
//         assert(0);//表示没有找到路返回失败
//     }
//     return 1;
// }


// int8_t Cross::outputCarToRoad()
// {
//     for(int i = 0; i<4; ++i)
//     {
//         if(_outToRoad[i].roadId == -1)continue;//把没有路的排除掉
//         Road *road = Road::roads[_outToRoad[i].roadId - ROAD_INDEX];
//         int16_t lenToCross = 0;
//         while(!_outToRoad[i].DQueue.empty() || !_outToRoad[i].LQueue.empty() || !_outToRoad[i].RQueue.empty())
//         {
//             while(!_outToRoad[i].DQueue.empty() &&  _outToRoad[i].DQueue.front()->_distanceToCross == lenToCross)//DQueue中第一排的
//             {
//                 _outToRoad[i].outQueue.push(_outToRoad[i].DQueue.front());//先调度直走的第一排的车辆
//                 _outToRoad[i].DQueue.pop();
//             }
//             while(!_outToRoad[i].LQueue.empty() &&  _outToRoad[i].LQueue.front()->_distanceToCross == lenToCross)//LQueue中第一排的
//             {
//                 _outToRoad[i].outQueue.push(_outToRoad[i].LQueue.front());//调度左拐的第一排的车辆
//                 _outToRoad[i].LQueue.pop();
//             }
//             while(!_outToRoad[i].RQueue.empty() &&  _outToRoad[i].RQueue.front()->_distanceToCross == lenToCross)//RQueue中第一排的
//             {
//                 _outToRoad[i].outQueue.push(_outToRoad[i].RQueue.front());//调度右拐的第一排的车辆
//                 _outToRoad[i].RQueue.pop();
//             }
//             ++lenToCross;
//         }
//         while(!_outToRoad[i].startQueue.empty())
//         {
//             _outToRoad[i].outQueue.push(_outToRoad[i].startQueue.front());
//             _outToRoad[i].startQueue.pop();
//         }
//         assert(_outToRoad[i].DQueue.empty() && _outToRoad[i].LQueue.empty() && _outToRoad[i].RQueue.empty() && _outToRoad[i].startQueue.empty());
//         road->addCarsToRoad(_outToRoad[i].outQueue); //把入口内的车输出到对应的road上
//         assert(_outToRoad[i].outQueue.empty());
//     }
//     return 0;
// }
