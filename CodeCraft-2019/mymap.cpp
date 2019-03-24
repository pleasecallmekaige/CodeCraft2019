#include "mymap.h"
#include "cross.h"

Map::Map(string roadfile, string crossfile)
{
    readroad(roadfile);
    readcross(crossfile);
    initMap();
}

void Map::readroad(string file)
{
    ifstream infile; 
    infile.open(file.data());   //将文件流对象与文件连接起来 
    assert(infile.is_open());   //若失败,则输出错误消息,并终止程序运行
	string s;
    string t;
    int result;
	getline(infile,s);
    while(getline(infile,s))
    {
		stringstream input(s.substr(1,s.length()-2));
        vector<int> res;
		while(input>>result){
			input>>t;
			res.push_back(result);
		}
		//car * tmp = new car((uint32_t)res[0],(int8_t)res[1],(int8_t)res[2],(int8_t)res[3],(uint32_t)res[4]);
		road.push_back(res);
    }
    infile.close();             //关闭文件输入流
}

void Map::readcross(string file)
{
    ifstream infile; 
    infile.open(file.data());   //将文件流对象与文件连接起来 
    assert(infile.is_open());   //若失败,则输出错误消息,并终止程序运行
	string s;
    string t;
    int result;
	getline(infile,s);
    while(getline(infile,s))
    {
		stringstream input(s.substr(1,s.length()-2));
        vector<int> res;
		while(input>>result){
			input>>t;
			res.push_back(result);
		}
		//car * tmp = new car((uint32_t)res[0],(int8_t)res[1],(int8_t)res[2],(int8_t)res[3],(uint32_t)res[4]);
		cross.push_back(res);
    }
    infile.close();             //关闭文件输入流
}

void Map::initMap()
{
    int i,j,k;
    int n = cross.size();
    int m = road.size();
    /*初始化map[n][n]的大小*/
    map.resize(n);
    for(i = 0; i<n; ++i)
        map[i].resize(n);

    /*初始化map[n][n]的内容全为max*/
    for(i=0; i<n; ++i)
    {
        for(j=0;j<n;j++)
        {
            if(i != j)
                map[i][j] = INT16_MAX;
            else
                map[i][j] = 0;
        }
    }

    /*初始化map[n][n]的内容*/    
    for(i=0; i<m; ++i)
    {
        if(1 == road[i][6])/*双向通行*/
        {
            map[road[i][4] - CROSS_INDEX][road[i][5] - CROSS_INDEX] = road[i][1];
            map[road[i][5] - CROSS_INDEX][road[i][4] - CROSS_INDEX] = road[i][1];
        }
        else/*单向通行*/
        {
            map[road[i][4] - CROSS_INDEX][road[i][5] - CROSS_INDEX] = road[i][1];
        }
    }
        
    /*Floyd-Warshall算法核心语句*/
    for(k=0;k<n;k++)  
        for(i=0;i<n;i++)  
            for(j=0;j<n;j++)  
                if(map[i][j]>map[i][k]+map[k][j] )   
                    map[i][j]=map[i][k]+map[k][j]; 
}

