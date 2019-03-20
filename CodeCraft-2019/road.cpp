#include "road.h"
#include "cross.h"

/*
(道路id，道路长度，最高限速，车道数目，起始点id，终点id，是否双向)
 */

vector<Road *> Road::roads;

Road::Road(vector<int>& oneRoad)
    :_id(oneRoad[0]),
    _length(oneRoad[1]),
    _speed(oneRoad[2]),
    _limitSpeed(oneRoad[2]),
    _channel(oneRoad[3]),
    _from(oneRoad[4]),
    _to(oneRoad[5]),
    _isDuplex(oneRoad[6])
    {
        carsInRoadFromTo.resize(_channel);
        // for(int i = 0; i<_channel; ++i)
        // {
        //     carsInRoadFromTo[i].resize(_length);
        // }
        carsInRoadToFrom.resize(_channel);
        // for(int i = 0; i<_channel; ++i)
        // {
        //     carsInRoadToFrom[i].resize(_length);
        // }
    }

void Road::initRoads(Map & map) 
{
    int n = map.road.size();
    for (int i = 0; i < n; ++i) 
    {
        Road::roads.push_back(new Road(map.road[i]));
    }
}

void Road::processCarInRoad(Map & map) {
    //先处理正向行驶的车
    for (int i = 0; i < _channel; ++i) {//从第一条车道开始处理
        int len = carsInRoadFromTo[i].size();//该车道上正在行驶的车辆数
        vector<int> erase_car;//保存要驶出的车的在该车道上的index
        for (int j = 0; j < len; ++j) {
            Car* car = carsInRoadFromTo[i][j];
            if (car->_curSpeed > car->_distanceToCross) {
                //车即将驶出当前road，交给路口处理
                car->_nextCross = car->searchPath(map);
                int dif =roads[car->_nextRoad - ROAD_INDEX]->_limitSpeed - car->_distanceToCross;
                if (dif <= 0)//根据任务书中10.8-(5)Table1中的计算规则，车可能会被退回
                {
                    car->_distanceToCross = 0;
                    for (j = j+1; j < len; ++j) 
                    {//后面的车均不能通过路口
                        Car* car_j = carsInRoadFromTo[i][j];
                        car_j->_distanceToCross -= car_j->_curSpeed;//后面的车不能超过前面的车
                        if (car_j->_distanceToCross <= carsInRoadFromTo[i][j - 1]->_distanceToCross + 1)
                        {
                            car_j->_distanceToCross = carsInRoadFromTo[i][j - 1]->_distanceToCross + 1;
                            car_j->_curSpeed = min(car_j->_curSpeed, carsInRoadFromTo[i][j - 1]->_curSpeed);
                        }
                    }
                }
                /*把需要进入入口的车加入到队列中*/
                Cross::crosses[car->_nextCross - CROSS_INDEX]->addCarToQueue(car);
                erase_car.push_back(j);
                //carsInRoadFromTo[i].erase(carsInRoadFromTo[i].begin()+j);//删去即将驶出该道路的车
            }
            else {
                //车未行驶出当前road，更新车的信息,
                car->_distanceToCross -= car->_curSpeed;//后面的车不能超过前面的车
                if (j > 0 && car->_distanceToCross <= carsInRoadFromTo[i][j - 1]->_distanceToCross + 1)
                    car->_distanceToCross = carsInRoadFromTo[i][j - 1]->_distanceToCross + 1;                
            }
        }
        while (!erase_car.empty()) {
            carsInRoadFromTo[i].erase(carsInRoadFromTo[i].begin() + erase_car.back());
            erase_car.pop_back();
        }
            
    }
    if (!_isDuplex)
        return;
    //处理反向行驶的车
    for (int i = 0; i < _channel; ++i) {//从第一条车道开始处理
        int len = carsInRoadToFrom[i].size();//该车道上正在行驶的车辆数
        vector<int> erase_car;//保存要驶出的车的在该车道上的index
        for (int j = 0; j < len; ++j) {
            Car* car = carsInRoadToFrom[i][j];
            if (car->_curSpeed > car->_distanceToCross) {
                //车即将驶出当前road，交给路口处理
                car->_nextCross = car->searchPath(map);
                int dif = roads[car->_nextRoad - ROAD_INDEX]->_limitSpeed - car->_distanceToCross;
                if (dif <= 0)//根据任务书中10.8-(5)Table1中的计算规则
                {
                    car->_distanceToCross = 0;
                    continue;
                }
                Cross::crosses[car->_nextCross - CROSS_INDEX]->addCarToQueue(car);
                erase_car.push_back(j);
                //carsInRoadToFrom[i].erase(carsInRoadToFrom[i].begin()+j);//删去即将驶出该道路的车
            } else {
                //车未行驶出当前road，更新车的信息,
                car->_distanceToCross -= car->_curSpeed;//后面的车不能超过前面的车
                if (j > 0 && car->_distanceToCross <= carsInRoadToFrom[i][j - 1]->_distanceToCross + 1)
                    car->_distanceToCross = carsInRoadToFrom[i][j - 1]->_distanceToCross + 1;
            }
        }
        while (!erase_car.empty()) {
            carsInRoadToFrom[i].erase(carsInRoadToFrom[i].begin() + erase_car.back());
            erase_car.pop_back();
        }

    }
}

bool Road::addCarToRoad(Car* car, int lane) 
{
    int dif = min(car->_maxSpeed,_limitSpeed) - car->_distanceToCross;//根据任务书中10.8-(5)Table1中的计算规则

    assert(dif>0 && dif<_length);//必须大于零才正常
    assert(car->_curCross == _from || car->_curCross == _to);
    if(car->_curCross == _from)
    {
        if(carsInRoadFromTo[lane].empty())
        {
            carsInRoadFromTo[lane].push_back(car);
            car->_distanceToCross = _length - dif;
        }
        else if((_length - 1) != carsInRoadFromTo[lane].back()->_distanceToCross)
        {
            if(_length - dif <= carsInRoadFromTo[lane].back()->_distanceToCross)
                car->_distanceToCross = carsInRoadFromTo[lane].back()->_distanceToCross + 1;
            else 
                car->_distanceToCross = _length - dif;
            carsInRoadFromTo[lane].push_back(car);
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
            car->_distanceToCross = _length - dif;
        }
        else if((_length - 1) != carsInRoadToFrom[lane].back()->_distanceToCross)
        {
            if(_length - dif <= carsInRoadToFrom[lane].back()->_distanceToCross)
                car->_distanceToCross = carsInRoadToFrom[lane].back()->_distanceToCross + 1;
            else 
                car->_distanceToCross = _length - dif;
            carsInRoadToFrom[lane].push_back(car);
        }
        else
        {
            return false;
        }
    }
    car->_atRoad = _id;
    car->_atChannel = lane;
    car->_preCross = car->_curCross;
    car->_curCross = car->_nextCross;
    car->_answerPath.push_back(_id);
    car->_curSpeed = min(car->_maxSpeed,_limitSpeed);
    
    return true;
}

void Road::addCarsToRoad(queue<Car *> waiting_cars) {
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
        // Car *car = waiting_cars.front();
        // backCar(car->_id);
        waiting_cars.pop();
        cout<<"error"<<endl;
    }
}


void Road::backCar(uint32_t car_id) {
    Car::cars[car_id - CAR_INDEX]->_distanceToCross = 0;
    //还要其它信息...
}



