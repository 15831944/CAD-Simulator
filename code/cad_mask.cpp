#include "cad_mask.h"
#include <math.h>
#include "glwidget.h"

#define TEXTURE_UNIT GL_TEXTURE0
#define SHADOW_TEXTURE_UNIT GL_TEXTURE1

QOpenGLShaderProgram* CAD_mask::s_program = Q_NULLPTR;
int CAD_mask::s_positionLoc,
    CAD_mask::s_normalLoc,
    CAD_mask::s_texCoordLoc,
    CAD_mask::s_modelMatrixLoc,
    CAD_mask::s_viewMatrixLoc,
    CAD_mask::s_projectionMatrixLoc,
    CAD_mask::s_lightViewMatrixLoc,
    CAD_mask::s_lightProjectionMatrixLoc,
    CAD_mask::s_lightPositionLoc,
    CAD_mask::s_modelViewNormalMatrixLoc,
    CAD_mask::s_shadowTypeLoc,
    CAD_mask::s_count,
    CAD_mask::IsWireframe,
    CAD_mask::s_color = 0;

CAD_mask::CAD_mask( QMatrix4x4& modelMatrix,
                              QMatrix4x4& viewMatrix,
                              const QString& fileName):
                              m_modelMatrix( modelMatrix ),
                              m_viewMatrix( viewMatrix ),
                              m_vertexBuffer( QOpenGLBuffer::VertexBuffer ),
                              w_vertexBuffer( QOpenGLBuffer::VertexBuffer )
{

    initializeOpenGLFunctions( );
    if ( s_count++ == 0 )
    {
        s_program = new QOpenGLShaderProgram;
        s_program->addShaderFromSourceFile( QOpenGLShader::Vertex,":/Shader/wireframe.vert" );
        s_program->addShaderFromSourceFile( QOpenGLShader::Fragment,":/Shader/wireframe.frag" );
        s_program->link( );
        s_program->bind( );

        s_positionLoc = s_program->attributeLocation( "position" );
        s_modelMatrixLoc = s_program->uniformLocation( "modelMatrix" );
        s_viewMatrixLoc = s_program->uniformLocation( "viewMatrix" );
        s_projectionMatrixLoc = s_program->uniformLocation( "projectionMatrix" );
        IsWireframe = s_program->uniformLocation("IsWireframe");
        s_color = s_program->uniformLocation("color");


        s_program->release( );
    }
    if(!dxf.in(fileName.toStdString(),&f)){qDebug()<<"Fail to open CAD file.";}
    qDebug() << "Sucessed load cad file";
    qDebug() << "Create Model...." << fileName;
    IniNormal();
    CreateModel();
    qDebug() << "..............OK";

    //<QVector> -> Vertex [Model]
    m_vertices = new Vertex[vector_position.size()];
    Vertex* v = m_vertices;
    for(int i =0;i<(int)vector_position.size();i++)
    {
     v[i].set(vector_position.at(i));
    }

    //<QVector> -> Vertex [Wireframe]
    w_vertices = new Vertex[Wire_vector_position.size()];
    Vertex* w = w_vertices;
    for(int i =0;i<(int)Wire_vector_position.size();i++)
    {
     w[i].set(Wire_vector_position.at(i));
    }


    //allocate model vertexBuffer
    m_vertexBuffer.setUsagePattern( QOpenGLBuffer::DynamicDraw );
    m_vertexBuffer.create( );
    m_vertexBuffer.bind( );
    m_vertexBuffer.allocate( v, (int)vector_position.size() * sizeof( Vertex ) );
    m_vertexBuffer.release( );

    //allocate wireframe vertexBuffer
    w_vertexBuffer.setUsagePattern( QOpenGLBuffer::DynamicDraw );
    w_vertexBuffer.create( );
    w_vertexBuffer.bind( );
    w_vertexBuffer.allocate( w, (int)Wire_vector_position.size() * sizeof( Vertex ) );
    w_vertexBuffer.release( );
}

CAD_mask::~CAD_mask( void )
{
    m_vertexBuffer.destroy( );
    delete []m_vertices;
    delete []w_vertices;
    if ( --s_count == 0 )
    {
        delete s_program;
    }
}

