#ifndef TEST_CREATIONCLASS_H
#define TEST_CREATIONCLASS_H


#include "dl_creationadapter.h"
#include "pointdata.h"


class Test_CreationClass : public DL_CreationAdapter {
public:
    static int count;
    Test_CreationClass();
    vector<PointData> nObject;
    double Minx,Miny,Minz;
    double Maxx,Maxy,Maxz;
    //virtual void addLine(const DL_LineData& data);
    virtual void add3dFace(const DL_3dFaceData& data);

    //virtual void addAttribute(const DL_AttributeData& data);
    virtual void setVariableVector(const std::string& key,  double v1, double v2, double v3);//, int code
    //virtual void addPolyline(const DL_PolylineData& data);
    /*
    virtual void addLayer(const DL_LayerData& data);
    virtual void addPoint(const DL_PointData& data);
    virtual void addCircle(const DL_CircleData& data);
    virtual void addArc(const DL_ArcData& data);
    virtual void addCircle(const DL_CircleData& data);
    virtual void addPolyline(const DL_PolylineData& data);
    virtual void addVertex(const DL_VertexData& data);
    virtual void add3dFace(const DL_3dFaceData& data);
    //插入对象
    virtual void addInsert(const DL_InsertData& data);
    //插入块
    virtual void addBlock(const DL_BlockData& data);
    virtual void endBlock();

    void printAttributes();
    */

};

#endif // TEST_CREATIONCLASS_H
