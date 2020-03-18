#include "cad_model.h"
#include <math.h>
#include "glwidget.h"
#include <QTime>

#define TEXTURE_UNIT GL_TEXTURE0
#define SHADOW_TEXTURE_UNIT GL_TEXTURE1

QOpenGLShaderProgram* CAD_model::s_program = Q_NULLPTR;
int CAD_model::s_positionLoc,
    CAD_model::s_normalLoc,
    CAD_model::s_texCoordLoc,
    CAD_model::s_modelMatrixLoc,
    CAD_model::s_viewMatrixLoc,
    CAD_model::s_projectionMatrixLoc,
    CAD_model::s_lightViewMatrixLoc,
    CAD_model::s_lightProjectionMatrixLoc,
    CAD_model::s_lightPositionLoc,
    CAD_model::s_modelViewNormalMatrixLoc,
    CAD_model::s_shadowTypeLoc,
    CAD_model::s_count = 0;



CAD_model::CAD_model( QMatrix4x4& modelMatrix,
                      QMatrix4x4& viewMatrix,
                      QVector3D&  lightposition,
                      QMatrix4x4& lightViewMatrix,
                      const QString& fileName,
                      const QString& texfileName,
                      ShadowType shadowType ):             
                      Xmax(0),Xmin(0),Ymax(0),Ymin(0),Zmax(0),Zmin(0),
                      m_modelMatrix( modelMatrix ),
                      m_viewMatrix( viewMatrix ),
                      m_lightposition(lightposition),
                      m_lightViewMatrix(lightViewMatrix),
                      m_shadowType( shadowType ),
                      m_vertexBuffer( QOpenGLBuffer::VertexBuffer ),
                      m_texture( QOpenGLTexture::Target2D ),
                      ambient(0.8),
                      s_ambient(0)


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
    if(!dxf.in(fileName.toStdString(),&f)){qDebug()<<"Fail to open CAD file.";}
    qDebug() << "Sucessed load cad file";
    qDebug() << "Create Model....";
    CreateModel();
    qDebug() << "..............OK";
    m_vertices = new Vertex[vector_position.size()];
    Vertex* v = m_vertices;
    for(int i =0;i<(int)vector_position.size();i++)
    {
     v[i].set(vector_position.at(i),vector_normal.at(i),vector_texCoord.at(i));
    }

    m_vertexBuffer.setUsagePattern( QOpenGLBuffer::DynamicDraw );
    m_vertexBuffer.create( );
    m_vertexBuffer.bind( );
    m_vertexBuffer.allocate( v, (int)vector_position.size() * sizeof( Vertex ) );
    m_vertexBuffer.release( );

    m_texture.setMinificationFilter( QOpenGLTexture::LinearMipMapLinear );
    m_texture.setMagnificationFilter( QOpenGLTexture::Linear );
    QImage image( texfileName );
    Q_ASSERT( !texfileName.isNull( ) );
    m_texture.setData( image );

}

CAD_model::~CAD_model( void )
{
    m_vertexBuffer.destroy( );
    m_texture.destroy( );
    delete []m_vertices;
    if ( --s_count == 0 )
    {
        delete s_program;
    }
}

void CAD_model::render(GLWidget *widget)
{
    //qDebug() << "render CAD model";
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
        glDrawArrays( GL_TRIANGLES, 0, (int)vector_position.size() );
        glBindTexture( GL_TEXTURE_2D, 0 );
        glActiveTexture( TEXTURE_UNIT );
       }
    else
       {
        glDrawArrays( GL_TRIANGLES, 0, (int)vector_position.size() );
       }
    m_texture.release( );
    m_vertexBuffer.release( );

    s_program->release( );
}

void CAD_model::renderShadow(GLWidget *widget)
{
    m_vertexBuffer.bind( );
    QOpenGLShaderProgram& depthProgram = widget->depthProgram( );
    depthProgram.enableAttributeArray( "position" );
    depthProgram.setAttributeBuffer("position",GL_FLOAT,0,3,sizeof(Vertex));
    depthProgram.setUniformValue( "modelMatrix", m_modelMatrix );
    glDrawArrays( GL_TRIANGLES, 0, (int)vector_position.size() );
    m_vertexBuffer.release();
}

