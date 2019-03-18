#include<iostream>
#include <vector>
/*
(道路id，道路长度，最高限速，车道数目，起始点id，终点id，是否双向)
 */
#define ROAD_INDEX 5000

class road
{
private:
    std::vector<std::vector<uint32_t>> content;

    uint32_t id;
    int8_t length;
    int8_t end;
    int8_t limitSpeed; /*车的最高速度*/
    uint32_t channel;/*车道数目*/
    int from;
    int to;
    bool isDuplex;

    /*获取各个车道的车辆数目，以及能否进入*/
};