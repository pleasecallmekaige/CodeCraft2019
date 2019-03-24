#include "car.h"
#include "cross.h"
#include "road.h"
#include <stdlib.h>
#include <time.h>

extern uint32_t turntime;
/*
(车辆id，始发地、目的地、最高速度、出发时间)
 */
const TURN TURNTABLE[4][4]  =   {{ none,     isRight,  isForward,isLeft    },
                                 { isLeft,   none,     isRight,  isForward },
                                 { isForward,isLeft,   none,     isRight   },
                                 { isRight,  isForward,isLeft,   none      }
                                  };

vector<Car *> Car::cars;

int Car::numStop = 0;
int Car::numRuning = 0;
int Car::numEnd = 0;
int Car::numALL = 0;
int Car::numWait = 0;

Car::Car(std::vector<int> &res)//res就是car.txt的一行
       :_id(res[0]),
        _from(res[1]),
        _to(res[2]),
        _maxSpeed(res[3]),
        _planeTime(res[4]),
        _startTime(_planeTime),
        _preCross(res[1]),
        _curCross(res[1]),
        _nextCross(res[1]),
        _turnto(isForward),
        _status(isStop),
        _curSpeed(_maxSpeed),
        _atRoad(NULL),//初始化时没有路，是在路口上
        _nextRoad(NULL),
        _atChannel(0),
        _distanceToCross(0),
        _priority(0),
        _isEndStatusOnRoad(false)
    {     
    }


TURN Car::whereToTurn(Road* atRoad, Road* nextRoad, vector<int>& nextRoadvector)
{
    int f=-1,t=-1;
    if(NULL == atRoad)
        return isForward;
    for(int i = 1; i<=4; ++i)//第0个是crossid，从第一个开始
    {
        if(atRoad->_id == nextRoadvector[i])
            f = i;
        if(nextRoad->_id == nextRoadvector[i])
            t = i;
    }
    return TURNTABLE[t-1][f-1];
}

void Car::moveToNextRoad()
{
    Road* pnextRoad = _nextRoad;
    Road* patRoad = _atRoad;
    vector<vector<Car*>>& fromVector = (_curCross==patRoad->_from)?patRoad->carsInRoadToFrom:patRoad->carsInRoadFromTo;
    // vector<vector<Car*>>& toVector = (_curCross==pnextRoad->_from)?pnextRoad->carsInRoadFromTo:pnextRoad->carsInRoadToFrom;
    patRoad->outCarToRoad(this); 
    patRoad->driveOneChannel(fromVector[this->_atChannel]);  
    pnextRoad->addCarToRoad(this);//更新车的所有状态
}

void Car::moveToEnd()
{
    Road* patRoad = _atRoad;
    patRoad->outCarToRoad(this);
    this->setStatusEnd();
}

int Car::getShortestDistance(Map &cityMap)
{
    vector<int> nextRoad;
    vector<int> trueNextRoad;
    vector<int> nextCross;
    int resCross = -1;
    int resRoad = -1;
    nextRoad = cityMap.cross[Cross::crosses[_curCross]->_index];
    for(int i = 1; i<=4; ++i)//第0个是crossid，从第一个开始
    {
        if(nextRoad[i] != -1)
        {
            /*处理单向通行的road*/
            if(cityMap.road[Road::roads[nextRoad[i]]->_index][6] == 1)
            {//查找下一个相邻的路口
                nextCross.push_back((cityMap.road[Road::roads[nextRoad[i]]->_index][4] == _curCross)? cityMap.road[Road::roads[nextRoad[i]]->_index][5]:cityMap.road[Road::roads[nextRoad[i]]->_index][4]);
                trueNextRoad.push_back(nextRoad[i]);
            }
            else if(cityMap.road[Road::roads[nextRoad[i]]->_index][6] == 0 && _curCross == cityMap.road[Road::roads[nextRoad[i]]->_index][4])
            {
                nextCross.push_back(cityMap.road[Road::roads[nextRoad[i]]->_index][5]);
                trueNextRoad.push_back(nextRoad[i]);
            }
        }
    }
    int score = INT32_MAX;
    for(size_t i = 0u; i<nextCross.size(); ++i)
    {
        int distance = cityMap.map[Cross::crosses[nextCross[i]]->_index][Cross::crosses[_to]->_index] + cityMap.map[Cross::crosses[_curCross]->_index][Cross::crosses[nextCross[i]]->_index]; 
        distance = getScore(distance, _curCross, trueNextRoad[i]);
        if(distance < score && nextCross[i] != _preCross)
        {//出现距离更小的路，且没有掉头返回上一个路口（车辆不允许掉头）
            score = distance;
            resCross = nextCross[i];
            resRoad = trueNextRoad[i];
        }
    }
    assert(resRoad != -1);//没有找到下个路
    assert(resCross != -1);//没有找到下个路口

    return score;
}