void CAD_mask::render(GLWidget *widget)
{
    //Render CAD model
    s_program->bind();
    m_vertexBuffer.bind();
    setVertexAttribute( s_positionLoc, GL_FLOAT, 3, 0 );
    s_program->setUniformValue( s_modelMatrixLoc, m_modelMatrix );
    s_program->setUniformValue( s_viewMatrixLoc, m_viewMatrix );
    s_program->setUniformValue( s_projectionMatrixLoc, widget->projectionMatrix( ) );
    s_program->setUniformValue( s_modelViewNormalMatrixLoc,( m_viewMatrix * m_modelMatrix ).normalMatrix( ) );
    s_program->setUniformValue( s_color,QVector3D(0.0,0.0,0.0));

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1.0,1.0);

    glDrawArrays( GL_TRIANGLES, 0, (int)vector_position.size() );
    glDisable(GL_POLYGON_OFFSET_FILL);
    m_vertexBuffer.release( );
    s_program->release( );



    //Render CAD wireframe
    s_program->bind();
    w_vertexBuffer.bind();
    setVertexAttribute( s_positionLoc, GL_FLOAT, 3, 0 );
    s_program->setUniformValue( s_modelMatrixLoc, m_modelMatrix );
    s_program->setUniformValue( s_viewMatrixLoc,  m_viewMatrix );
    s_program->setUniformValue( s_projectionMatrixLoc, widget->projectionMatrix( ) );
    s_program->setUniformValue( s_modelViewNormalMatrixLoc,( m_viewMatrix * m_modelMatrix ).normalMatrix( ) );
    s_program->setUniformValue( s_color,QVector3D(1.0,1.0,1.0));
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(-1.0,-1.0);

    glDrawArrays( GL_LINES, 0, (int)Wire_vector_position.size() );
    glDisable(GL_POLYGON_OFFSET_FILL);
    w_vertexBuffer.release( );
    s_program->release( );


}


void CAD_mask::renderShadow(GLWidget *widget)
{
    m_vertexBuffer.bind( );
    QOpenGLShaderProgram& depthProgram = widget->depthProgram( );
    depthProgram.enableAttributeArray( "position" );
    depthProgram.setAttributeBuffer("position",GL_FLOAT,0,3,sizeof(Vertex));
    depthProgram.setUniformValue( "modelMatrix", m_modelMatrix );
    glDrawArrays( GL_TRIANGLES, 0, (int)vector_position.size() );
    m_vertexBuffer.release();
}


void CAD_mask::setVertexAttribute(int attributeLocation, GLenum elementType, quint32 elementSize, quint32 offset)
{
    s_program->enableAttributeArray( attributeLocation );
    s_program->setAttributeBuffer( attributeLocation,elementType,offset,elementSize,sizeof(Vertex));
}