void CAD_model::setVertexAttribute(int attributeLocation, GLenum elementType, quint32 elementSize, quint32 offset)
{
    s_program->enableAttributeArray( attributeLocation );
    s_program->setAttributeBuffer( attributeLocation,elementType,offset,elementSize,sizeof(Vertex));
}

void CAD_model::CreateModel()
{

    const QVector2D Triangle_texCoords[] =
        {
            QVector2D( 0.0, 0.0 ),
            QVector2D( 1.0, 0.0 ),
            QVector2D( 0.5, 1.0 )
        };

    for(int i = 0 ; i<(int)f.nObject.at(0)._3Dface.size() ; i++ )
    {
        //find MAX xyz
        if(i==0)
        {
            Xmax = (GLfloat)f.nObject.at(0)._3Dface[i].x0;
            Xmin = (GLfloat)f.nObject.at(0)._3Dface[i].x0;

            if((GLfloat)f.nObject.at(0)._3Dface[i].x1>Xmax){Xmax=(GLfloat)f.nObject.at(0)._3Dface[i].x1;}
            if((GLfloat)f.nObject.at(0)._3Dface[i].x2>Xmax){Xmax=(GLfloat)f.nObject.at(0)._3Dface[i].x2;}
            if((GLfloat)f.nObject.at(0)._3Dface[i].x3>Xmax){Xmax=(GLfloat)f.nObject.at(0)._3Dface[i].x3;}

            if((GLfloat)f.nObject.at(0)._3Dface[i].x1<Xmin){Xmin=(GLfloat)f.nObject.at(0)._3Dface[i].x1;}
            if((GLfloat)f.nObject.at(0)._3Dface[i].x2<Xmin){Xmin=(GLfloat)f.nObject.at(0)._3Dface[i].x2;}
            if((GLfloat)f.nObject.at(0)._3Dface[i].x3<Xmin){Xmin=(GLfloat)f.nObject.at(0)._3Dface[i].x3;}

            Ymax = (GLfloat)f.nObject.at(0)._3Dface[i].y0;
            Ymin = (GLfloat)f.nObject.at(0)._3Dface[i].y0;

            if((GLfloat)f.nObject.at(0)._3Dface[i].y1>Ymax){Ymax=(GLfloat)f.nObject.at(0)._3Dface[i].y1;}
            if((GLfloat)f.nObject.at(0)._3Dface[i].y2>Ymax){Ymax=(GLfloat)f.nObject.at(0)._3Dface[i].y2;}
            if((GLfloat)f.nObject.at(0)._3Dface[i].y3>Ymax){Ymax=(GLfloat)f.nObject.at(0)._3Dface[i].y3;}

            if((GLfloat)f.nObject.at(0)._3Dface[i].y1<Ymin){Ymin=(GLfloat)f.nObject.at(0)._3Dface[i].y1;}
            if((GLfloat)f.nObject.at(0)._3Dface[i].y2<Ymin){Ymin=(GLfloat)f.nObject.at(0)._3Dface[i].y2;}
            if((GLfloat)f.nObject.at(0)._3Dface[i].y3<Ymin){Ymin=(GLfloat)f.nObject.at(0)._3Dface[i].y3;}

            Zmax = (GLfloat)f.nObject.at(0)._3Dface[i].z0;
            Zmin = (GLfloat)f.nObject.at(0)._3Dface[i].z0;

            if((GLfloat)f.nObject.at(0)._3Dface[i].z1>Zmax){Zmax=(GLfloat)f.nObject.at(0)._3Dface[i].z1;}
            if((GLfloat)f.nObject.at(0)._3Dface[i].z2>Zmax){Zmax=(GLfloat)f.nObject.at(0)._3Dface[i].z2;}
            if((GLfloat)f.nObject.at(0)._3Dface[i].z3>Zmax){Zmax=(GLfloat)f.nObject.at(0)._3Dface[i].z3;}

            if((GLfloat)f.nObject.at(0)._3Dface[i].z1<Zmin){Zmin=(GLfloat)f.nObject.at(0)._3Dface[i].z1;}
            if((GLfloat)f.nObject.at(0)._3Dface[i].z2<Zmin){Zmin=(GLfloat)f.nObject.at(0)._3Dface[i].z2;}
            if((GLfloat)f.nObject.at(0)._3Dface[i].z3<Zmin){Zmin=(GLfloat)f.nObject.at(0)._3Dface[i].z3;}

        }
        else
        {
            if((GLfloat)f.nObject.at(0)._3Dface[i].x0>Xmax){Xmax=(GLfloat)f.nObject.at(0)._3Dface[i].x0;}
            if((GLfloat)f.nObject.at(0)._3Dface[i].x1>Xmax){Xmax=(GLfloat)f.nObject.at(0)._3Dface[i].x1;}
            if((GLfloat)f.nObject.at(0)._3Dface[i].x2>Xmax){Xmax=(GLfloat)f.nObject.at(0)._3Dface[i].x2;}
            if((GLfloat)f.nObject.at(0)._3Dface[i].x3>Xmax){Xmax=(GLfloat)f.nObject.at(0)._3Dface[i].x3;}

            if((GLfloat)f.nObject.at(0)._3Dface[i].x0<Xmin){Xmin=(GLfloat)f.nObject.at(0)._3Dface[i].x0;}
            if((GLfloat)f.nObject.at(0)._3Dface[i].x1<Xmin){Xmin=(GLfloat)f.nObject.at(0)._3Dface[i].x1;}
            if((GLfloat)f.nObject.at(0)._3Dface[i].x2<Xmin){Xmin=(GLfloat)f.nObject.at(0)._3Dface[i].x2;}
            if((GLfloat)f.nObject.at(0)._3Dface[i].x3<Xmin){Xmin=(GLfloat)f.nObject.at(0)._3Dface[i].x3;}

            if((GLfloat)f.nObject.at(0)._3Dface[i].y0>Ymax){Ymax=(GLfloat)f.nObject.at(0)._3Dface[i].y0;}
            if((GLfloat)f.nObject.at(0)._3Dface[i].y1>Ymax){Ymax=(GLfloat)f.nObject.at(0)._3Dface[i].y1;}
            if((GLfloat)f.nObject.at(0)._3Dface[i].y2>Ymax){Ymax=(GLfloat)f.nObject.at(0)._3Dface[i].y2;}
            if((GLfloat)f.nObject.at(0)._3Dface[i].y3>Ymax){Ymax=(GLfloat)f.nObject.at(0)._3Dface[i].y3;}

            if((GLfloat)f.nObject.at(0)._3Dface[i].y0<Ymin){Ymin=(GLfloat)f.nObject.at(0)._3Dface[i].y0;}
            if((GLfloat)f.nObject.at(0)._3Dface[i].y1<Ymin){Ymin=(GLfloat)f.nObject.at(0)._3Dface[i].y1;}
            if((GLfloat)f.nObject.at(0)._3Dface[i].y2<Ymin){Ymin=(GLfloat)f.nObject.at(0)._3Dface[i].y2;}
            if((GLfloat)f.nObject.at(0)._3Dface[i].y3<Ymin){Ymin=(GLfloat)f.nObject.at(0)._3Dface[i].y3;}

            if((GLfloat)f.nObject.at(0)._3Dface[i].z0>Zmax){Zmax=(GLfloat)f.nObject.at(0)._3Dface[i].z0;}
            if((GLfloat)f.nObject.at(0)._3Dface[i].z1>Zmax){Zmax=(GLfloat)f.nObject.at(0)._3Dface[i].z1;}
            if((GLfloat)f.nObject.at(0)._3Dface[i].z2>Zmax){Zmax=(GLfloat)f.nObject.at(0)._3Dface[i].z2;}
            if((GLfloat)f.nObject.at(0)._3Dface[i].z3>Zmax){Zmax=(GLfloat)f.nObject.at(0)._3Dface[i].z3;}

            if((GLfloat)f.nObject.at(0)._3Dface[i].z0<Zmin){Zmin=(GLfloat)f.nObject.at(0)._3Dface[i].z0;}
            if((GLfloat)f.nObject.at(0)._3Dface[i].z1<Zmin){Zmin=(GLfloat)f.nObject.at(0)._3Dface[i].z1;}
            if((GLfloat)f.nObject.at(0)._3Dface[i].z2<Zmin){Zmin=(GLfloat)f.nObject.at(0)._3Dface[i].z2;}
            if((GLfloat)f.nObject.at(0)._3Dface[i].z3<Zmin){Zmin=(GLfloat)f.nObject.at(0)._3Dface[i].z3;}
        }

        GLfloat V1x,V1y,V1z,V2x,V2y,V2z;
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
           vector_position.push_back(QVector3D((GLfloat)f.nObject.at(0)._3Dface[i].x0,
                                               (GLfloat)f.nObject.at(0)._3Dface[i].y0,
                                               (GLfloat)f.nObject.at(0)._3Dface[i].z0));
           vector_normal.push_back(n);
           vector_texCoord.push_back(Triangle_texCoords[0]);

           vector_position.push_back(QVector3D((GLfloat)f.nObject.at(0)._3Dface[i].x1,
                                               (GLfloat)f.nObject.at(0)._3Dface[i].y1,
                                               (GLfloat)f.nObject.at(0)._3Dface[i].z1));
           vector_normal.push_back(n);
           vector_texCoord.push_back(Triangle_texCoords[1]);

           vector_position.push_back(QVector3D((GLfloat)f.nObject.at(0)._3Dface[i].x2,
                                               (GLfloat)f.nObject.at(0)._3Dface[i].y2,
                                               (GLfloat)f.nObject.at(0)._3Dface[i].z2));
           vector_normal.push_back(n);
           vector_texCoord.push_back(Triangle_texCoords[2]);


          }
        //第二種狀況 : 3dface 為四邊形 -> 分割為兩三角形
        else
          {
           vector_position.push_back(QVector3D((GLfloat)f.nObject.at(0)._3Dface[i].x0,
                                               (GLfloat)f.nObject.at(0)._3Dface[i].y0,
                                               (GLfloat)f.nObject.at(0)._3Dface[i].z0));
           vector_normal.push_back(n);
           vector_texCoord.push_back(Triangle_texCoords[0]);

           vector_position.push_back(QVector3D((GLfloat)f.nObject.at(0)._3Dface[i].x1,
                                               (GLfloat)f.nObject.at(0)._3Dface[i].y1,
                                               (GLfloat)f.nObject.at(0)._3Dface[i].z1));
           vector_normal.push_back(n);
           vector_texCoord.push_back(Triangle_texCoords[1]);

           vector_position.push_back(QVector3D((GLfloat)f.nObject.at(0)._3Dface[i].x2,
                                               (GLfloat)f.nObject.at(0)._3Dface[i].y2,
                                               (GLfloat)f.nObject.at(0)._3Dface[i].z2));
           vector_normal.push_back(n);
           vector_texCoord.push_back(Triangle_texCoords[2]);

           vector_position.push_back(QVector3D((GLfloat)f.nObject.at(0)._3Dface[i].x2,
                                               (GLfloat)f.nObject.at(0)._3Dface[i].y2,
                                               (GLfloat)f.nObject.at(0)._3Dface[i].z2));
           vector_normal.push_back(n);
           vector_texCoord.push_back(Triangle_texCoords[0]);

           vector_position.push_back(QVector3D((GLfloat)f.nObject.at(0)._3Dface[i].x3,
                                               (GLfloat)f.nObject.at(0)._3Dface[i].y3,
                                               (GLfloat)f.nObject.at(0)._3Dface[i].z3));
           vector_normal.push_back(n);
           vector_texCoord.push_back(Triangle_texCoords[1]);

           vector_position.push_back(QVector3D((GLfloat)f.nObject.at(0)._3Dface[i].x0,
                                               (GLfloat)f.nObject.at(0)._3Dface[i].y0,
                                               (GLfloat)f.nObject.at(0)._3Dface[i].z0));
           vector_normal.push_back(n);
           vector_texCoord.push_back(Triangle_texCoords[2]);
          }

    }
    m_modelMatrix.translate(-(Xmax-Xmin)/2,-(Ymax-Ymin+4)/2,-(Zmax-Zmin)/2);
}
