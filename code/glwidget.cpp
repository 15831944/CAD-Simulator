#include "glwidget.h"
#include <QMouseEvent>
#include <QOpenGLShaderProgram>
#include <QOpenGLFramebufferObject>
#include <QCoreApplication>
#include <math.h>
#include <QDebug>
#include <QTime>

#define TEXTURE_UNIT    GL_TEXTURE0
#define SHADOW_TEXTURE_UNIT GL_TEXTURE1

bool GLWidget::m_transparent = false;

GLWidget::GLWidget(QWidget *parent)
    : QOpenGLWidget(parent),
      Theta(0),
      Phi(0),
      R(150),
      TranX(0),
      TranY(0),
      Cam(0),
      RandomShiftX(0),
      RandomShiftY(0),
      RandomRot(0),
      isLoaded(false),
      iswireframe(false),ismask(false),
      RandomLight(true),
      RandomShift(false),
      RandomCamRot(false),
      IsEuler(false),
      Xmax(0),Xmin(0),Ymax(0),Ymin(0),Zmax(0),Zmin(0),
      m_FBO(0),
      m_FBO2(0),
      m_shadowMapWidth(1024),
      m_shadowMapHeight(1024),  
      depthMapFBO(0)

{
    // --transparent causes the clear color to be transparent. Therefore, on systems that
    // support it, the widget will become transparent apart from the model.
    if (m_transparent) {
        QSurfaceFormat fmt = format();
        fmt.setDepthBufferSize(24);
        fmt.setAlphaBufferSize(8);
        setFormat(fmt);
    }
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
}

GLWidget::~GLWidget()
{
    cleanup();
}

QSize GLWidget::minimumSizeHint() const
{
    return QSize(50, 50);
}

QSize GLWidget::sizeHint() const
{
    return QSize(400, 400);
}



void GLWidget::setTheta(int theta)
{

    if (theta != Theta) {
        Theta = theta;
        emit ThetaChanged(theta);
        update();
    }
}

void GLWidget::setPhi(int phi)
{

    if (phi != Phi) {
        Phi = phi;
        emit PhiChanged(phi);
        update();
    }
}

void GLWidget::setR(int r)
{

    if (r != R) {
        R = r;
        emit RChanged(r);
        update();
    }
}

void GLWidget::setRotate(int rotate)
{
    if(rotate!=Cam){
       Cam=rotate;
       emit RotateChange(rotate);
       update();
    }
}

void GLWidget::setTranX(int tranX)
{
    if (tranX != TranX) {
        TranX = tranX;
        emit TranslateX(tranX);
        update();
    }
}

void GLWidget::setTranY(int tranY)
{
    if (tranY != TranY) {
        TranY = tranY;
        emit TranslateY(tranY);
        update();
    }
}

void GLWidget::cleanup()
{
    makeCurrent();
    m_Vbo.destroy();
    back_Vbo.destroy();
    Wireframe_Vbo.destroy();
    delete m_FBO;
    delete m_FBO2;
    doneCurrent();
}
void GLWidget::Add_DShader_Link()
{
    m_depthProgram.addShaderFromSourceFile( QOpenGLShader::Vertex,
                                               ":/Shader/Depth.vert" );
    m_depthProgram.addShaderFromSourceFile( QOpenGLShader::Fragment,
                                               ":/Shader/Depth.frag" );
    m_depthProgram.link( );
}

void GLWidget::initializeGL(void)
{
    connect( context( ), SIGNAL( aboutToBeDestroyed( ) ),this, SLOT( cleanup( ) ) );

    initializeOpenGLFunctions();
    printVersionInformation();
    glClearColor(0.0,0.0, 0.0, 1);
    glEnable( GL_DEPTH_TEST );
    glEnable( GL_CULL_FACE );
    generateShadowFBO();
    Add_DShader_Link();

}

void GLWidget::generateShadowFBO(void)
{
    QSize size( m_shadowMapWidth, m_shadowMapHeight );
    m_FBO = new QOpenGLFramebufferObject( size );
}

void GLWidget::printVersionInformation()
{
  QString glType;
  QString glVersion;
  QString glProfile;

  // Get Version Information
  glType = (context()->isOpenGLES()) ? "OpenGL ES" : "OpenGL";
  glVersion = reinterpret_cast<const char*>(glGetString(GL_VERSION));

  // Get Profile Information
#define CASE(c) case QSurfaceFormat::c: glProfile = #c; break
  switch (format().profile())
  {
    CASE(NoProfile);
    CASE(CoreProfile);
    CASE(CompatibilityProfile);
  }
#undef CASE

  // qPrintable() will print our QString w/o quotes around it.
  qDebug() << qPrintable(glType) << qPrintable(glVersion) << "(" << qPrintable(glProfile) << ")";
}