void CAD_mask::IniNormal()
{
    for(int i = 0 ; i<(int)f.nObject.at(0)._3Dface.size() ; i++ ){
        GLfloat V1x,V1y,V1z,V2x,V2y,V2z;
        PointNormal temp,temp1;

        V1x =(GLfloat)f.nObject.at(0)._3Dface[i].x1 - (GLfloat)f.nObject.at(0)._3Dface[i].x0;
        V1y =(GLfloat)f.nObject.at(0)._3Dface[i].y1 - (GLfloat)f.nObject.at(0)._3Dface[i].y0;
        V1z =(GLfloat)f.nObject.at(0)._3Dface[i].z1 - (GLfloat)f.nObject.at(0)._3Dface[i].z0;

        V2x =(GLfloat)f.nObject.at(0)._3Dface[i].x3 - (GLfloat)f.nObject.at(0)._3Dface[i].x0;
        V2y =(GLfloat)f.nObject.at(0)._3Dface[i].y3 - (GLfloat)f.nObject.at(0)._3Dface[i].y0;
        V2z =(GLfloat)f.nObject.at(0)._3Dface[i].z3 - (GLfloat)f.nObject.at(0)._3Dface[i].z0;
        QVector3D n = QVector3D::normal(QVector3D(V1x, V1y, V1z), QVector3D(V2x, V2y,V2z));

        if(f.nObject.at(0)._3Dface[i].x2 == f.nObject.at(0)._3Dface[i].x3 &&
           f.nObject.at(0)._3Dface[i].y2 == f.nObject.at(0)._3Dface[i].y3 &&
           f.nObject.at(0)._3Dface[i].z2 == f.nObject.at(0)._3Dface[i].z3)
        {temp.x0 = (GLfloat)f.nObject.at(0)._3Dface[i].x0;
         temp.y0 = (GLfloat)f.nObject.at(0)._3Dface[i].y0;
         temp.z0 = (GLfloat)f.nObject.at(0)._3Dface[i].z0;
         temp.x1 = (GLfloat)f.nObject.at(0)._3Dface[i].x1;
         temp.y1 = (GLfloat)f.nObject.at(0)._3Dface[i].y1;
         temp.z1 = (GLfloat)f.nObject.at(0)._3Dface[i].z1;
         temp.x2 = (GLfloat)f.nObject.at(0)._3Dface[i].x2;
         temp.y2 = (GLfloat)f.nObject.at(0)._3Dface[i].y2;
         temp.z2 = (GLfloat)f.nObject.at(0)._3Dface[i].z2;
         temp.Nx = n.x();
         temp.Ny = n.y();
         temp.Nz = n.z();
         temp.visible0 = true;
         temp.visible1 = true;
         temp.visible2 = true;
         SolveInvisible(temp);
         Normal.push_back(temp);
        }
        else
        {
         temp.x0 = (GLfloat)f.nObject.at(0)._3Dface[i].x0;
         temp.y0 = (GLfloat)f.nObject.at(0)._3Dface[i].y0;
         temp.z0 = (GLfloat)f.nObject.at(0)._3Dface[i].z0;
         temp.x1 = (GLfloat)f.nObject.at(0)._3Dface[i].x1;
         temp.y1 = (GLfloat)f.nObject.at(0)._3Dface[i].y1;
         temp.z1 = (GLfloat)f.nObject.at(0)._3Dface[i].z1;
         temp.x2 = (GLfloat)f.nObject.at(0)._3Dface[i].x2;
         temp.y2 = (GLfloat)f.nObject.at(0)._3Dface[i].y2;
         temp.z2 = (GLfloat)f.nObject.at(0)._3Dface[i].z2;
         temp.Nx = n.x();
         temp.Ny = n.y();
         temp.Nz = n.z();
         temp.visible0 = true;
         temp.visible1 = true;
         temp.visible2 = true;
         SolveInvisible(temp);
         Normal.push_back(temp);

         temp1.x0 = (GLfloat)f.nObject.at(0)._3Dface[i].x2;
         temp1.y0 = (GLfloat)f.nObject.at(0)._3Dface[i].y2;
         temp1.z0 = (GLfloat)f.nObject.at(0)._3Dface[i].z2;
         temp1.x1 = (GLfloat)f.nObject.at(0)._3Dface[i].x3;
         temp1.y1 = (GLfloat)f.nObject.at(0)._3Dface[i].y3;
         temp1.z1 = (GLfloat)f.nObject.at(0)._3Dface[i].z3;
         temp1.x2 = (GLfloat)f.nObject.at(0)._3Dface[i].x0;
         temp1.y2 = (GLfloat)f.nObject.at(0)._3Dface[i].y0;
         temp1.z2 = (GLfloat)f.nObject.at(0)._3Dface[i].z0;
         temp.Nx = n.x();
         temp.Ny = n.y();
         temp.Nz = n.z();
         temp1.visible0 = true;
         temp1.visible1 = true;
         temp1.visible2 = true;
         SolveInvisible(temp1);
         Normal.push_back(temp1);

        }
    }
}

int  CAD_mask::checkData(PointNormal point,double x,double y,double z)
{
    if(x == point.x0 && y == point.y0 && z == point.z0){return 0;}
    if(x == point.x1 && y == point.y1 && z == point.z1){return 1;}
    if(x == point.x2 && y == point.y2 && z == point.z2){return 2;}
    return -1;
}

double CAD_mask::CalNormalAngle(double a1,double a2,double a3,double b1,double b2,double b3)
{
    double angle;
    angle = acos( (a1*b1+a2*b2+a3*b3)/( sqrt(a1*a1+a2*a2+a3*a3)*sqrt(b1*b1+b2*b2+b3*b3) ) );
    return angle*180/M_PI;
}

bool CAD_mask::checkNormal(PointNormal point,PointNormal _point)
{
    if(CalNormalAngle(point.Nx,point.Ny,point.Nz,_point.Nx,_point.Ny,_point.Nz)<40
       ||CalNormalAngle(point.Nx,point.Ny,point.Nz,_point.Nx,_point.Ny,_point.Nz)>=180)
    {return true;}
    else
    {return false;}
}

