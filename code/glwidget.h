#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QMatrix4x4>
#include "cad_model.h"
#include "cad_wireframe.h"
#include "cad_background.h"
#include "cad_mask.h"
#include <QOpenGLTexture>


QT_FORWARD_DECLARE_CLASS(QOpenGLShaderProgram)

QT_BEGIN_NAMESPACE
class QOpenGLFramebufferObject;
QT_END_NAMESPACE

class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    GLWidget(QWidget *parent = 0);
    ~GLWidget();
    void Add_DShader_Link();
    void generateShadowFBO( void );
    void createModel(const QString& fileName);
    void createWireframe(const QString &fileName,int mode);
    void createMask(const QString &fileName);
    void SetViewANDLight(void);
    void render(void);

    static void setTransparent(bool t) { m_transparent = t; }
    int  shadowTexture(void);
    static bool isTransparent() { return m_transparent; }

    inline QMatrix4x4& projectionMatrix( void ) { return m_projectionMatrix; }
    inline QMatrix4x4& lightProjectionMatrix( void ) { return m_lightProjectionMatrix; }
    inline QOpenGLShaderProgram& depthProgram( void ) { return m_depthProgram; }



    QSize minimumSizeHint() const override;
    QSize sizeHint() const override;

    CAD_model*           CADmodel;
    CAD_background* CADmodel_back;
    CAD_wireframe*   CADwireframe;
    CAD_mask*        CADmask;

    int Theta,ThetaS,ThetaE,Theta_Step;
    int Phi,PhiS,PhiE,Phi_Step;
    int R,RS,RE,R_Step;
    int TranX,TranY;
    int Cam,CamS,CamE,Cam_Step;
    int RandomLMaxTheta,RandomLMinTheta,RandomLMaxPhi,RandomLMinPhi;
    int RandomShiftX,RandomShiftY;
    int RandomRot;
    double ShiftX,ShiftY,ShiftZ;
    bool isLoaded;
    bool iswireframe,ismask;
    bool RandomLight;
    bool RandomShift;
    bool RandomCamRot;
    bool IsEuler;
    GLfloat Xmax,Xmin,Ymax,Ymin,Zmax,Zmin;
    int LPx,LPy,RDx,RDy;
    QOpenGLShaderProgram m_depthProgram;
    QOpenGLFramebufferObject* m_FBO;
    QOpenGLFramebufferObject* m_FBO2;
    QOpenGLBuffer m_Vbo;
    QVector3D m_rotatedLightPosition;


public slots:
    void setTheta(int theta);
    void setPhi(int phi);
    void setR(int r);
    void setRotate(int rotate);
    void setTranX(int tranX);
    void setTranY(int tranY);
    void cleanup();

signals:
    void ThetaChanged(int theta);
    void PhiChanged(int phi);
    void RChanged(int r);
    void RotateChange(int rotate);
    void TranslateX(int tranX);
    void TranslateY(int tranY);


protected:
    void initializeGL(void) override;
    void renderToFBO(void);
    void renderShadow(void);
    void paintGL(void) override;
    void resizeGL(int width, int height) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    void printVersionInformation();
    void CalRandomShift();

    int m_projMatrixLoc,w_projMatrixLoc,s_projMatrixLoc;
    int m_modelMatrixLoc,w_modelMatrixLoc,s_modelMatrixLoc;
    int m_viewMatrixLoc,w_viewMatrixLoc,s_viewMatrixLoc;
    int m_normalMatrixLoc,w_normalMatrixLoc;
    int m_lightViewMatrixLoc,m_lightProjectionMatrixLoc,m_lightPosLoc;
    int m_texCoordAttr,m_texUniform;
    int m_color;
    int m_shadowLoc;
    int UsingTexture,IsWireframe;
    int m_shadowMapWidth, m_shadowMapHeight;


    static bool m_transparent;
    GLuint depthMapFBO;

    QPoint m_lastPos;
    QOpenGLBuffer back_Vbo;
    QOpenGLBuffer Wireframe_Vbo;

    QMatrix4x4 m_proj;
    QMatrix4x4 m_camera;
    QMatrix4x4 b_camera;
    QMatrix4x4 w_modelMatrix;
    QMatrix4x4 m_backgroundMatrix;
    QMatrix4x4 m_viewMatrix,back_viewMatrix,m_projectionMatrix;
    QMatrix4x4 m_lightViewMatrix, m_lightProjectionMatrix;
    QMatrix4x4 m_modelMatrices,m_backgroundMatrices;

    QVector3D eye;
    QVector3D center;
    QVector3D up;



};

#endif
