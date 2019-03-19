#include<iostream>
#include "lib\road.h"

/*
(道路id，道路长度，最高限速，车道数目，起始点id，终点id，是否双向)
 */

vector<Road *> roads;

Road::Road(vector<int>& oneRoad)
    :_id(oneRoad[0]),
    _length(oneRoad[1]),
    _speed(oneRoad[2]),
    _limitSpeed(oneRoad[2]),
    _channel(oneRoad[3]),
    _from(oneRoad[4]),
    _to(oneRoad[5]),
    _isDuplex(oneRoad[6])
    {}

void Road::initRoads(Map & map) {
    int n = map.road.size();
    for (int i = 0; i < n; ++i) {
        Road::roads.push_back(new Road(map.road[i]));
    }
}

void Road::processCarInRoad(Map & map) {
    //先处理正向行驶的车
        for (int i = 0; i < _channel; ++i) {//从第一条车道开始处理
            for (int j = 0; j < carsInRoadFromTo[_channel].size(); ++j) {
                Car* car = Car::cars[carsInRoadFromTo[i][j] - CAR_INDEX];
                if (car->_curSpeed > car->_distanceToCross) {
                    //车即将驶出当前road，交给路口处理
                    car->_nextCross = car->searchPath(map);
                    switch (car->_turnto) {
                        case isLeft:Cross::crosses[car->_nextCross]->_outToRoad[_id].LQueue.push(car); break;
                        case isRight:Cross::crosses[car->_nextCross]->_outToRoad[_id].RQueue.push(car); break;
                        case isForward:Cross::crosses[car->_nextCross]->_outToRoad[_id].DQueue.push(car); break;
                        default:
                            break;
                    }
                    carsInRoadFromTo[i].erase(carsInRoadFromTo[i].begin()+j);
                } else {
                    //车未行驶出当前road，更新车的信息
                    Car::cars[car->_id - CAR_INDEX]->_distanceToCross -= car->_curSpeed;
                }
            }
        }
        if (!_isDuplex)
            return;
        //处理反向行驶的车

        for (int i = 0; i < _channel; ++i) {//从第一条车道开始处理
            for (int j = 0; j < carsInRoadToFrom[_channel].size(); ++j) {
                Car* car = Car::cars[carsInRoadToFrom[i][j] - CAR_INDEX];
                if (car->_curSpeed > car->_distanceToCross) {
                    //车即将驶出当前road，交给路口处理
                    car->_nextCross = car->searchPath(map);
                    switch (car->_turnto) {
                    case isLeft:Cross::crosses[car->_nextCross]->_outToRoad[_id].LQueue.push(car); break;
                    case isRight:Cross::crosses[car->_nextCross]->_outToRoad[_id].RQueue.push(car); break;
                    case isForward:Cross::crosses[car->_nextCross]->_outToRoad[_id].DQueue.push(car); break;
                    default:
                        break;
                    }
                    carsInRoadToFrom[i].erase(carsInRoadToFrom[i].begin() + j);
                } else {
                    //车未行驶出当前road，更新车的信息
                    Car::cars[car->_id - CAR_INDEX]->_distanceToCross -= car->_curSpeed;
                }
            }
        }
}

void Road::addCarToRoad(uint32_t carId, int lane) {
    Car *car = Car::cars[carId - CAR_INDEX];
    carsInRoadFromTo[lane].push_back(carId);
    Car::cars[carId - CAR_INDEX]->_atRoad = _id;
    Car::cars[carId - CAR_INDEX]->_curCross = _to;
    int dif = _limitSpeed - car->_distanceToCross;
    if (dif > 0)//根据任务书中10.8-(5)Table1中的计算规则
        Car::cars[carId - CAR_INDEX]->_distanceToCross = _length - dif;
    else {
        //将车退回原路口
        backCar(car->_id);
    }
        
}

void Road::addCarsToRoad(std::queue<Car*>& waiting_cars) {
    while (!waiting_cars.empty()) {
        Car *car = waiting_cars.front();
        waiting_cars.pop();
        int i = 0;
        for(; i < _channel; ++i) //道路编号小的依次放
            if (!isFullLane(i)) {//如果放当前车道放满了就放下一个车道
                addCarToRoad(car->_id, i);
                break;
            }
        if (i == _channel)//说明所有的车道都已放满
            break;
    }
    while (!waiting_cars.empty()) {//将车退回原road上，并将到路口的距离设为0，等待下一次启动
        Car *car = waiting_cars.front();
        backCar(car->_id);
        waiting_cars.pop();
    }
}

bool Road::isFullLane(int lane) {
    //考虑当前lane上最后一辆车到路口的距离，考虑车的单位是1，若放满则位length-1
    if (Car::cars[carsInRoadFromTo[lane].back() - CAR_INDEX]->_distanceToCross == _length - 1)
        return true;
    return false;
}

void Road::backCar(uint32_t car_id) {
    Car::cars[car_id - CAR_INDEX]->_distanceToCross = 0;
    //还要其它信息...
}



