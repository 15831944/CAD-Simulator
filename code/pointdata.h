#ifndef POINTDATA_H
#define POINTDATA_H

#include <vector>
#include <string>

using namespace std;

class DXF3Dface{
    public:
        double x0,y0,z0;//第一角點x,y,z值
        double x1,y1,z1;//第二角點x,y,z值
        double x2,y2,z2;//第三角點x,y,z值
        double x3,y3,z3;//第四角點x,y,z值 
};

class PointNormal{
    public:
        double x0,y0,z0;
        double x1,y1,z1;
        double x2,y2,z2;
        double x3,y3,z3;
        double Nx,Ny,Nz;
        bool visible0,visible1,visible2,visible3;
};

class PointData{
    public:
        PointData(){
            sx = 1;
            sy = 1;
            sz = 1;
            ipx = 0;
            ipy = 0;
            ipz = 0;

        }
        ~PointData(){}
        string name;

        double sx;//缩放比例
        double sy;
        double sz;

        double ipx;//物體座標系在世界座標系中的位置
        double ipy;
        double ipz;
        vector<DXF3Dface> _3Dface;
};

#endif // POINTDATA_H
