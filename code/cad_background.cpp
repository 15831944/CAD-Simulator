#include "cad_background.h"
#include <math.h>
#include "glwidget.h"
#include <QTime>


#define TEXTURE_UNIT GL_TEXTURE0
#define SHADOW_TEXTURE_UNIT GL_TEXTURE1

QOpenGLShaderProgram* CAD_background::s_program = Q_NULLPTR;
int CAD_background::s_positionLoc,
    CAD_background::s_normalLoc,
    CAD_background::s_texCoordLoc,
    CAD_background::s_modelMatrixLoc,
    CAD_background::s_viewMatrixLoc,
    CAD_background::s_projectionMatrixLoc,
    CAD_background::s_lightViewMatrixLoc,
    CAD_background::s_lightProjectionMatrixLoc,
    CAD_background::s_lightPositionLoc,
    CAD_background::s_modelViewNormalMatrixLoc,
    CAD_background::s_shadowTypeLoc,
    CAD_background::s_count = 0;

CAD_background::CAD_background( QMatrix4x4& modelMatrix,
                                QMatrix4x4& viewMatrix,
                                QVector3D&  lightposition,
                                QMatrix4x4& lightViewMatrix,
                                const QString& texfileName,
                                ShadowType shadowType ):
                                m_modelMatrix( modelMatrix ),
                                m_viewMatrix( viewMatrix ),
                                m_lightposition(lightposition),
                                m_lightViewMatrix(lightViewMatrix),
                                m_shadowType( shadowType ),
                                m_vertexBuffer( QOpenGLBuffer::VertexBuffer ),
                                m_texture( QOpenGLTexture::Target2D )
{
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
    initializeOpenGLFunctions( );
    if ( s_count++ == 0 )
    {
        s_program = new QOpenGLShaderProgram;
        s_program->addShaderFromSourceFile( QOpenGLShader::Vertex,":/Shader/Common.vert" );
        s_program->addShaderFromSourceFile( QOpenGLShader::Fragment,":/Shader/Common.frag" );
        s_program->link( );
        s_program->bind( );
        s_positionLoc = s_program->attributeLocation( "position" );
        s_normalLoc = s_program->attributeLocation( "normal" );
        s_texCoordLoc = s_program->attributeLocation( "texCoord" );
        s_modelMatrixLoc = s_program->uniformLocation( "modelMatrix" );
        s_viewMatrixLoc = s_program->uniformLocation( "viewMatrix" );
        s_projectionMatrixLoc = s_program->uniformLocation( "projectionMatrix" );
        s_lightPositionLoc = s_program->uniformLocation( "lightPosition" );
        s_lightViewMatrixLoc = s_program->uniformLocation( "lightViewMatrix" );
        s_lightProjectionMatrixLoc = s_program->uniformLocation( "lightProjectionMatrix" );
        s_modelViewNormalMatrixLoc = s_program->uniformLocation( "modelViewNormalMatrix" );
        s_shadowTypeLoc = s_program->uniformLocation( "shadowType" );
        int textureLoc = s_program->uniformLocation( "texture" );
        int shadowLoc = s_program->uniformLocation( "shadowTexture" );
        s_ambient = s_program->uniformLocation("ambient");


        s_program->setUniformValue( textureLoc,TEXTURE_UNIT - GL_TEXTURE0 );
        s_program->setUniformValue( shadowLoc,SHADOW_TEXTURE_UNIT - GL_TEXTURE0 );

        s_program->release( );
    }

    const QVector3D vertices[] =
        {
            QVector3D( 0.0, 0.0, 0.0 ),
            QVector3D( 640, 0.0, 0.0 ),
            QVector3D( 640, 480, 0.0 ),
            QVector3D( 0.0, 480, 0.0 )
        };
    const QVector2D texCoords[] =
        {
            QVector2D( 0.0, 0.0 ),
            QVector2D( 1.0, 0.0 ),
            QVector2D( 1.0, 1.0 ),
            QVector2D( 0.0, 1.0 )
        };
    const QVector3D normal  = QVector3D( 0.0, 0.0, 1.0 );

    m_vertices = new Vertex[6];
    Vertex* v = m_vertices;
    v[0].set( vertices[0], normal, texCoords[0] );
    v[1].set( vertices[1], normal, texCoords[1] );
    v[2].set( vertices[2], normal, texCoords[2] );
    v[3].set( vertices[0], normal, texCoords[0] );
    v[4].set( vertices[2], normal, texCoords[2] );
    v[5].set( vertices[3], normal, texCoords[3] );


    m_vertexBuffer.setUsagePattern( QOpenGLBuffer::DynamicDraw );
    m_vertexBuffer.create( );
    m_vertexBuffer.bind( );
    m_vertexBuffer.allocate( v, 6 * sizeof( Vertex ) );
    m_vertexBuffer.release( );

    m_texture.setMinificationFilter( QOpenGLTexture::LinearMipMapLinear );
    m_texture.setMagnificationFilter( QOpenGLTexture::Linear );
    QImage image( texfileName );
    Q_ASSERT( !texfileName.isNull( ) );
    m_texture.setData( image );

}