void CAD_mask::SolveInvisible(PointNormal& point)
{
    for(int i = 0;i<(int)Normal.size();i++)
    {
     int count =0;
     int flag[3];
     flag[0] = checkData(Normal.at(i),point.x0,point.y0,point.z0);
     if(flag[0]!=-1)
     {count++; }

     flag[1] = checkData(Normal.at(i),point.x1,point.y1,point.z1);
     if(flag[1]!=-1)
     {count++; }

     flag[2] = checkData(Normal.at(i),point.x2,point.y2,point.z2);
     if(flag[2]!=-1)
     {count++; }

     if(count ==2)
     {
      int Mpoint=0,TMpoint=0;
      switch(flag[0])
      {
        case -1:switch(flag[1])
                {
                    case 0:switch(flag[2])
                           {
                              case 1:Mpoint=1;
                                     TMpoint=0;
                                     break;
                              case 2:Mpoint=1;
                                     TMpoint=2;
                                     break;
                           }
                            break;
                    case 1:switch(flag[2])
                           {
                              case 0:Mpoint=1;
                                     TMpoint=0;
                                     break;
                              case 2:Mpoint=1;
                                     TMpoint=1;
                                     break;
                           }
                            break;
                    case 2:switch(flag[2])
                           {
                              case 0:Mpoint=1;
                                     TMpoint=2;
                                     break;
                              case 1:Mpoint=1;
                                     TMpoint=1;
                                     break;
                           }
                            break;
                }
                break;

        case 0:switch(flag[1])
               {
                   case -1:switch(flag[2])
                        {
                            case 1:Mpoint=2;
                                   TMpoint=0;
                                   break;
                            case 2:Mpoint=2;
                                   TMpoint=2;
                                   break;
                        }
                        break;
                    case 1:switch(flag[2])
                        {
                            case -1:Mpoint=0;
                                    TMpoint=0;
                                    break;
                            case 2:break;
                        }
                        break;
                    case 2:switch(flag[2])
                        {
                            case -1:Mpoint=0;
                                    TMpoint=2;
                                    break;
                            case 1:break;
                        }
                        break;
                }
                break;
        case 1:switch(flag[1])
               {
                    case -1:switch(flag[2])
                        {
                            case 0:Mpoint=2;
                                   TMpoint=0;
                                   break;
                            case 2:Mpoint=2;
                                   TMpoint=2;
                                   break;
                        }
                        break;
                    case 0:switch(flag[2])
                        {
                            case -1:Mpoint=0;
                                    TMpoint=0;
                                    break;
                            case 2:break;
                        }
                        break;
                    case 2:switch(flag[2])
                        {
                            case -1:Mpoint=0;
                                    TMpoint=1;
                                    break;
                            case 0:break;
                        }
                        break;
                }
                break;
        case 2:switch(flag[1])
                {
                    case -1:switch(flag[2])
                        {
                            case 0:Mpoint=2;
                                   TMpoint=2;
                                   break;
                            case 1:Mpoint=2;
                                   TMpoint=1;
                                   break;
                        }
                        break;
                    case 0:switch(flag[2])
                        {
                            case -1:Mpoint=0;
                                    TMpoint=2;
                                    break;
                            case 1:break;
                        }
                        break;
                    case 1:switch(flag[2])
                        {
                            case -1:Mpoint=0;
                                    TMpoint=1;
                                    break;
                            case 0:break;
                        }
                        break;
                }
                break;
      }

      switch(Mpoint)
      {
      case 0:if(checkNormal(point,Normal.at(i)))
             {point.visible0 = false;
              switch(TMpoint)
              {
              case 0:Normal.at(i).visible0 = false;
                     break;
              case 1:Normal.at(i).visible1 = false;
                     break;
              case 2:Normal.at(i).visible2 = false;
                     break;
              }
             }
             else
             {
              point.visible0 = false;
             }
             break;

      case 1:if(checkNormal(point,Normal.at(i)))
             {point.visible1 = false;
              switch(TMpoint)
              {
              case 0:Normal.at(i).visible0 = false;
                     break;
              case 1:Normal.at(i).visible1 = false;
                     break;
              case 2:Normal.at(i).visible2 = false;
                     break;
              }
             }
             else
             {
              point.visible1 = false;
             }
             break;

      case 2:if(checkNormal(point,Normal.at(i)))
             {point.visible2 = false;
              switch(TMpoint)
              {
              case 0:Normal.at(i).visible0 = false;
                     break;
              case 1:Normal.at(i).visible1 = false;
                     break;
              case 2:Normal.at(i).visible2 = false;
                     break;
              }
             }
             else
             {
              point.visible2 = false;
             }
             break;

      }

     }
    }
}

