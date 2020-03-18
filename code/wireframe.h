#ifndef WIREFRAME_H
#define WIREFRAME_H

#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <qopengl.h>
#include <QVector>
#include <QVector3D>
#include "test_creationclass.h"
#include "dl_dxf.h"

class Wireframe : protected QOpenGLFunctions
{
public:
    Wireframe();
    virtual ~Wireframe();

    //void Draw_CAD_Model(QOpenGLShaderProgram *program,QOpenGLVertexArrayObject *VAO);
    const GLfloat *constData() const { return m_data.constData(); }
    const GLfloat *Data() const{ return m_data.data(); }
    int count() const { return m_count; }
    int vertexCount() const { return m_count / 6; }
    void loadModel();
    QOpenGLBuffer arrayBuf;
    QOpenGLBuffer indexBuf;
    Test_CreationClass f ;
    DL_Dxf dxf ;

private:
    void quad(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2, GLfloat x3, GLfloat y3, GLfloat x4, GLfloat y4);
    void extrude(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2);
    void add(const QVector3D &v, const QVector3D &n);

    QVector<GLfloat> m_data;
    QVector<QVector3D>PointPosition;
    QVector<QVector3D>PointNormal;
    int m_count;


};


#endif // WIREFRAME_H
