#include <iostream>
#include <string>
#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <cassert> 
#include <vector>
#include <map>

#include "mymap.h"
#include "cross.h"
#include "road.h"
#include "car.h"

using namespace std;
uint32_t turntime = 1;

void writeAnswer(string file);

int main(int argc, char *argv[])
{
    std::cout << "Begin" << std::endl;
	
	if(argc < 6){
		std::cout << "please input args: carPath, roadPath, crossPath, answerPath" << std::endl;
		exit(1);
	}

	
	std::string carPath(argv[1]);
	std::string roadPath(argv[2]);
	std::string crossPath(argv[3]);
	std::string presetAnswerPath(argv[4]);
	std::string answerPath(argv[5]);


	Map cityMap(roadPath,crossPath,presetAnswerPath);
	Road::initRoads(cityMap);
	Cross::initCrosses(cityMap);
	Map::initMap(cityMap, Cross::crosses);
	Car::initCars(carPath, cityMap);	
	Car::numALL = Car::cars.size() + Car::pricars.size();
	Car::numStop = Car::numALL;//所有未启动的车
 

	//mySDL::init();//可视化初始化

	do
	{
		Car::Scheduler(cityMap);
		/*路上的车辆加入cross*/
		/*路口处理车辆*/
		/*路口的车辆出来到路上*/
		/*启动车辆加到路口*/
		cout<<"turntime:"<<turntime<<"  numRuning:"<<Car::numRuning
		<<"  "<<"numStop:"<<Car::numStop<<"  numEnd:"<<Car::numEnd<<endl;
		++turntime;
		//mySDL::display();//可视化
	}while(Car::numRuning != 0 || Car::numStop !=0);
	uint64_t num=0;
    for (size_t i=0u; i<Car::cars.size(); ++i )
    {
		Car* p = Car::cars[i];
		num+=p->_numOfSchedule;
	}
    for (size_t i=0u; i<Car::pricars.size(); ++i )
    {
		Car* p = Car::pricars[i];
		num+=p->_numOfSchedule;
	}
	cout<<"allcarsturntime: "<<num<<endl;

	writeAnswer(answerPath);	
	return 0;
}

/*写入answer.txt文件*/
void writeAnswer(string file)
{
	ofstream outfile; 
    outfile.open(file.data());   //将文件流对象与文件连接起来 
    assert(outfile.is_open());   //若失败,则输出错误消息,并终止程序运行
	outfile << "#(carId,StartTime,RoadId...)";
	outfile << "\n";
    for (size_t i=0u; i<Car::cars.size(); ++i )
    {
        Car* p = Car::cars[i];
		if(p->_preset == 1)continue;
		outfile << "("<<p->_id<< ", " <<p->_startTime;
		int n = p->_answerPath.size();
		for(int i=0; i<n; ++i)
		{
			outfile << ", " << p->_answerPath[i];
		}
		outfile << ")";
		outfile << "\n";
    }
    for (size_t i=0u; i<Car::pricars.size(); ++i )
    {
        Car* p = Car::pricars[i];
		if(p->_preset == 1)continue;
		outfile << "("<<p->_id<< ", " <<p->_startTime;
		int n = p->_answerPath.size();
		for(int i=0; i<n; ++i)
		{
			outfile << ", " << p->_answerPath[i];
		}
		outfile << ")";
		outfile << "\n";
    }
	outfile.close();
}



