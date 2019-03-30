#include "mymap.h"
#include "cross.h"
#include "road.h"
#include "car.h"
#include "param.h"
#include <algorithm>

bool comp(vector<int> i, vector<int> j){return i[0]<j[0];}

Map::Map(string roadfile, string crossfile)
{
    readroad(roadfile);
    readcross(crossfile);
    sort(cross.begin(),cross.end(),comp);
    sort(road.begin(),road.end(),comp);
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

void Map::initMap(Map & cityMap, map<int, Cross*>& crosses)
{
    int i,j,k;
    int n = cityMap.cross.size();
    int m = cityMap.road.size();
    /*初始化map[n][n]的大小*/
    cityMap.mymap.resize(n);
    for(i = 0; i<n; ++i)
        cityMap.mymap[i].resize(n);

    /*初始化map[n][n]的内容全为max*/
    for(i=0; i<n; ++i)
    {
        for(j=0;j<n;j++)
        {
            if(i != j)
                cityMap.mymap[i][j] = INT16_MAX;
            else
                cityMap.mymap[i][j] = 0;
        }
    }

    /*初始化map[n][n]的内容*/    
    for(i=0; i<m; ++i)
    {
        if(1 == cityMap.road[i][6])/*双向通行*/
        {
            cityMap.mymap[crosses[cityMap.road[i][4]]->_index][crosses[cityMap.road[i][5]]->_index] = cityMap.road[i][1];
            cityMap.mymap[crosses[cityMap.road[i][5]]->_index][crosses[cityMap.road[i][4]]->_index] = cityMap.road[i][1];
        }
        else/*单向通行*/
        {
            cityMap.mymap[crosses[cityMap.road[i][4]]->_index][crosses[cityMap.road[i][5]]->_index] = cityMap.road[i][1];
        }
    }
    
    /*Floyd-Warshall算法核心语句*/
    for(k=0;k<n;k++)
        for(i=0;i<n;i++)  
            for(j=0;j<n;j++)  
                if(cityMap.mymap[i][j]>cityMap.mymap[i][k]+cityMap.mymap[k][j] )   
                    cityMap.mymap[i][j]=cityMap.mymap[i][k]+cityMap.mymap[k][j]; 
}

/*更新邻接矩阵*/
void Map::updateMatrix()
{
    int i,j,k,n;
    n = mymap.size();
    for(k=0;k<n;k++)
        for(i=0;i<n;i++)  
            for(j=0;j<n;j++)  
                if(mymap[i][j]>mymap[i][k]+mymap[k][j])
                    mymap[i][j]=mymap[i][k]+mymap[k][j]; 
}

bool isCrowd(Road* proad, vector<vector<Car*>>& v)
{
    for(size_t i=0u; i<v.size(); ++i)
    {
        if(!v[i].empty() && v[i].back()->_distanceToCross == proad->_length - 1)
            continue;
        else
            return false;
    }
    return true;
}

void Map::updataOneRoad(Road* proad)
{
    int jams = Cross::crosses[proad->_from]->_jams;
    vector<vector<Car*>> v = proad->carsInRoadFromTo;

    if( jams >= 2 && isCrowd(proad,v))
    {
        mymap[Cross::crosses[proad->_from]->_index] [Cross::crosses[proad->_to]->_index] = jams*100 + proad->_length;
    }
    else
    {
        mymap[Cross::crosses[proad->_from]->_index] [Cross::crosses[proad->_to]->_index] = proad->_length;
    }
    
    if(proad->_isDuplex == 0)
        return;
    jams = Cross::crosses[proad->_to]->_jams;
    v = proad->carsInRoadToFrom;  
    if( jams >= 2 && isCrowd(proad,v))
    {
        mymap[Cross::crosses[proad->_to]->_index] [Cross::crosses[proad->_from]->_index] = jams*100 + proad->_length;
    }
    else
    {
        mymap[Cross::crosses[proad->_to]->_index] [Cross::crosses[proad->_from]->_index] = proad->_length;
    }
}

/*输入两个路口的id，查询其最短距离*/
int Map::getDistance(int cross1_id, int cross2_id)
{
    return mymap[Cross::crosses[cross1_id]->_index][Cross::crosses[cross2_id]->_index];
}
