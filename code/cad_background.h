#ifndef CAD_BACKGROUND_H
#define CAD_BACKGROUND_H

#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QDebug>
#include <QVector>
#include <QVector3D>

class GLWidget;
class GLWidget_Multi;

class CAD_background: protected QOpenGLFunctions
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
    explicit CAD_background( QMatrix4x4& modelMatrix,
                             QMatrix4x4& viewMatrix,
                             QVector3D&  lightposition,
                              QMatrix4x4& lightViewMatrix,
                             const QString& texfileName,
                             ShadowType shadowType );
    ~CAD_background( void );
    void render( GLWidget* widget );
    void renderShadow( GLWidget* widget );
    void setVertexAttribute( int attributeLocation,
                             GLenum elementType,
                             quint32 elementSize,
                             quint32 offset );

  protected:
    QMatrix4x4&             m_modelMatrix;
    QMatrix4x4&             m_viewMatrix;
    QVector3D&              m_lightposition;
    QMatrix4x4&             m_lightViewMatrix;
    ShadowType              m_shadowType;
    QOpenGLBuffer           m_vertexBuffer;
    QOpenGLTexture          m_texture;
    Vertex*                 m_vertices;

    static QOpenGLShaderProgram* s_program;
    static int s_positionLoc, s_normalLoc,
               s_texCoordLoc, s_modelMatrixLoc,
               s_viewMatrixLoc, s_projectionMatrixLoc,
               s_lightViewMatrixLoc, s_lightProjectionMatrixLoc,
               s_lightPositionLoc, s_modelViewNormalMatrixLoc, s_shadowTypeLoc;
    static int s_count;
    int s_ambient;
};

#endif // CAD_BACKGROUND_H
