#include <iostream>
#include <string>
#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <cassert> 
#include <vector>

#include <car.h>

using namespace std;

vector<car *> cars;
void readCar(string file);
void readroad(string file);


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
	std::cout << "carPath is " << carPath << std::endl;
	std::cout << "roadPath is " << roadPath << std::endl;
	std::cout << "crossPath is " << crossPath << std::endl;
	std::cout << "answerPath is " << answerPath << std::endl;

	readCar(carPath);	
	car::numStop = cars.size();//所有未启动的车
	// while(1)
	// {
		
	// }	

	/*把计划开始的*/
	// TODO:read input filebuf
	// TODO:process
	// TODO:write output file
	
	return 0;
}

/*读入car.txt文件*/
void readCar(string file)
{
    ifstream infile; 
    infile.open(file.data());   //将文件流对象与文件连接起来 
    assert(infile.is_open());   //若失败,则输出错误消息,并终止程序运行 
	int result;
	vector<int, std::allocator<int>> res;
    string s;
	string t;
	getline(infile,s);  //#
    while(getline(infile,s))
    {
		stringstream input(s.substr(1,s.length()-2));
		while(input>>result){
			input>>t;
			res.push_back(result);
		}
		//car * tmp = new car((uint32_t)res[0],(int8_t)res[1],(int8_t)res[2],(int8_t)res[3],(uint32_t)res[4]);
		cars.push_back(new car(res));
    }
    infile.close();             //关闭文件输入流 
}

