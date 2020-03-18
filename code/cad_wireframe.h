#ifndef CAD_WIREFRAME_H
#define CAD_WIREFRAME_H

#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include "test_creationclass.h"
#include "dl_dxf.h"
#include <QDebug>
#include <QVector>
#include <QVector3D>

class GLWidget;
class GLWidget_Multi;

class CAD_wireframe: protected QOpenGLFunctions
{
    struct Vertex
        {
            void set( const QVector3D& _position)
            {
                position = _position;
            }

            QVector3D position;
        };
  public:
    enum ShadowType
        {
            NoShadow = 0,
            SimpleShadow,
            PCFShadow
        };
    explicit CAD_wireframe( QMatrix4x4& modelMatrix,
                            QMatrix4x4& viewMatrix,
                            const QString& fileName,
                            bool ismask);
    ~CAD_wireframe( void );
    void render( GLWidget* widget );
    void renderShadow( GLWidget* widget );
    void setVertexAttribute( int attributeLocation,
                             GLenum elementType,
                             quint32 elementSize,
                             quint32 offset );
    GLfloat Xmax=0,Xmin=0;
    GLfloat Ymax=0,Ymin=0;
    GLfloat Zmax=0,Zmin=0;

    QMatrix4x4&             m_modelMatrix;
    QMatrix4x4&             m_viewMatrix;
    ShadowType              m_shadowType;
    QOpenGLBuffer           m_vertexBuffer;
    QOpenGLBuffer           w_vertexBuffer;
    Vertex*                 m_vertices;
    Vertex*                 w_vertices;

    Test_CreationClass f ;
    DL_Dxf dxf ;
    vector<QVector3D> Wire_vector_position;
    vector<QVector3D> vector_position;
    vector<QVector3D> vector_normal;

    vector <PointNormal> Normal;
    void IniNormal();
    int  checkData(PointNormal point,double x,double y,double z);
    double CalNormalAngle(double a1,double a2,double a3,double b1,double b2,double b3);
    bool checkNormal(PointNormal point,PointNormal _point);
    void SolveInvisible(PointNormal& point);
    void CreateModel();
    bool mask;
    static QOpenGLShaderProgram* s_program;
    static int s_positionLoc, s_normalLoc,
               s_texCoordLoc, s_modelMatrixLoc,
               s_viewMatrixLoc, s_projectionMatrixLoc,
               s_lightViewMatrixLoc, s_lightProjectionMatrixLoc,
               s_lightPositionLoc, s_modelViewNormalMatrixLoc, s_shadowTypeLoc,IsWireframe,s_color;
    static int s_count;
};

#endif
