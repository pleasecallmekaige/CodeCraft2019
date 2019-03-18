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

using namespace std;
//extern int Scheduler(vector<Car *>& cars);

vector<Car *> cars;
vector<Cross *> crosses;
uint32_t turntime = 1;


void initCar(string file);
void initCross(Map &map);
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

	initCar(carPath);	
	Car::numALL = cars.size();
	Car::numStop = Car::numALL;//所有未启动的车
	Map map(roadPath,crossPath);

	do
	{
		int nextRoad;
		Car::Scheduler(cars);
		for(int i=0; i<Car::numALL; ++i)
		{
			if(cars[i]->getStatus() == isRuning)
			{
				nextRoad = cars[i]->searchPath(map);
				cars[i]->updataCurCross(nextRoad);
			}
		}
		++turntime;
		// cout<<"turntime:"<<turntime<<endl;
		// cout<<"numRuning:"<<Car::numRuning<<endl;
	}while(Car::numRuning != 0 || Car::numStop !=0);

	writeAnswer(answerPath, cars);



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
		outfile << "("<<p->id<< ", " <<p->startTime;
		int n = p->answerPath.size();
		for(int i=0; i<n; ++i)
		{
			outfile << ", " << p->answerPath[i];
		}
		outfile << ")";
		outfile << "\n";
    }
	outfile.close();
}

/*读入car.txt文件*/
void readCar(string file)
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

void initCar(string file)
{
	readCar(file);
}

