/*
(路口id,道路id,道路id,道路id,道路id)
 */
#include "cross.h"

vector<Cross *> Cross::crosses;

Cross::Cross(vector<int>& oneCross)
   :id(oneCross[0])
{
    outToRoad[0].roadId = oneCross[1];
    outToRoad[1].roadId = oneCross[2];
    outToRoad[2].roadId = oneCross[3];
    outToRoad[3].roadId = oneCross[4];
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

int8_t Cross::processStartCar(Map &map, Car* car)
{
    car->_preCross = car->_curCross;
    car->_curCross = car->_nextCross;
    car->_nextCross = car->searchPath(map);//_nextRoad已经更新为要出发的road  _toturn

    if(car->_nextRoad == outToRoad[0].roadId)
        outToRoad[0].startQueue.push(car);
    else if(car->_nextRoad == outToRoad[1].roadId)
        outToRoad[1].startQueue.push(car);
    else if(car->_nextRoad == outToRoad[2].roadId)
        outToRoad[2].startQueue.push(car);
    else if(car->_nextRoad == outToRoad[3].roadId)
        outToRoad[3].startQueue.push(car);
    else
        return -1;//表示没有找到路返回失败
    return 1;
}