CAD_background::~CAD_background( void )
{
    m_vertexBuffer.destroy( );
    m_texture.destroy( );
    delete []m_vertices;
    if ( --s_count == 0 )
    {
        delete s_program;
    }
}

void CAD_background::render(GLWidget *widget)
{
    s_program->bind();
    m_vertexBuffer.bind();
    int offset = 0;
    setVertexAttribute( s_positionLoc, GL_FLOAT, 3, offset );
    offset += 3 * sizeof( GLfloat );
    setVertexAttribute( s_normalLoc, GL_FLOAT, 3, offset );
    offset += 3 * sizeof( GLfloat );
    setVertexAttribute( s_texCoordLoc, GL_FLOAT, 2, offset );

    float random_ambient =0;
    random_ambient = (qrand() % ((10 + 1) - 5) + 5);
    random_ambient/=10;
    random_ambient = 0.7;
    s_program->setUniformValue(s_ambient,random_ambient);
    s_program->setUniformValue( s_modelMatrixLoc, m_modelMatrix );
    s_program->setUniformValue( s_viewMatrixLoc, m_viewMatrix );
    s_program->setUniformValue( s_projectionMatrixLoc, widget->projectionMatrix( ) );
    s_program->setUniformValue( s_modelViewNormalMatrixLoc,( m_viewMatrix * m_modelMatrix ).normalMatrix( ) );

    m_texture.bind( );
    if ( m_shadowType != NoShadow )
       {
        s_program->setUniformValue( s_lightPositionLoc, m_lightposition );
        s_program->setUniformValue( s_lightViewMatrixLoc, m_lightViewMatrix );
        s_program->setUniformValue( s_lightProjectionMatrixLoc, widget->lightProjectionMatrix( ) );

        glActiveTexture( SHADOW_TEXTURE_UNIT );
        glBindTexture( GL_TEXTURE_2D, widget->shadowTexture( ) );
        glDrawArrays( GL_TRIANGLES, 0, 6 );
        glBindTexture( GL_TEXTURE_2D, 0 );
        glActiveTexture( TEXTURE_UNIT );
       }
    else
       {
        glDrawArrays( GL_TRIANGLES, 0, 6 );
       }
    m_texture.release( );
    m_vertexBuffer.release( );

    s_program->release( );
}


void CAD_background::renderShadow(GLWidget *widget)
{
    m_vertexBuffer.bind( );
    QOpenGLShaderProgram& depthProgram = widget->depthProgram( );
    depthProgram.enableAttributeArray( "position" );
    depthProgram.setAttributeBuffer("position",GL_FLOAT,0,3,sizeof(Vertex));
    depthProgram.setUniformValue( "modelMatrix", m_modelMatrix );
    glDrawArrays( GL_QUADS, 0, 6 );
    m_vertexBuffer.release();
}

void CAD_background::setVertexAttribute(int attributeLocation, GLenum elementType, quint32 elementSize, quint32 offset)
{
    s_program->enableAttributeArray( attributeLocation );
    s_program->setAttributeBuffer( attributeLocation,elementType,offset,elementSize,sizeof(Vertex));
}
