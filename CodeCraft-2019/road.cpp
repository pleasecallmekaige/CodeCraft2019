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

void Road::processCarInRoad() {
    for (int i = 1; i <= _channel; ++i) {
        for (int j = 0; j < _carsInRoad[_channel].size(); ++j) {

        }
    }
}

void Road::addCarToRoad(queue<Car *> &inputCarQueue) {
    inputCarQueue.front()->_distanceToCross;
}