int Car::searchPath(Map &cityMap)
{
    vector<int> nextRoad;
    vector<int> trueNextRoad;
    vector<int> nextCross;
    int resCross = -1;
    int resRoad = -1;
    nextRoad = cityMap.cross[Cross::crosses[_curCross]->_index];
    for(int i = 1; i<=4; ++i)//第0个是crossid，从第一个开始
    {
        if(nextRoad[i] != -1)
        {
            /*处理单向通行的road*/
            if(cityMap.road[Road::roads[nextRoad[i]]->_index][6] == 1)
            {//查找下一个相邻的路口
                nextCross.push_back((cityMap.road[Road::roads[nextRoad[i]]->_index][4] == _curCross)? cityMap.road[Road::roads[nextRoad[i]]->_index][5]:cityMap.road[Road::roads[nextRoad[i]]->_index][4]);
                trueNextRoad.push_back(nextRoad[i]);
            }
            else if(cityMap.road[Road::roads[nextRoad[i]]->_index][6] == 0 && _curCross == cityMap.road[Road::roads[nextRoad[i]]->_index][4])
            {
                nextCross.push_back(cityMap.road[Road::roads[nextRoad[i]]->_index][5]);
                trueNextRoad.push_back(nextRoad[i]);
            }
        }
    }
    int score = INT32_MAX;
    for(size_t i = 0u; i<nextCross.size(); ++i)
    {
        int distance = cityMap.map[Cross::crosses[nextCross[i]]->_index][Cross::crosses[_to]->_index] + cityMap.map[Cross::crosses[_curCross]->_index][Cross::crosses[nextCross[i]]->_index]; 
        distance = getScore(distance, _curCross, trueNextRoad[i]);
        if(distance < score && nextCross[i] != _preCross)
        {//出现距离更小的路，且没有掉头返回上一个路口（车辆不允许掉头）
            score = distance;
            resCross = nextCross[i];
            resRoad = trueNextRoad[i];
        }
    }
    /*更新目标road*/
    assert(resRoad != -1);//没有找到下个路
    assert(resCross != -1);//没有找到下个路口
    if(_curCross == _to)//特殊处理一下到达终点的车辆的搜索 判定为直行，
    {
        int f;
        for(int i = 1; i<=4; ++i)//第0个是crossid，从第一个开始
        {
            if(_atRoad->_id == nextRoad[i])
            {
                f = i;//获取到atRoad的索引号
                break;
            }
        }
        f = (f+1)%4+1; //获取对面Road的索引号
        if(nextRoad[f] == -1)
            _nextRoad = NULL;
        else
            _nextRoad = Road::roads[nextRoad[f]];
        _turnto = isForward;
        return _curCross;
    }
    _nextRoad = Road::roads[resRoad];

    /*更新turnto*/
    _turnto = whereToTurn(_atRoad, _nextRoad, nextRoad);
    return resCross;
}

int Car::getScore(int distance, int _curCross, int nextRoadId)
{
    Road* proad = Road::roads[nextRoadId];
    float jams = proad->getJams(_curCross);
    assert(jams<1);
    //if(jams<0.2)jams=0;
    if(jams>=0.8)jams=0.79999;
    int a = distance/(1.2-jams);
    return a;
}