void GLWidget::CalRandomShift()
{
    int baseX = 40;
    int baseY = 10;
    int baseR = 100;
    int RandomRangeX = 0;
    int RandomRangeY = 0;

    RandomRangeX = fabs(baseX+0.5*(R-baseR));
    RandomRangeY = fabs(baseY+0.4*(R-baseR));

    RandomShiftX = qrand() % ((RandomRangeX+1)+RandomRangeX)-RandomRangeX;
    RandomShiftY = qrand() % ((RandomRangeY+1)+RandomRangeY)-RandomRangeY;
}

void GLWidget::render(void)
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    /*
    if(ismask){
               CADmask->render(this);
              }*/
    if(iswireframe){
                    CADwireframe->render(this);
                   }
    else{
         CADmodel->render(this);
         CADmodel_back->render(this);
        }
}

void GLWidget::createModel(const QString &fileName)
{

    SetViewANDLight();  // set m_viewMatrix & m_lightViewMatrix

    m_modelMatrices.setToIdentity();

    m_backgroundMatrices.setToIdentity();
    m_backgroundMatrices.translate(-320, -240, -20);
    CADmodel = new CAD_model(m_modelMatrices,
                             m_viewMatrix,
                             m_rotatedLightPosition,
                             m_lightViewMatrix,
                             fileName,":/Textures/model.jpg",CAD_model::SimpleShadow);
    CADmodel_back = new CAD_background(m_backgroundMatrices,
                                       back_viewMatrix,
                                       m_rotatedLightPosition,
                                       m_lightViewMatrix,
                                       ":/Textures/real0.jpg",CAD_background::SimpleShadow);

    Xmax = CADmodel->Xmax;
    Xmin = CADmodel->Xmin;
    Ymax = CADmodel->Ymax;
    Ymin = CADmodel->Ymin;
    Zmax = CADmodel->Zmax;
    Zmin = CADmodel->Zmin;

    isLoaded = true;
}

void GLWidget::createWireframe(const QString &fileName,int mode)
{
    SetViewANDLight();
    w_modelMatrix.setToIdentity();
    if(mode ==0){CADwireframe = new CAD_wireframe(w_modelMatrix,m_viewMatrix,fileName,false);}
    else{CADwireframe = new CAD_wireframe(w_modelMatrix,m_viewMatrix,fileName,true);}

    Xmax = CADwireframe->Xmax;
    Xmin = CADwireframe->Xmin;
    Ymax = CADwireframe->Ymax;
    Ymin = CADwireframe->Ymin;
    Zmax = CADwireframe->Zmax;
    Zmin = CADwireframe->Zmin;

    isLoaded =true;
    iswireframe = true;
}
void GLWidget::createMask(const QString &fileName)
{
    SetViewANDLight();
    CADmask = new CAD_mask(w_modelMatrix,m_viewMatrix,fileName);

    Xmax = CADmask->Xmax;
    Xmin = CADmask->Xmin;
    Ymax = CADmask->Ymax;
    Ymin = CADmask->Ymin;
    Zmax = CADmask->Zmax;
    Zmin = CADmask->Zmin;

    isLoaded =true;
    ismask = true;
}

