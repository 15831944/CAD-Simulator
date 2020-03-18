#include "wireframe.h"
#include <QOpenGLShaderProgram>
#include <QVector2D>
#include <QVector3D>


Wireframe::Wireframe()
    : indexBuf(QOpenGLBuffer::IndexBuffer),m_count(0)
{
    initializeOpenGLFunctions();
    m_data.resize(2500 * 6);
    // Generate 2 VBOs
    //arrayBuf.create();
    //indexBuf.create();
}

Wireframe::~Wireframe()
{
    //arrayBuf.destroy();
    //indexBuf.destroy();
}

void Wireframe::loadModel()
{
    qDebug() << "Start Loading Model......";
    qDebug() << "Found " << f.nObject.at(0)._3Dface.size() << " 3dFace" ;


    for(int i = 0 ; i<(int)f.nObject.at(0)._3Dface.size() ; i++ ){
        GLfloat V1x,V1y,V1z,V2x,V2y,V2z;
        //GLfloat Nx = 0 ;
        //GLfloat Ny = 0 ;
        //GLfloat Nz = 0 ;

        V1x =(GLfloat)f.nObject.at(0)._3Dface[i].x1 - (GLfloat)f.nObject.at(0)._3Dface[i].x0;
        V1y =(GLfloat)f.nObject.at(0)._3Dface[i].y1 - (GLfloat)f.nObject.at(0)._3Dface[i].y0;
        V1z =(GLfloat)f.nObject.at(0)._3Dface[i].z1 - (GLfloat)f.nObject.at(0)._3Dface[i].z0;

        V2x =(GLfloat)f.nObject.at(0)._3Dface[i].x3 - (GLfloat)f.nObject.at(0)._3Dface[i].x0;
        V2y =(GLfloat)f.nObject.at(0)._3Dface[i].y3 - (GLfloat)f.nObject.at(0)._3Dface[i].y0;
        V2z =(GLfloat)f.nObject.at(0)._3Dface[i].z3 - (GLfloat)f.nObject.at(0)._3Dface[i].z0;

        QVector3D n = QVector3D::normal(QVector3D(V1x, V1y, V1z), QVector3D(V2x, V2y,V2z));
        //第一種狀況 : 3dface 為三角形 -> 最後一點重複
        if(f.nObject.at(0)._3Dface[i].x2 == f.nObject.at(0)._3Dface[i].x3 &&
           f.nObject.at(0)._3Dface[i].y2 == f.nObject.at(0)._3Dface[i].y3 &&
           f.nObject.at(0)._3Dface[i].z2 == f.nObject.at(0)._3Dface[i].z3)
          {
           add(QVector3D((GLfloat)f.nObject.at(0)._3Dface[i].x0,(GLfloat)f.nObject.at(0)._3Dface[i].y0,(GLfloat)f.nObject.at(0)._3Dface[i].z0),n);
           add(QVector3D((GLfloat)f.nObject.at(0)._3Dface[i].x1,(GLfloat)f.nObject.at(0)._3Dface[i].y1,(GLfloat)f.nObject.at(0)._3Dface[i].z1),n);
           add(QVector3D((GLfloat)f.nObject.at(0)._3Dface[i].x2,(GLfloat)f.nObject.at(0)._3Dface[i].y2,(GLfloat)f.nObject.at(0)._3Dface[i].z2),n);
          }
        //第二種狀況 : 3dface 為四邊形 -> 分割為兩三角形
        else
          {
           add(QVector3D((GLfloat)f.nObject.at(0)._3Dface[i].x0,(GLfloat)f.nObject.at(0)._3Dface[i].y0,(GLfloat)f.nObject.at(0)._3Dface[i].z0),n);
           add(QVector3D((GLfloat)f.nObject.at(0)._3Dface[i].x1,(GLfloat)f.nObject.at(0)._3Dface[i].y1,(GLfloat)f.nObject.at(0)._3Dface[i].z1),n);
           add(QVector3D((GLfloat)f.nObject.at(0)._3Dface[i].x2,(GLfloat)f.nObject.at(0)._3Dface[i].y2,(GLfloat)f.nObject.at(0)._3Dface[i].z2),n);

           add(QVector3D((GLfloat)f.nObject.at(0)._3Dface[i].x2,(GLfloat)f.nObject.at(0)._3Dface[i].y2,(GLfloat)f.nObject.at(0)._3Dface[i].z2),n);
           add(QVector3D((GLfloat)f.nObject.at(0)._3Dface[i].x3,(GLfloat)f.nObject.at(0)._3Dface[i].y3,(GLfloat)f.nObject.at(0)._3Dface[i].z3),n);
           add(QVector3D((GLfloat)f.nObject.at(0)._3Dface[i].x0,(GLfloat)f.nObject.at(0)._3Dface[i].y0,(GLfloat)f.nObject.at(0)._3Dface[i].z0),n);
          }

    }
}



void Wireframe::add(const QVector3D &v, const QVector3D &n)
{
    GLfloat *p = m_data.data() + m_count;
    *p++ = v.x();
    *p++ = v.y();
    *p++ = v.z();
    *p++ = n.x();
    *p++ = n.y();
    *p++ = n.z();

    m_count += 6;
}

void Wireframe::quad(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2, GLfloat x3, GLfloat y3, GLfloat x4, GLfloat y4)
{
    QVector3D n = QVector3D::normal(QVector3D(x4 - x1, y4 - y1, 0.0f), QVector3D(x2 - x1, y2 - y1, 0.0f));

    add(QVector3D(x1, y1, -0.05f), n);
    add(QVector3D(x4, y4, -0.05f), n);
    add(QVector3D(x2, y2, -0.05f), n);

    add(QVector3D(x3, y3, -0.05f), n);
    add(QVector3D(x2, y2, -0.05f), n);
    add(QVector3D(x4, y4, -0.05f), n);

    n = QVector3D::normal(QVector3D(x1 - x4, y1 - y4, 0.0f), QVector3D(x2 - x4, y2 - y4, 0.0f));

    add(QVector3D(x4, y4, 0.05f), n);
    add(QVector3D(x1, y1, 0.05f), n);
    add(QVector3D(x2, y2, 0.05f), n);

    add(QVector3D(x2, y2, 0.05f), n);
    add(QVector3D(x3, y3, 0.05f), n);
    add(QVector3D(x4, y4, 0.05f), n);
}

void Wireframe::extrude(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2)
{
    QVector3D n = QVector3D::normal(QVector3D(0.0f, 0.0f, -0.1f), QVector3D(x2 - x1, y2 - y1, 0.0f));

    add(QVector3D(x1, y1, +0.05f), n);
    add(QVector3D(x1, y1, -0.05f), n);
    add(QVector3D(x2, y2, +0.05f), n);

    add(QVector3D(x2, y2, -0.05f), n);
    add(QVector3D(x2, y2, +0.05f), n);
    add(QVector3D(x1, y1, -0.05f), n);
}

