#ifndef CAD_MODEL_H
#define CAD_MODEL_H

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

class CAD_model: protected QOpenGLFunctions
{
    struct Vertex
        {
            void set( const QVector3D& _position, const QVector3D& _normal,
                      const QVector2D& _texCoord )
            {
                position = _position;
                normal = _normal;
                texCoord = _texCoord;
            }

            QVector3D position;
            QVector3D normal;
            QVector2D texCoord;
        };
  public:
    enum ShadowType
        {
            NoShadow = 0,
            SimpleShadow,
            PCFShadow
        };
    explicit CAD_model( QMatrix4x4& modelMatrix,
                        QMatrix4x4& viewMatrix,
                        QVector3D&  lightposition,
                        QMatrix4x4& lightViewMatrix,
                        const QString& fileName,
                        const QString& texfileName,
                        ShadowType shadowType );
    ~CAD_model( void );
    void render( GLWidget* widget );
    void renderShadow( GLWidget* widget );
    void setVertexAttribute( int attributeLocation,
                             GLenum elementType,
                             quint32 elementSize,
                             quint32 offset );
    inline Vertex& CallVextexData(void){return *m_vertices;}
    GLfloat Xmax,Xmin;
    GLfloat Ymax,Ymin;
    GLfloat Zmax,Zmin;

    QMatrix4x4&             m_modelMatrix;
    QMatrix4x4&             m_viewMatrix;
    QVector3D&              m_lightposition;
    QMatrix4x4&             m_lightViewMatrix;
    ShadowType              m_shadowType;
    QOpenGLBuffer           m_vertexBuffer;
    QOpenGLTexture          m_texture;
    Vertex*                 m_vertices;
    Test_CreationClass f ;
    DL_Dxf dxf ;
    vector<QVector3D> vector_position;
    vector<QVector3D> vector_normal;
    vector<QVector2D> vector_texCoord;

    void CreateModel();

    static QOpenGLShaderProgram* s_program;
    static int s_positionLoc, s_normalLoc,
               s_texCoordLoc, s_modelMatrixLoc,
               s_viewMatrixLoc, s_projectionMatrixLoc,
               s_lightViewMatrixLoc, s_lightProjectionMatrixLoc,
               s_lightPositionLoc, s_modelViewNormalMatrixLoc, s_shadowTypeLoc;
    static int s_count;
    float ambient;
    int s_ambient;
};

#endif // CAD_MODEL_H
