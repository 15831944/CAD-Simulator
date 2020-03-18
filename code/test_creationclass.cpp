#include "test_creationclass.h"

#include <iostream>
#include <stdio.h>

Test_CreationClass::Test_CreationClass()
{   
    PointData object;
    nObject.push_back(object);
}


/*
void Test_CreationClass::addAttribute(const DL_AttributeData& data)
{

    std::cout << "Call addAttribute " <<std::endl;

}
*/

void Test_CreationClass::setVariableVector(const std::string& key,  double v1, double v2, double v3)//, int code
{
    if(key.compare("$EXTMIN")==0)
    {
      Minx = v1;
      Miny = v2;
      Minz = v3;
    }

    if(key.compare("$EXTMAX")==0)
    {
      Maxx = v1;
      Maxy = v2;
      Maxz = v3;
    }

}

/*
void Test_CreationClass::addLine(const DL_LineData& data)
{
     std::cout << "Line: "
               << data.x1 << "/" << data.y1 << data.z1 << " "
               << data.x2 << "/" << data.y2 << data.z2 << std::endl;
}
*/

void Test_CreationClass::add3dFace(const DL_3dFaceData& data)
{
     //std::cout <<"Call  add3dFace "<< std::endl;
     DXF3Dface n3Dface;

     n3Dface.x0 = data.x[0];n3Dface.y0 = data.y[0];n3Dface.z0 = data.z[0];
     n3Dface.x1 = data.x[1];n3Dface.y1 = data.y[1];n3Dface.z1 = data.z[1];
     n3Dface.x2 = data.x[2];n3Dface.y2 = data.y[2];n3Dface.z2 = data.z[2];
     n3Dface.x3 = data.x[3];n3Dface.y3 = data.y[3];n3Dface.z3 = data.z[3];

     nObject[nObject.size()-1]._3Dface.push_back(n3Dface);
}
/*
void addPolyline(const DL_PolylineData& data)
{

}*/
