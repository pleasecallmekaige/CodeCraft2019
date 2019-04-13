#include "car.h"
#include "cross.h"
#include "road.h"
#include "param.h"
#include <algorithm>
#include <set>

extern uint32_t turntime;
extern int PriCarallAriveEnd;
extern int ePlaneTimeOfPriCars;

void updataMap(Map& cityMap)
{
    for(size_t i=0u; i<cityMap.road.size(); ++i)//更新路况
    {
        Road::roads[cityMap.road[i][0]]->updateRoadCondition(cityMap);
    }
    cityMap.updateMatrix();
}

bool isFinish()
{
    if(Car::numRuning == 0 && Car::numStop ==0 && Car::numStart == 0)
        return true;
    else
        return false;   
}


void driveCarInitList(bool flag, Map& cityMap)
{
    Road::runCarInInitList(cityMap);
    if(flag == false)
    {
        for(size_t i=0u; i<cityMap.road.size(); ++i)//调度路上的车
        {
            Road * proad = Road::roads[cityMap.road[i][0]];
            proad->startCar(proad->_from,false);
            proad->startCar(proad->_to,false);
        }
    }
}

void driveJustCurrentRoad(Map& cityMap)
{
    for(size_t i=0u; i<cityMap.road.size(); ++i)//调度路上的车
    {
        assert(Road::roads[cityMap.road[i][0]]->_numOfWaitCar >= 0);
        Road::roads[cityMap.road[i][0]]->driveAllCarJustOnRoadToEndStatus();
    }
}

bool driveCarInWaitStatus(Map &cityMap)
{
    int tmp = 0;
    while (Car::numWait != 0)
    {
        if(tmp == Car::numWait)
            return false;
        tmp = Car::numWait;
        for (size_t i=0u; i<cityMap.cross.size(); ++i )//调度所有路口，通过路口把路上所有优先级最高的车先调度
        {
            if(i>0 )//保证id升序处理
                assert(Cross::crosses[cityMap.cross[i][0]]->_id > Cross::crosses[cityMap.cross[i-1][0]]->_id);
            Cross::crosses[cityMap.cross[i][0]]->processEachCross(cityMap);              
        }
    }
    return true;
}

void creatCarStartVector(Map& cityMap)
{
    int limit =  INT_LIMIT_NUM_CAR;
    limit = INT_LIMIT_NUM_CAR - Car::numPresetAndPriRuning;
    for (size_t i=0u; i<Car::cars.size(); ++i )
    {
        Car* p = Car::cars[i];
        if(turntime >= p->_startTime && p->getStatus() == isStop && (Car::numRuning - Car::numPresetAndPriRuning + Car::numStart < limit || p->_preset == 1)) 
        {
            if(p->getCurCross()->addStartCar(cityMap,p) == 1)//把车加入到发车队
            {
                p->setStartTime(turntime);
                p->setStatusStart();
            }
        }
    }
    for (size_t i=0u; i<Car::pricars.size(); ++i )
    {
        Car* p = Car::pricars[i];
        if(turntime >= p->_startTime && p->getStatus() == isStop && (Car::numRuning - Car::numPresetAndPriRuning + Car::numStart < limit + 100 || p->_preset == 1)) 
        {
            if(p->getCurCross()->addStartCar(cityMap,p) == 1)//把车加入到发车队
            {
                p->setStartTime(turntime);
                p->setStatusStart();
            }
        }
    }
    for(size_t i=0u; i<cityMap.road.size(); ++i)//排序
    {
        assert(Road::roads[cityMap.road[i][0]]->_numOfWaitCar >= 0);
        Road* proad = Road::roads[cityMap.road[i][0]];
        stable_sort(proad->startFromTo.begin(), proad->startFromTo.end(), [](Car* a,Car* b){return a->_id < b->_id;});
        stable_sort(proad->startToFrom.begin(), proad->startToFrom.end(), [](Car* a,Car* b){return a->_id < b->_id;});
        stable_sort(proad->startFromTo.begin(), proad->startFromTo.end(), [](Car* a,Car* b){return a->_startTime < b->_startTime;});
        stable_sort(proad->startToFrom.begin(), proad->startToFrom.end(), [](Car* a,Car* b){return a->_startTime < b->_startTime;});
        stable_sort(proad->startFromTo.begin(), proad->startFromTo.end(), [](Car* a,Car* b){return a->_priority > b->_priority;});
        stable_sort(proad->startToFrom.begin(), proad->startToFrom.end(), [](Car* a,Car* b){return a->_priority > b->_priority;});
    }
}