void GLWidget::SetViewANDLight(void)
{
    double Nx,Ny,Nz;
    if(Phi == 90 || Phi ==270){Nx = 0;}
    else{Nx = R * sin(Theta*M_PI/180) * cos(Phi*M_PI/180);}

    Ny = R * sin(Theta*M_PI/180) * sin(Phi*M_PI/180);
    if(Theta == 90|| Theta ==270)   {Nz =0;}
    else    {Nz = R * cos(Theta*M_PI/180);}

    QVector3D light;

    if(RandomLight){

        int Random_theta = qrand() % ((RandomLMaxTheta+1)-RandomLMinTheta)+RandomLMinTheta;
        int Random_phi   = qrand() % ((RandomLMaxPhi+1)-RandomLMinPhi)+RandomLMinPhi;
        double RNx,RNy,RNz;
        if(Random_phi == 90 || Random_phi ==270){RNx = 0;}
        else{RNx = R * sin(Random_theta*M_PI/180) * cos(Random_phi*M_PI/180);}
        RNy = R * sin(Random_theta*M_PI/180) * sin(Random_phi*M_PI/180);
        if(Random_theta == 90|| Random_theta ==270)   {RNz =0;}
        else    {RNz = R * cos(Random_theta*M_PI/180);}

        if(RandomShift){light = QVector3D( RNx-RandomShiftX,RNy-RandomShiftY,RNz);}
        else{light = QVector3D( RNx-TranX,RNy-TranY,RNz);}

    }
    else{
        if(RandomShift){light = QVector3D( Ny-RandomShiftY,Nx-RandomShiftX,Nz);}
        else{light = QVector3D( Ny-TranY,Nx-TranX,Nz);}
        }
    QVector3D CameraFinal = QVector3D(0,0,0);
    QVector3D CameraCenter= QVector3D(0,0,0);
    QVector3D CameraUP    = QVector3D(0,1,0);
    if(IsEuler){//定義尤拉座標系 :: 旋轉作用在模型上 :: 平移作用在相機上
                //定義相機座標系 :: 相機固定位置:: 負責平移
        if(RandomShift){eye = QVector3D(0-RandomShiftX, 0-RandomShiftY, R);
                        center = QVector3D(0-RandomShiftX, 0-RandomShiftY, 0.0);}
        else{eye = QVector3D(0-TranX, 0-TranY, R);
             center = QVector3D(0-TranX, 0-TranY, 0.0);}
        up = QVector3D(0.0,1.0,0.0);

        m_viewMatrix.setToIdentity( );
        m_viewMatrix.lookAt(eye,center,up);

        //定義光源座標系
        m_lightViewMatrix.setToIdentity( );
        m_lightViewMatrix.lookAt(m_rotatedLightPosition,center,up);

        //定義模型座標系 :: 模型只負責自己的旋轉 :: 不移動
        QVector3D NewModelPos;
        NewModelPos = QVector3D(-(Xmax-Xmin)/2,-(Ymax-Ymin+4)/2,-(Zmax-Zmin)/2);

        m_modelMatrices.setToIdentity();
        if(RandomCamRot){m_modelMatrices.rotate(-RandomRot,0.0,0.0,1.0);}
        else{m_modelMatrices.rotate(-Cam,0.0,0.0,1.0);}
        m_modelMatrices.rotate(Phi,1.0,0.0,0.0);
        m_modelMatrices.rotate(-Theta,0.0,1.0,0.0);
        m_modelMatrices.translate(NewModelPos);

        w_modelMatrix.setToIdentity();
        if(RandomCamRot){w_modelMatrix.rotate(-RandomRot,0.0,0.0,1.0);}
        else{w_modelMatrix.rotate(-Cam,0.0,0.0,1.0);}
        w_modelMatrix.rotate(Phi,1.0,0.0,0.0);
        w_modelMatrix.rotate(-Theta,0.0,1.0,0.0);
        w_modelMatrix.translate(NewModelPos);

        //建立背景座標系
        QVector3D NewBackGroundPos ;
        NewBackGroundPos = QVector3D(-320,-240,-20);

        m_backgroundMatrices.setToIdentity();
        m_backgroundMatrices.translate(NewBackGroundPos);

        back_viewMatrix.setToIdentity( );
        back_viewMatrix.lookAt(eye,center,up);
    }

    else{//定義球座標系   :: 模型旋轉利用相機位置改變達成 :: 平移作用在相機上
         //定義相機座標系 :: 相機位置於球面上 :: 負責平移
        if(RandomShift){
            QMatrix4x4 CameraMatrix ;

            CameraMatrix.setToIdentity();
            CameraMatrix.translate(0,0,150);
            CameraFinal = CameraMatrix*CameraFinal;

            CameraMatrix.setToIdentity();
            if(RandomCamRot){CameraMatrix.rotate(RandomRot,0,0,1);}
            else{CameraMatrix.rotate(Cam,0,0,1);}
            CameraUP = CameraMatrix*CameraUP;

            CameraMatrix.setToIdentity();
            CameraMatrix.rotate(Phi  ,0,0,1);
            CameraMatrix.rotate(Theta,0,1,0);
            CameraMatrix.translate(-RandomShiftX,RandomShiftY,0);
            CameraCenter= CameraMatrix*CameraCenter;
            CameraFinal = CameraMatrix*CameraFinal;

            eye    = QVector3D(Nx-RandomShiftX, Ny-RandomShiftY, Nz);
            center = QVector3D(0-RandomShiftX, 0-RandomShiftY, 0.0);

        }
        else{QMatrix4x4 CameraMatrix ;

             CameraMatrix.setToIdentity();
             CameraMatrix.translate(0,0,150);
             CameraFinal = CameraMatrix*CameraFinal;

             CameraMatrix.setToIdentity();
             if(RandomCamRot){CameraMatrix.rotate(RandomRot,0,0,1);}
             else{CameraMatrix.rotate(Cam,0,0,1);}
             CameraUP = CameraMatrix*CameraUP;

             CameraMatrix.setToIdentity();
             CameraMatrix.rotate(Phi  ,0,0,1);
             CameraMatrix.rotate(Theta,0,1,0);
             CameraMatrix.translate(-TranX,TranY,0);
             CameraCenter= CameraMatrix*CameraCenter;
             CameraFinal = CameraMatrix*CameraFinal;

             eye = QVector3D(Nx-TranX, Ny-TranY, Nz);
             center = QVector3D(0-TranX, 0-TranY, 0.0);
        }

        if(RandomCamRot){up = QVector3D(1.0*sin((360-RandomRot)*M_PI/180), 1.0*cos((360-RandomRot)*M_PI/180), 0);}
        else{            up = QVector3D(1.0*sin((360-Cam)*M_PI/180), 1.0*cos((360-Cam)*M_PI/180), 0);}

        m_viewMatrix.setToIdentity( );

        m_viewMatrix.lookAt(CameraFinal,CameraCenter,CameraUP);

        //定義光源座標系
        m_lightViewMatrix.setToIdentity( );
        m_lightViewMatrix.lookAt(m_rotatedLightPosition,CameraCenter,CameraUP);

        //定義模型座標系 :: 模型固定不旋轉不移動
        QVector3D NewModelPos;
        NewModelPos = QVector3D(-(Xmax-Xmin)/2,-(Ymax-Ymin+4)/2,-(Zmax-Zmin)/2);

        m_modelMatrices.setToIdentity();
        m_modelMatrices.translate(NewModelPos);

        //計算背景隨模型旋轉進行 Z-Shift
        double RotBackZshift;
        RotBackZshift = max(2.5*sin(Theta*M_PI/180),7*sin(Theta*M_PI/180)*sin(Phi*M_PI/180));
        double Cam_ObjCenter_Dis;
        Cam_ObjCenter_Dis = sqrt((Nx-TranX)*(Nx-TranX)+(Ny-TranY)*(Ny-TranY)+Nz*Nz);
        double ShiftBackZshift = 0;
        ShiftBackZshift = Cam_ObjCenter_Dis -100;

        //建立背景座標系 :: 背景根據相機位置調整位置及旋轉
        QVector3D NewBackGroundPos ;
        NewBackGroundPos = QVector3D(-320,-240,-20-ShiftBackZshift-RotBackZshift);

        m_backgroundMatrices.setToIdentity();
        m_backgroundMatrices.rotate(Phi,0.0,0.0,1.0);
        m_backgroundMatrices.rotate(Theta,0.0,1.0,0);
        m_backgroundMatrices.translate(NewBackGroundPos);

        back_viewMatrix.setToIdentity( );
        back_viewMatrix.lookAt(eye,center,up);
    }
}


