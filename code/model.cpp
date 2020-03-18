#include "model.h"
#include <qmath.h>
#include <QDebug>

CAD_Model::CAD_Model()
    : m_count(0),
      isCulling(true),
      isHiding(true),
      iswireframe(false)
{
    m_data.resize(80000);
    Wireframe_data.resize(80000);
}
void CAD_Model::loadModel()
{
    //qDebug() << "Start Loading Model......";
    //if(iswireframe){

    IniNormal ();
    //qDebug() << " Wireframe mode---";
    CreateWireframe();
    //}

    //else{

        //qDebug() << "  Object mode---";
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
    //}
    qDebug() <<"Test-------------->" <<vertexCount();

}

bool CAD_Model::CheckSameLine(double Px1,double Py1,double Pz1,double Px2,double Py2,double Pz2, PointNormal Triangle)
{
 //check line 1
    if((Px1 == Triangle.x0 && Py1 == Triangle.y0 && Pz1 == Triangle.z0) &&(Px2 == Triangle.x1 && Py2 == Triangle.y1 && Pz2 == Triangle.z1))
    {return true;}
    if((Px2 == Triangle.x0 && Py2 == Triangle.y0 && Pz2 == Triangle.z0) &&(Px1 == Triangle.x1 && Py1 == Triangle.y1 && Pz1 == Triangle.z1))
    {return true;}


    if((Px1 == Triangle.x1 && Py1 == Triangle.y1 && Pz1 == Triangle.z1) &&(Px2 == Triangle.x2 && Py2 == Triangle.y2 && Pz2 == Triangle.z2))
    {return true;}
    if((Px2 == Triangle.x1 && Py2 == Triangle.y1 && Pz2 == Triangle.z1) &&(Px1 == Triangle.x2 && Py1 == Triangle.y2 && Pz1 == Triangle.z2))
    {return true;}


    if((Px1 == Triangle.x2 && Py1 == Triangle.y2 && Pz1 == Triangle.z2) &&(Px2 == Triangle.x0 && Py2 == Triangle.y0 && Pz2 == Triangle.z0))
    {return true;}
    if((Px2 == Triangle.x1 && Py2 == Triangle.y1 && Pz2 == Triangle.z1) &&(Px1 == Triangle.x2 && Py1 == Triangle.y2 && Pz1 == Triangle.z2))
    {return true;}


    else{return false;}
}


void CAD_Model::CreateWireframe()
{
    //qDebug() << "  Create wireframe data";
    for(int i = 0 ; i<(int)Normal.size() ; i++ ){

        if(Normal.at(i).visible0)
          {
           addWire(QVector3D((GLfloat)Normal.at(i).x0,(GLfloat)Normal.at(i).y0,(GLfloat)Normal.at(i).z0));
           addWire(QVector3D((GLfloat)Normal.at(i).x1,(GLfloat)Normal.at(i).y1,(GLfloat)Normal.at(i).z1));
          }
        if(Normal.at(i).visible1)
          {
           addWire(QVector3D((GLfloat)Normal.at(i).x1,(GLfloat)Normal.at(i).y1,(GLfloat)Normal.at(i).z1));
           addWire(QVector3D((GLfloat)Normal.at(i).x2,(GLfloat)Normal.at(i).y2,(GLfloat)Normal.at(i).z2));
          }
        if(Normal.at(i).visible2)
          {
           addWire(QVector3D((GLfloat)Normal.at(i).x0,(GLfloat)Normal.at(i).y0,(GLfloat)Normal.at(i).z0));
           addWire(QVector3D((GLfloat)Normal.at(i).x2,(GLfloat)Normal.at(i).y2,(GLfloat)Normal.at(i).z2));
          }


    }
}
void CAD_Model::addWire(const QVector3D &v)
{
    GLfloat *p = Wireframe_data.data() + wire_count;
    *p++ = v.x();
    *p++ = v.y();
    *p++ = v.z();
    wire_count += 3;
}


void CAD_Model::add(const QVector3D &v, const QVector3D &n)
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



void CAD_Model::quad(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2, GLfloat x3, GLfloat y3, GLfloat x4, GLfloat y4)
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

void CAD_Model::extrude(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2)
{
    QVector3D n = QVector3D::normal(QVector3D(0.0f, 0.0f, -0.1f), QVector3D(x2 - x1, y2 - y1, 0.0f));

    add(QVector3D(x1, y1, +0.05f), n);
    add(QVector3D(x1, y1, -0.05f), n);
    add(QVector3D(x2, y2, +0.05f), n);

    add(QVector3D(x2, y2, -0.05f), n);
    add(QVector3D(x2, y2, +0.05f), n);
    add(QVector3D(x1, y1, -0.05f), n);
}

