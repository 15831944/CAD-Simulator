#ifndef BACKGROUND_H
#define BACKGROUND_H

#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <qopengl.h>
#include <QVector>
#include <QVector3D>
#include "test_creationclass.h"
#include "dl_dxf.h"

class Background : protected QOpenGLFunctions
{
public:
    Background();
    const GLfloat *constData() const { return m_data.constData(); }
    const GLfloat *Data() const{ return m_data.data(); }
    int count() const { return m_count; }
    int vertexCount() const { return m_count / 8; }

private:
    void add(const QVector3D &v, const QVector3D &n,const QVector2D &c);

    QVector<GLfloat> m_data;
    int m_count;


};


#endif // BACKGROUND_H