void GLWidget::renderToFBO(void)
{
    m_FBO->bind( );
    glViewport( 0, 0, m_shadowMapWidth, m_shadowMapHeight );
    renderShadow();
    m_FBO->bindDefault( );
    glViewport( 0, 0, width( ), height( ) );
}

void GLWidget::renderShadow(void)
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glCullFace( GL_FRONT );

    m_depthProgram.bind( );
    m_depthProgram.setUniformValue( "modelMatrix", m_modelMatrices );
    m_depthProgram.setUniformValue( "viewMatrix" , m_lightViewMatrix );
    m_depthProgram.setUniformValue( "projectionMatrix", m_lightProjectionMatrix );
    CADmodel->renderShadow(this);
    CADmodel_back->renderShadow(this);
    m_depthProgram.release( );
    glCullFace( GL_BACK );
}

int GLWidget::shadowTexture()
{
    return m_FBO->texture();
}

void GLWidget::paintGL(void)
{

    if(isLoaded){
        /*
        if(ismask){
           SetViewANDLight();
           render();
        }*/
        if(iswireframe){
           SetViewANDLight();
           render();
        }
        else{
        SetViewANDLight();
        renderToFBO( );
        render();}
   }
   //update( );
}

void GLWidget::resizeGL(int w, int h)
{

    m_proj.setToIdentity();
    m_proj.perspective(45.0f, GLfloat(w)/h, 0.01f, 300.0f);


    // Model project matrix
    m_projectionMatrix.setToIdentity();
    m_projectionMatrix.perspective(45.0f, GLfloat(w)/h, 0.01f, 600.0f);

    // Light project matrix

    m_lightProjectionMatrix.setToIdentity();
    m_lightProjectionMatrix.perspective(90.0f, GLfloat(w)/GLfloat(h), 0.7f, 600.0f);

}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    m_lastPos = event->pos();
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    int dx = event->x() - m_lastPos.x();
    int dy = event->y() - m_lastPos.y();

    if (event->buttons() & Qt::LeftButton) {
        setTheta(Theta + 8 * dy);
        setPhi(Phi + 8 * dx);
    } else if (event->buttons() & Qt::RightButton) {
        setTheta(Theta + 8 * dy);
        setR(R + 8 * dx);
    }
    m_lastPos = event->pos();
}
