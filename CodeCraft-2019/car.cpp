#include "car.h"
#include "cross.h"
#include "road.h"

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
        _atRoad(-1),//初始化时没有路，是在路口上
        _nextRoad(-1),
        _atChannel(1),
        _distanceToCross(0)
        {}

TURN whereToTurn(int &atRoad, int &nextRoad, vector<int>& nextRoadvector)
{
    int f=-1,t=-1;
    if(-1 == atRoad)
        return isForward;
    for(int i = 1; i<=4; ++i)//第0个是crossid，从第一个开始
    {
        if(atRoad == nextRoadvector[i])
            f = i;
        if(nextRoad == nextRoadvector[i])
            t = i;
    }
    return TURNTABLE[t-1][f-1];
}

int Car::getScore(int distance, int _curCross, int nextRoadId)
{
    Road* proad = Road::roads[nextRoadId - ROAD_INDEX];
    assert(_curCross==proad->_from || _curCross==proad->_to);
    float jams = (_curCross == proad->_from)?proad->_jamsFromTo:proad->_jamsToFrom;
    assert(jams<1);
    //if(jams>=0.8)jams=0.79999;

    return distance/(1-jams);
}

int Car::searchPath(Map &map)
{
    vector<int> nextRoad;
    vector<int> trueNextRoad;
    vector<int> nextCross;
    int resCross = -1;
    int resRoad = -1;

    nextRoad = map.cross[_curCross-1];
    for(int i = 1; i<=4; ++i)//第0个是crossid，从第一个开始
    {
        if(nextRoad[i] != -1)
        {
            /*处理单向通行的road*/
            if(map.road[nextRoad[i]-ROAD_INDEX][6] == 1)
            {//查找下一个相邻的路口
                nextCross.push_back((map.road[nextRoad[i]-ROAD_INDEX][4] == _curCross)? map.road[nextRoad[i]-ROAD_INDEX][5]:map.road[nextRoad[i]-ROAD_INDEX][4]);
                trueNextRoad.push_back(nextRoad[i]);
            }
            else if(map.road[nextRoad[i]-ROAD_INDEX][6] == 0 && _curCross == map.road[nextRoad[i]-ROAD_INDEX][4])
            {
                nextCross.push_back(map.road[nextRoad[i]-ROAD_INDEX][5]);
                trueNextRoad.push_back(nextRoad[i]);
            }
        }
    }
    int score = INT32_MAX;
    for(size_t i = 0u; i<nextCross.size(); ++i)
    {
        int distance = map.map[nextCross[i] - CROSS_INDEX][_to - CROSS_INDEX] + map.map[_curCross - CROSS_INDEX][nextCross[i] - CROSS_INDEX]; 
        distance = getScore(distance, _curCross, trueNextRoad[i]);
        if(distance < score && nextCross[i] != _preCross)
        {//出现距离更小的路，且没有掉头返回上一个路口（车辆不允许掉头）
            score = distance;
            resCross = nextCross[i];
            resRoad = trueNextRoad[i];
        }
    }
    /*更新目标road*/
    _nextRoad = resRoad;
    assert(resRoad != -1);//没有找到下个路
    assert(resCross != -1);//没有找到下个路口

    /*更新turnto*/
    _turnto = whereToTurn(_atRoad, _nextRoad, nextRoad);
    return resCross;
}



// void Car::updataCross(int nextCross)
// {
//     _preCross = _curCross;
//     _curCross = nextCross;
//     _answerPath.push_back(_atRoad);
//     if(_curCross == _to)
//         setStatusEnd();
// }

// void Car::updataRoad(int nextCross)
// { 
//     _preCross = _curCross;
//     _curCross = nextCross;
//     _answerPath.push_back(_atRoad);
//     if(_curCross == _to)
//         setStatusEnd();
// }



/*static function*/
void Car::Scheduler(Map &map)
{
    static int j = 6;
    for (int i=0; i<Car::numALL; ++i )//把启动车辆加入入口
    {
        Car* p = cars[i];
        //if(p->_startTime == turntime && p->getStatus() == isStop  && Car::numRuning < 100)//调度车辆启动 && Car::numRuning <100
        if(turntime >= p->_planeTime && p->getStatus() == isStop && p->_maxSpeed >j && Car::numRuning <500)
        {
            p->setStatusRuning();
            p->setStartTime(turntime);
            Cross::crosses[p->_from - CROSS_INDEX]->processStartCar(map, p);
            //Car::cars[i]->searchPath(map);
        }
        // if(p->_startTime <= turntime && p->getStatus() == isStop)
        //     p->setStartTime(turntime + 1);
    }
    if(Car::numRuning < 18)j-=2;
    for(size_t i=0u; i<Road::roads.size(); ++i)//调度路上的车进入cross
    {
        Road::roads[i]->processCarInRoad(map);//进入入口后判断是否到达终点
    }
    for (size_t i=0u; i<Cross::crosses.size(); ++i )//调度所有路口
    {
        Cross::crosses[i]->outputCarToRoad();//车辆从路口出来
    }

    for(size_t i=0u; i<Road::roads.size(); ++i)//更新路况
    {
        Road::roads[i]->updateRoadCondition();
    }
}
//100 10

/*读入car.txt文件 static function*/
void Car::readCars(string file)
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
		//car * tmp = new car((uint32_t)res[0],(int8_t)res[1],(int8_t)res[2],(int8_t)res[3],(uint32_t)res[4]);
		cars.push_back(new Car(res));
    }
    infile.close();             //关闭文件输入流 
}

/*static function*/
void Car::initCars(string file)
{
	Car::readCars(file);
}