/*static function*/
void Car::Scheduler(Map &cityMap)
{
    do
    {
        for(size_t i=0u; i<cityMap.road.size(); ++i)//调度路上的车
        {
            assert(Road::roads[cityMap.road[i][0]]->_numOfWaitCar >= 0);
            Road::roads[cityMap.road[i][0]]->driveAllCarJustOnRoadToEndStatus();
        }

        for (size_t i=0u; i<cityMap.cross.size(); ++i )//调度所有路口，通过路口把路上所有优先级最高的车先调度
        {
            Cross::crosses[cityMap.cross[i][0]]->processEachCross(cityMap);              
        }
        //cout<<"WaitCar: "<<Car::numWait<<endl;
    } while (Car::numWait != 0);
    
    for(size_t i=0u; i<cityMap.road.size(); ++i)//更新路况
    {
        Road::roads[cityMap.road[i][0]]->updateRoadCondition();
    }

    /*起步车辆最后加入入口*/
    for (int i=0; i<Car::numALL; ++i )//把启动车辆加入入口
    {
        Car* p = cars[i];
        if(turntime >= p->_startTime && p->getStatus() == isStop && Car::numRuning <2000)
        {
            p->setStatusRuning();
            p->setStartTime(turntime);
            Cross::crosses[p->_from]->processStartCar(cityMap, p);
        }
     
        if(p->getStatus() == isRuning) 
        {
            assert(p->_atRoad != NULL);
            p->_isEndStatusOnRoad = false;//每次时间片完都把所有的车设为等待；
            p->getAtRoad()->addNumOfWaiteCar();//每次处理完都把车的状态设为等待
        }
    }
    /*需要善后处理的
    p->_isEndStatusOnRoad = false;//每次时间片完都把所有的车设为等待；
    每个路口的_processNum要清0
    */
    for (size_t i=0u; i<cityMap.cross.size(); ++i )
    {
        Cross::crosses[cityMap.cross[i][0]]->_processNum = 0;
    }
}

void swap(Car*& a, Car*& b)
{
    Car* tmp = a;
    a = b;
    b = tmp;
}
void Partition(vector<Car*>& data,int begin,int end)
{
    if(end-begin<=0)
        return;
    int l = begin;
    int r = end;
    int small = l-1;
    while(l<r)
    {
        if(data[l]->_priority < data[end]->_priority)
        {
            small++;
            if(small!=l)
            {
              swap(data[small],data[l]);
            }
        }
        l++;
    }
    small++;
    swap(data[small],data[end]);

    Partition(data,begin,small-1);
    Partition(data,small+1,end);
}
void qiuckSort(vector<Car*>& data,int length)
{
    if(length<=0)
        return;
    Partition(data,0,length-1);
}

#define MAX 400

/*static function*/
void Car::initCars(string file, Map &cityMap)
{
	readCars(file, cityMap);
    qiuckSort(cars, cars.size());
    srand((unsigned)time(0));  
    for (size_t i=0u; i<cars.size(); ++i )
    {
        cars[i]->_startTime = cars[i]->_startTime + (int)MAX * (i/cars.size()) * rand() / (RAND_MAX + 1);
    }
}

/*读入car.txt文件 static function*/
void Car::readCars(string file, Map &cityMap)
{
    ifstream infile; 
    infile.open(file.data());   //将文件流对象与文件连接起来 
    assert(infile.is_open());   //若失败,则输出错误消息,并终止程序运行 
	int result;
    string s;
	string t;
	getline(infile,s);  //#
    while(getline(infile,s))
    {
		stringstream input(s.substr(1,s.length()-2));
		vector<int> res;
		while(input>>result){
			input>>t;
			res.push_back(result);
		}
		cars.push_back(new Car(res));
        cars.back()->_priority = cars.back()->getShortestDistance(cityMap)/cars.back()->_maxSpeed;
    }
    infile.close();             //关闭文件输入流 
}