void CAD_Model::NormalNormalize(GLfloat& Nx,GLfloat& Ny,GLfloat& Nz)
{
    float L = sqrtf((float)Nx*(float)Nx+(float)Ny*(float)Ny+(float)Nz*(float)Nz);
    Nx = Nx/(GLfloat)L;
    if(Nx == -0){Nx = 0;}
    Ny = Ny/(GLfloat)L;
    if(Ny == -0){Ny = 0;}
    Nz = Nz/(GLfloat)L;
    if(Nz == -0){Nz = 0;}
}

int CAD_Model::checkData(PointNormal point,double x,double y,double z)
{
    if(x == point.x0 && y == point.y0 && z == point.z0){return 0;}
    if(x == point.x1 && y == point.y1 && z == point.z1){return 1;}
    if(x == point.x2 && y == point.y2 && z == point.z2){return 2;}
    return -1;
}

double CAD_Model::CalNormalAngle(double a1,double a2,double a3,double b1,double b2,double b3)
{
    double angle;
    angle = acos( (a1*b1+a2*b2+a3*b3)/( sqrt(a1*a1+a2*a2+a3*a3)*sqrt(b1*b1+b2*b2+b3*b3) ) );
    return angle*180/M_PI;
}

bool CAD_Model::checkNormal(PointNormal point,PointNormal _point)
{
    if(CalNormalAngle(point.Nx,point.Ny,point.Nz,_point.Nx,_point.Ny,_point.Nz)<30)
    {return true;}
    else
    {return false;}
}

void CAD_Model::SolveInvisible(PointNormal& point)
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
void CAD_Model::CalNormal(GLfloat& Nx,GLfloat& Ny,GLfloat& Nz,GLfloat V1x,GLfloat V1y,GLfloat V1z,GLfloat V2x,GLfloat V2y,GLfloat V2z)
{
    Nx = V1y*V2z -V1z*V2y;
    Ny = V1z*V2x -V1x*V2z;
    Nz = V1x*V2y -V1y*V2x;
}

void CAD_Model::IniNormal()
{
    for(int i = 0 ; i<(int)f.nObject.at(0)._3Dface.size() ; i++ ){
        GLfloat V1x,V1y,V1z,V2x,V2y,V2z;

        GLfloat Nx = 0 ;
        GLfloat Ny = 0 ;
        GLfloat Nz = 0 ;
        PointNormal temp,temp1;

        V1x =(GLfloat)f.nObject.at(0)._3Dface[i].x1 - (GLfloat)f.nObject.at(0)._3Dface[i].x0;
        V1y =(GLfloat)f.nObject.at(0)._3Dface[i].y1 - (GLfloat)f.nObject.at(0)._3Dface[i].y0;
        V1z =(GLfloat)f.nObject.at(0)._3Dface[i].z1 - (GLfloat)f.nObject.at(0)._3Dface[i].z0;

        V2x =(GLfloat)f.nObject.at(0)._3Dface[i].x3 - (GLfloat)f.nObject.at(0)._3Dface[i].x0;
        V2y =(GLfloat)f.nObject.at(0)._3Dface[i].y3 - (GLfloat)f.nObject.at(0)._3Dface[i].y0;
        V2z =(GLfloat)f.nObject.at(0)._3Dface[i].z3 - (GLfloat)f.nObject.at(0)._3Dface[i].z0;
        CalNormal(Nx,Ny,Nz,V1x,V1y,V1z,V2x,V2y,V2z);
        NormalNormalize(Nx,Ny,Nz);

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
         temp.Nx = Nx;
         temp.Ny = Ny;
         temp.Nz = Nz;
         temp.visible0 = true;
         temp.visible1 = true;
         temp.visible2 = true;
         if(isHiding){
         SolveInvisible(temp);
         }
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
         temp.Nx = Nx;
         temp.Ny = Ny;
         temp.Nz = Nz;
         temp.visible0 = true;
         temp.visible1 = true;
         temp.visible2 = true;
         if(isHiding){
         SolveInvisible(temp);
         }
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
         temp1.Nx = Nx;
         temp1.Ny = Ny;
         temp1.Nz = Nz;
         temp1.visible0 = true;
         temp1.visible1 = true;
         temp1.visible2 = true;
         if(isHiding){
         SolveInvisible(temp1);
         }
         Normal.push_back(temp1);
        }
    }
}