/*static function*/
bool Scheduler(Map &cityMap)
{
    while(true)
    {
        creatCarStartVector(cityMap);
        driveJustCurrentRoad(cityMap);
        driveCarInitList(true, cityMap);
        if(!driveCarInWaitStatus(cityMap))
        {
            return false;
        }
        
        updataMap(cityMap);
        driveCarInitList(false, cityMap);

        updataMap(cityMap);
        // cityMap.updateMatrix();
        /*需要善后处理的
        p->_isEndStatusOnRoad = false;//每次时间片完都把所有的车设为等待；
        每个路口的_processNum要清0
        */
        for (size_t i=0u; i<Car::cars.size(); ++i )
        {
            Car* p = Car::cars[i];
            if(p->getStatus() == isRuning) 
            {
                assert(p->_atRoad != NULL);
                p->_isEndStatusOnRoad = false;//每次时间片完都把所有的车设为等待；
                p->getAtRoad()->addNumOfWaiteCar();//每次处理完都把车的状态设为等待
            }
        }
        for (size_t i=0u; i<Car::pricars.size(); ++i )
        {
            Car* p = Car::pricars[i];
            if(p->getStatus() == isRuning) 
            {
                assert(p->_atRoad != NULL);
                p->_isEndStatusOnRoad = false;//每次时间片完都把所有的车设为等待；
                p->getAtRoad()->addNumOfWaiteCar();//每次处理完都把车的状态设为等待
            }
        }
        for (size_t i=0u; i<cityMap.cross.size(); ++i )
        {
            Cross::crosses[cityMap.cross[i][0]]->_processNum = 0;
        }
		// cout<<"turntime:"<<turntime<<"  numRuning:"<<Car::numRuning
		// <<"  "<<"numStop:"<<Car::numStop<<"  numEnd:"<<Car::numEnd<<"  numPreset:"<<Car::numPresetAndPriRuning<<endl;
        if(isFinish())
        {
            break;
        }
        ++turntime;
    }
    return true;
}

double getA(Map &cityMap)
{
    set<int> tmpset;
    int numOfCar=Car::cars.size() + Car::pricars.size();//车辆总数
    int numOfPriCar=Car::pricars.size();//优先车辆总数
    int maxSpeedOfCars = (*max_element(cityMap.cars.begin(),cityMap.cars.end(),[](vector<int> a, vector<int> b) {return a[3]<b[3];} ) ) [3] ;
    int minSpeedOfCars = (*min_element(cityMap.cars.begin(),cityMap.cars.end(),[](vector<int> a, vector<int> b) {return a[3]<b[3];} ) ) [3] ;
    int maxSpeedOfPriCars = (*max_element(Car::pricars.begin(),Car::pricars.end(),[](Car* a, Car* b) {return a->_maxSpeed<b->_maxSpeed;} ) )->_maxSpeed;
    int minSpeedOfPriCars = (*min_element(Car::pricars.begin(),Car::pricars.end(),[](Car* a, Car* b) {return a->_maxSpeed<b->_maxSpeed;} ) )->_maxSpeed;
    int lPlaneTimeOfnCars = (*max_element(Car::cars.begin(),Car::cars.end(),[](Car* a, Car* b) {return a->_planeTime<b->_planeTime;} ) )->_planeTime;
    int ePlaneTimeOfnCars = (*min_element(Car::cars.begin(),Car::cars.end(),[](Car* a, Car* b) {return a->_planeTime<b->_planeTime;} ) )->_planeTime;
    int lPlaneTimeOfPriCars = (*max_element(Car::pricars.begin(),Car::pricars.end(),[](Car* a, Car* b) {return a->_planeTime<b->_planeTime;} ) )->_planeTime;
    ePlaneTimeOfPriCars = (*min_element(Car::pricars.begin(),Car::pricars.end(),[](Car* a, Car* b) {return a->_planeTime<b->_planeTime;} ) )->_planeTime;
    for(size_t i=0u; i<cityMap.cars.size(); ++i)
    {
        tmpset.insert(cityMap.cars[i][1]);
    }
    int fromfbOfCars = tmpset.size();
    tmpset.clear();
    for(size_t i=0u; i<cityMap.cars.size(); ++i)
    {
        tmpset.insert(cityMap.cars[i][2]);
    }
    int tofbOfCars = tmpset.size();
    tmpset.clear();

    for(size_t i=0u; i<Car::pricars.size(); ++i)
    {
        tmpset.insert(Car::pricars[i]->_from);
    }
    int fromfbOfPriCars = tmpset.size();
    tmpset.clear();
    for(size_t i=0u; i<Car::pricars.size(); ++i)
    {
        tmpset.insert(Car::pricars[i]->_to);
    }
    int tofbOfPriCars = tmpset.size();
    tmpset.clear();
    float l1 = (float)numOfCar/numOfPriCar;
    float l2 = ((float)maxSpeedOfCars/minSpeedOfCars) / ((float)maxSpeedOfPriCars/minSpeedOfPriCars);
    float l3 = ((float)max(lPlaneTimeOfnCars,lPlaneTimeOfPriCars)/min(ePlaneTimeOfnCars,ePlaneTimeOfPriCars)) / ((float)lPlaneTimeOfPriCars/ePlaneTimeOfPriCars);
    float l4 = (float)fromfbOfCars/fromfbOfPriCars;
    float l5 = (float)tofbOfCars/tofbOfPriCars;

    float a = l1*0.05 + (l2+l3+l4+l5)*0.2375;
    //int res = (PriCarallAriveEnd - ePlaneTimeOfPriCars)*a;
    return a;
}