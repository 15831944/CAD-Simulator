#ifndef MODEL_H
#define MODEL_H

#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <qopengl.h>
#include <QVector>
#include <QVector3D>
#include "test_creationclass.h"
#include "dl_dxf.h"

class CAD_Model : protected QOpenGLFunctions
{
public:
    CAD_Model();

    const GLfloat *constData() const { return m_data.constData(); }
    const GLfloat *Data() const{ return m_data.data(); }

    const GLfloat *WconstData() const { return Wireframe_data.constData(); }
    const GLfloat *WData() const{ return Wireframe_data.data(); }

    int m_count;
    int wire_count;
    bool isCulling;
    bool isHiding;
    bool iswireframe;
    int count() const { return m_count; }
    int Wcount() const { return wire_count; }
    int vertexCount() const { return m_count / 6; }
    int WvertexCount() const { return wire_count / 3; }


    void loadModel();
    bool CheckSameLine(double Px1,double Py1,double Pz1,double Px2,double Py2,double Pz2, PointNormal Triangle);
    void CreateWireframe();
    Test_CreationClass f ;
    DL_Dxf dxf ;


private:

    void quad(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2, GLfloat x3, GLfloat y3, GLfloat x4, GLfloat y4);
    void extrude(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2);
    void add(const QVector3D &v, const QVector3D &n);
    void addWire(const QVector3D &v);
    void NormalNormalize(GLfloat& Nx,GLfloat& Ny,GLfloat& Nz);
    int checkData(PointNormal point,double x,double y,double z);
    double CalNormalAngle(double a1,double a2,double a3,double b1,double b2,double b3);
    void CalNormal(GLfloat& Nx,GLfloat& Ny,GLfloat& Nz,GLfloat V1x,GLfloat V1y,GLfloat V1z,GLfloat V2x,GLfloat V2y,GLfloat V2z);
    bool checkNormal(PointNormal point,PointNormal _point);
    void SolveInvisible(PointNormal& point);
    void IniNormal();


    QVector<GLfloat> m_data;
    QVector<GLfloat> Wireframe_data;
    vector <PointNormal> Normal;

};


#endif // MODEL_H