void CAD_mask::CreateModel()
{
    for(int i = 0 ; i<(int)Normal.size() ; i++ ){

        if(Normal.at(i).visible0)
          {
           Wire_vector_position.push_back(QVector3D((GLfloat)Normal.at(i).x0,(GLfloat)Normal.at(i).y0,(GLfloat)Normal.at(i).z0));
           Wire_vector_position.push_back(QVector3D((GLfloat)Normal.at(i).x1,(GLfloat)Normal.at(i).y1,(GLfloat)Normal.at(i).z1));
          }
        if(Normal.at(i).visible1)
          {
           Wire_vector_position.push_back(QVector3D((GLfloat)Normal.at(i).x1,(GLfloat)Normal.at(i).y1,(GLfloat)Normal.at(i).z1));
           Wire_vector_position.push_back(QVector3D((GLfloat)Normal.at(i).x2,(GLfloat)Normal.at(i).y2,(GLfloat)Normal.at(i).z2));
          }
        if(Normal.at(i).visible2)
          {
           Wire_vector_position.push_back(QVector3D((GLfloat)Normal.at(i).x0,(GLfloat)Normal.at(i).y0,(GLfloat)Normal.at(i).z0));
           Wire_vector_position.push_back(QVector3D((GLfloat)Normal.at(i).x2,(GLfloat)Normal.at(i).y2,(GLfloat)Normal.at(i).z2));
          }
    }

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

           vector_position.push_back(QVector3D((GLfloat)f.nObject.at(0)._3Dface[i].x1,
                                               (GLfloat)f.nObject.at(0)._3Dface[i].y1,
                                               (GLfloat)f.nObject.at(0)._3Dface[i].z1));
           vector_normal.push_back(n);

           vector_position.push_back(QVector3D((GLfloat)f.nObject.at(0)._3Dface[i].x2,
                                               (GLfloat)f.nObject.at(0)._3Dface[i].y2,
                                               (GLfloat)f.nObject.at(0)._3Dface[i].z2));
           vector_normal.push_back(n);


          }
        //第二種狀況 : 3dface 為四邊形 -> 分割為兩三角形
        else
          {
           vector_position.push_back(QVector3D((GLfloat)f.nObject.at(0)._3Dface[i].x0,
                                               (GLfloat)f.nObject.at(0)._3Dface[i].y0,
                                               (GLfloat)f.nObject.at(0)._3Dface[i].z0));
           vector_normal.push_back(n);

           vector_position.push_back(QVector3D((GLfloat)f.nObject.at(0)._3Dface[i].x1,
                                               (GLfloat)f.nObject.at(0)._3Dface[i].y1,
                                               (GLfloat)f.nObject.at(0)._3Dface[i].z1));
           vector_normal.push_back(n);
           vector_position.push_back(QVector3D((GLfloat)f.nObject.at(0)._3Dface[i].x2,
                                               (GLfloat)f.nObject.at(0)._3Dface[i].y2,
                                               (GLfloat)f.nObject.at(0)._3Dface[i].z2));
           vector_normal.push_back(n);

           vector_position.push_back(QVector3D((GLfloat)f.nObject.at(0)._3Dface[i].x2,
                                               (GLfloat)f.nObject.at(0)._3Dface[i].y2,
                                               (GLfloat)f.nObject.at(0)._3Dface[i].z2));
           vector_normal.push_back(n);

           vector_position.push_back(QVector3D((GLfloat)f.nObject.at(0)._3Dface[i].x3,
                                               (GLfloat)f.nObject.at(0)._3Dface[i].y3,
                                               (GLfloat)f.nObject.at(0)._3Dface[i].z3));
           vector_normal.push_back(n);

           vector_position.push_back(QVector3D((GLfloat)f.nObject.at(0)._3Dface[i].x0,
                                               (GLfloat)f.nObject.at(0)._3Dface[i].y0,
                                               (GLfloat)f.nObject.at(0)._3Dface[i].z0));
           vector_normal.push_back(n);

          }

    }
    m_modelMatrix.translate(-(Xmax-Xmin)/2,-(Ymax-Ymin+4)/2,-(Zmax-Zmin)/2);

}
