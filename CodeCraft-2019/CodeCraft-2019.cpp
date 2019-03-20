#include <iostream>
#include <string>
#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <cassert> 
#include <vector>

#include "car.h"
#include "map.h"
#include "cross.h"
#include "road.h"

using namespace std;
//extern int Scheduler(vector<Car *>& cars);

uint32_t turntime = 1;


// void initCar(string file);
// void initCross(Map &map);
void writeAnswer(string file, vector<Car *>& cars);


int main(int argc, char *argv[])
{
    std::cout << "Begin" << std::endl;
	
	if(argc < 5){
		std::cout << "please input args: carPath, roadPath, crossPath, answerPath" << std::endl;
		exit(1);
	}
	
	std::string carPath(argv[1]);
	std::string roadPath(argv[2]);
	std::string crossPath(argv[3]);
	std::string answerPath(argv[4]);
	// std::cout << "carPath is " << carPath << std::endl;
	// std::cout << "roadPath is " << roadPath << std::endl;
	// std::cout << "crossPath is " << crossPath << std::endl;
	// std::cout << "answerPath is " << answerPath << std::endl;

	Car::initCars(carPath);	
	Car::numALL = Car::cars.size();
	Car::numStop = Car::numALL;//所有未启动的车
	Map map(roadPath,crossPath);
	Cross::initCrosses(map);
	Road::initRoads(map);

	do
	{
		Car::Scheduler(map);//启动车辆加到路口
		/*路上的车辆加入cross*/
		/*路口处理车辆*/
		/*路口的车辆出来到路上*/	

		++turntime;
		// cout<<"turntime:"<<turntime<<endl;
		// cout<<"numRuning:"<<Car::numRuning<<endl;
	}while(Car::numRuning != 0 || Car::numStop !=0);

	writeAnswer(answerPath, Car::cars);



	/*把计划开始的*/
	// TODO:read input filebuf
	// TODO:process
	// TODO:write output file
	
	return 0;
}

/*读入road.txt文件和cross.txt文件*/
void writeAnswer(string file, vector<Car *>& cars)
{
	ofstream outfile; 
    outfile.open(file.data());   //将文件流对象与文件连接起来 
    assert(outfile.is_open());   //若失败,则输出错误消息,并终止程序运行
	outfile << "#(carId,StartTime,RoadId...)";
	outfile << "\n";
    for (int i=0; i<Car::numALL; ++i )
    {
        Car* p = cars[i];
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



