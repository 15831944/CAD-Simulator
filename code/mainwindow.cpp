#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include "opencv2/opencv.hpp"
#include <cstdlib>
#include <qstackedlayout.h>
#include <QDebug>
#include <QTime>
#include <QPixmap>

using namespace std;
using namespace cv;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    ui->setupUi(this);
    ui->progressBar->setMaximumHeight(5);
    ui->ThetaNUM->setText(QString::number(ui->Theta->value()));
    ui->PhiNUM->setText(QString::number(ui->Phi->value()));
    ui->RNUM->setText(QString::number(ui->R->value()));
    ui->CmNUM->setText(QString::number(ui->CameraAngle->value()));
    w = ui->openGLWidget->width();
    h = ui->openGLWidget->height();
    gray = ui->gray->isChecked();
    IsRandomRot = ui->IsRandomRot->isChecked();
    IsRandomShift = ui->IsRandomShift->isChecked();
    ui->openGLWidget->RandomLight = ui->isRandomLight->isChecked();
    ui->openGLWidget_2->RandomLight= ui->isRandomLight->isChecked();


    ui->ThetaRange->setUpperPosition(0);
    ui->ThetaRange->setLowerPosition(0);
    ui->ThetaS->setText(QString::number(ui->ThetaRange->lowerPosition()));
    ui->ThetaE->setText(QString::number(ui->ThetaRange->upperPosition()));

    ui->PhiRange->setUpperPosition(0);
    ui->PhiRange->setLowerPosition(0);
    ui->PhiS->setText(QString::number(ui->PhiRange->lowerPosition()));
    ui->PhiE->setText(QString::number(ui->PhiRange->upperPosition()));

    ui->RRange->setUpperPosition(150);
    ui->RRange->setLowerPosition(150);
    ui->RS->setText(QString::number(ui->RRange->lowerPosition()));
    ui->RE->setText(QString::number(ui->RRange->upperPosition()));

    ui->CamRange->setUpperPosition(0);
    ui->CamRange->setLowerPosition(0);
    ui->CamS->setText(QString::number(ui->CamRange->lowerPosition()));
    ui->CamE->setText(QString::number(ui->CamRange->upperPosition()));

    ui->openGLWidget->RandomLMaxTheta = ui->LightRange_thetaUp->text().toInt();
    ui->openGLWidget->RandomLMinTheta = ui->LightRange_thetaDown->text().toInt();
    ui->openGLWidget->RandomLMaxPhi   = ui->LightRange_phiUp->text().toInt();
    ui->openGLWidget->RandomLMinPhi   = ui->LightRange_phiDown->text().toInt();

    ui->openGLWidget_2->RandomLMaxTheta = ui->LightRange_thetaUp->text().toInt();
    ui->openGLWidget_2->RandomLMinTheta = ui->LightRange_thetaDown->text().toInt();
    ui->openGLWidget_2->RandomLMaxPhi   = ui->LightRange_phiUp->text().toInt();
    ui->openGLWidget_2->RandomLMinPhi   = ui->LightRange_phiDown->text().toInt();

    ui->isBack_face->hide();
    ui->isHide_Invisible->hide();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::saveOne()
{
    ui->openGLWidget->update();
    ui->openGLWidget_2->update();
    ui->openGLWidget_3->update();
    w = ui->openGLWidget->width();
    h = ui->openGLWidget->height();
    suffix = ui->SaveSuffix->text();

    //QString name = pathname+"/train_data/"+
    //Build model folder
    if(!(QDir(savedir+"\\model").exists())){cout << "creat model folder." <<endl;QDir().mkdir(savedir+"\\model");}
    //Build wireframe folder
    if(!(QDir(savedir+"\\wireframe").exists())){cout << "creat wireframe folder." <<endl;QDir().mkdir(savedir+"\\wireframe");}
    //Build mask folder
    if(!(QDir(savedir+"\\mask").exists())){cout << "creat mask folder." <<endl;QDir().mkdir(savedir+"\\mask");}

    //Build angle folder in model
    if(QDir(savedir+"\\model\\"+QString::number(ui->openGLWidget->Theta)).exists())
      {}
    else
      {QDir().mkdir(savedir+"\\model\\"+QString::number(ui->openGLWidget->Theta));}

    //Build angle folder in wireframe
    if(QDir(savedir+"\\wireframe\\"+QString::number(ui->openGLWidget->Theta)).exists())
      {}
    else
      {QDir().mkdir(savedir+"\\wireframe\\"+QString::number(ui->openGLWidget->Theta));}

    //Build angle folder in mask
    if(QDir(savedir+"\\mask\\"+QString::number(ui->openGLWidget->Theta)).exists())
      {}
    else
      {QDir().mkdir(savedir+"\\mask\\"+QString::number(ui->openGLWidget->Theta));}

    //嘗試處理左上右下點
    QImage WirePixmap = ui->openGLWidget_2->grab().toImage();
    Mat WireMat,WireMat_gray;
    WireMat =Mat(WirePixmap.height(), WirePixmap.width(), CV_8UC4, (void*)WirePixmap.constBits(), WirePixmap.bytesPerLine());
    cvtColor(WireMat, WireMat_gray, cv::COLOR_BGR2GRAY);
    int WireHeight = WireMat_gray.rows;
    int WireWidth  = WireMat_gray.cols;
    int LPx=0,LPy=0,RDx=0,RDy=0;
    //qDebug() << "WireHeight : " << WireHeight << "WireWidth : " << WireWidth;


    bool sign = true;

    //Find Left Up X pos
    for(int i =0;i<WireWidth;i++){
        if(sign){for(int j=0;j<WireHeight;j++){
                     if (WireMat_gray.at<uchar>(j, i) != 0){LPx = i;sign = false;break;}}}
        else    {break;}}
    sign = true;

    //Find Left Up Y pos
    for(int i =0;i<WireHeight;i++){
        if(sign){for(int j=0;j<WireWidth;j++){
                     if (WireMat_gray.at<uchar>(i, j) != 0){LPy = i;sign = false;break;}}}
        else    {break;}}
    sign = true;

    //Find Right Down X pos
    for(int i =0;i<WireWidth;i++){
        if(sign){for(int j=0;j<WireHeight;j++){
                     if (WireMat_gray.at<uchar>(WireHeight-j-1,WireWidth-i-1) != 0){RDx = WireWidth-i;sign = false;break;}}}
        else    {break;}}
    sign = true;

    //Find Right Down Y pos
    for(int i =0;i<WireHeight;i++){
        if(sign){for(int j=0;j<WireWidth;j++){
                     if (WireMat_gray.at<uchar>(WireHeight-i-1, WireWidth-j-1) != 0){RDy = WireHeight-i;sign = false;break;}}}
        else    {break;}}
    sign = true;

    //Save model image
    QString model_name = savedir+"\\model\\"+QString::number(ui->openGLWidget->Theta)+"\\"+
    QString::number((int)ui->openGLWidget->Theta)+"_"+
    QString::number((int)ui->openGLWidget->Phi)+"_"+
    QString::number((int)ui->openGLWidget->R);
    if(IsRandomRot){model_name = model_name+"_"+QString::number((int)ui->openGLWidget->RandomRot);}
    else{model_name = model_name+"_"+QString::number((int)ui->openGLWidget->Cam);}
    model_name = model_name+"_"+QString::number(LPx)+"_"+
                                QString::number(LPy)+"_"+
                                QString::number(RDx)+"_"+
                                QString::number(RDy)+"_"+suffix+".jpg";

    ui->openGLWidget->grab().save(model_name);

    //Save wireframe image
    QString wireframe_name = savedir+"\\wireframe\\"+QString::number(ui->openGLWidget->Theta)+"\\"+
    QString::number((int)ui->openGLWidget->Theta)+"_"+
    QString::number((int)ui->openGLWidget->Phi)+"_"+
    QString::number((int)ui->openGLWidget->R);
    if(IsRandomRot){wireframe_name = wireframe_name+"_"+QString::number((int)ui->openGLWidget->RandomRot);}
    else{           wireframe_name = wireframe_name+"_"+QString::number((int)ui->openGLWidget->Cam);}
    wireframe_name = wireframe_name+"_"+QString::number(LPx)+"_"+
                                        QString::number(LPy)+"_"+
                                        QString::number(RDx)+"_"+
                                        QString::number(RDy)+"_"+suffix+".jpg";

    ui->openGLWidget_2->grab().save(wireframe_name);

    //Save mask image
    QString mask_name = savedir+"\\mask\\"+QString::number(ui->openGLWidget->Theta)+"\\"+
    QString::number((int)ui->openGLWidget->Theta)+"_"+
    QString::number((int)ui->openGLWidget->Phi)+"_"+
    QString::number((int)ui->openGLWidget->R);
    if(IsRandomRot){mask_name = mask_name+"_"+QString::number((int)ui->openGLWidget->RandomRot);}
    else{           mask_name = mask_name+"_"+QString::number((int)ui->openGLWidget->Cam);}
    mask_name = mask_name+"_"+QString::number(LPx)+"_"+
                              QString::number(LPy)+"_"+
                              QString::number(RDx)+"_"+
                              QString::number(RDy)+"_"+suffix+".jpg";

    ui->openGLWidget_3->grab().save(mask_name);
}

void MainWindow::CalRandomLight()
{
    if(ui->isRandomLight->isChecked()){
       int RandomLMaxTheta = ui->LightRange_thetaUp  ->text().toInt();
       int RandomLMinTheta = ui->LightRange_thetaDown->text().toInt();
       int RandomLMaxPhi   = ui->LightRange_phiUp    ->text().toInt();
       int RandomLMinPhi   = ui->LightRange_phiDown  ->text().toInt();

       int Random_theta = (qrand() % ((RandomLMaxTheta+1)-RandomLMinTheta)+RandomLMinTheta);
       int Random_phi   = (qrand() % ((RandomLMaxPhi+1)-RandomLMinPhi)+RandomLMinPhi);

       double RNx,RNy,RNz;
       if(Random_phi == 90 || Random_phi ==270){RNx = 0;}
       else{RNx = ui->R->value() * sin(Random_theta*M_PI/180) * cos(Random_phi*M_PI/180);}
       RNy = ui->R->value() * sin(Random_theta*M_PI/180) * sin(Random_phi*M_PI/180);
       if(Random_theta == 90|| Random_theta ==270)   {RNz =0;}
       else{RNz = ui->R->value() * cos(Random_theta*M_PI/180);}

       QVector3D light = QVector3D( RNx,RNy,RNz);
       ui->openGLWidget->m_rotatedLightPosition = light;
      }
}

void MainWindow::CalRandomShift()
{
    int baseX = 40;
    int baseY = 25;
    int baseR = 100;
    int RandomRangeX = 0;
    int RandomRangeY = 0;
    int R = (int)ui->openGLWidget->R;

    RandomRangeX = fabs(baseX+0.5*(R-baseR));
    RandomRangeY = fabs(baseY+0.4*(R-baseR));

    //qrand() % ((max + 1) - min) + min;
    RandomShiftX = qrand() % ((RandomRangeX+1)+RandomRangeX)-RandomRangeX;
    RandomShiftY = qrand() % ((RandomRangeY+1)+RandomRangeY)-RandomRangeY;

    ui->openGLWidget->RandomShiftX = RandomShiftX;
    ui->openGLWidget->RandomShiftY = RandomShiftY;
    ui->openGLWidget_2->RandomShiftX = RandomShiftX;
    ui->openGLWidget_2->RandomShiftY = RandomShiftY;
    ui->openGLWidget_3->RandomShiftX = RandomShiftX;
    ui->openGLWidget_3->RandomShiftY = RandomShiftY;
}

void MainWindow::CalRandomRot()
{
    int RandomRot = 0;
    RandomRot = qrand() % 360;
    ui->openGLWidget->RandomRot = RandomRot;
    ui->openGLWidget_2->RandomRot = RandomRot;
    ui->openGLWidget_3->RandomRot = RandomRot;
}

void MainWindow::on_open_clicked()
{
    QString Filename;

    Filename = QFileDialog::getOpenFileName(this,"Open my file");
    if(!Filename.isEmpty())
    {
        ui->openGLWidget->createModel(Filename);
        ui->filename->setText(Filename);

        ui->openGLWidget_2->createWireframe(Filename,0);
        ui->openGLWidget_3->createWireframe(Filename,1);
    }
}


void MainWindow::on_Theta_valueChanged(int value)
{
    ui->ThetaNUM->setText(QString::number(value));

    CalRandomLight();
    CalRandomShift();
    CalRandomRot();

    ui->openGLWidget->Theta = value;
    ui->openGLWidget_2->Theta = value;
    ui->openGLWidget_3->Theta = value;
    ui->openGLWidget->SetViewANDLight();
    ui->openGLWidget_2->SetViewANDLight();
    ui->openGLWidget_3->SetViewANDLight();
    ui->openGLWidget->update();
    ui->openGLWidget_2->update();
    ui->openGLWidget_3->update();
}

void MainWindow::on_Phi_valueChanged(int value)
{
    //ui->PhiNUM->setText(QString::number(ui->Phi->value()));
    ui->PhiNUM->setText(QString::number(value));

    CalRandomLight();
    CalRandomShift();
    CalRandomRot();

    ui->openGLWidget->Phi = value;
    ui->openGLWidget_2->Phi = value;
    ui->openGLWidget_3->Phi = value;
    ui->openGLWidget->SetViewANDLight();
    ui->openGLWidget_2->SetViewANDLight();
    ui->openGLWidget_3->SetViewANDLight();
    ui->openGLWidget->update();
    ui->openGLWidget_2->update();
    ui->openGLWidget_3->update();
}

void MainWindow::on_R_valueChanged(int value)
{
    //ui->RNUM->setText(QString::number(ui->R->value()));
    ui->RNUM->setText(QString::number(value));

    CalRandomLight();
    CalRandomShift();
    CalRandomRot();

    ui->openGLWidget->R = value;
    ui->openGLWidget_2->R = value;
    ui->openGLWidget_3->R = value;
    ui->openGLWidget->SetViewANDLight();
    ui->openGLWidget_2->SetViewANDLight();
    ui->openGLWidget_3->SetViewANDLight();
    ui->openGLWidget->update();
    ui->openGLWidget_2->update();
    ui->openGLWidget_3->update();
}

void MainWindow::on_CameraAngle_valueChanged(int value)
{
    //ui->CmNUM->setText(QString::number(ui->CameraAngle->value()));
    ui->CmNUM->setText(QString::number(value));

    CalRandomLight();
    CalRandomShift();
    CalRandomRot();

    ui->openGLWidget->Cam = value;
    ui->openGLWidget_2->Cam = value;
    ui->openGLWidget_3->Cam = value;

    ui->openGLWidget->SetViewANDLight();
    ui->openGLWidget_2->SetViewANDLight();
    ui->openGLWidget_3->SetViewANDLight();

    ui->openGLWidget->update();
    ui->openGLWidget_2->update();
    ui->openGLWidget_3->update();
}


void MainWindow::on_pushButton_clicked()
{
    saveOne();
}

/*
void MainWindow::on_isBack_face_stateChanged(int arg1)
{
    if(ui->isBack_face->checkState())
    {ui->openGLWidget->isCulling = true;}
    else
    {ui->openGLWidget->isCulling = false;}
    //ui->openGLWidget->paintGL();
    ui->openGLWidget->update();
}
*/

/*
void MainWindow::on_isHide_Invisible_stateChanged(int arg1)
{
    if(ui->isHide_Invisible->checkState())
    {ui->openGLWidget->isHiding = true;}
    else
    {ui->openGLWidget->isHiding = false;}
    ui->openGLWidget->Normal.clear();
    //ui->openGLWidget->LoadModel();
    //ui->openGLWidget->paintGL();
    ui->openGLWidget->update();
}
*/

void MainWindow::on_ThetaRange_lowerValueChanged(int value)
{
    ui->ThetaS->setText(QString::number(value));
    ui->openGLWidget  ->ThetaS = (GLfloat)value;
    ui->openGLWidget_2->ThetaS = (GLfloat)value;
    ui->openGLWidget_3->ThetaS = (GLfloat)value;
}

void MainWindow::on_ThetaRange_upperValueChanged(int value)
{
    ui->ThetaE->setText(QString::number(value));
    ui->openGLWidget  ->ThetaE = (GLfloat)value;
    ui->openGLWidget_2->ThetaE = (GLfloat)value;
    ui->openGLWidget_3->ThetaE = (GLfloat)value;
}

void MainWindow::on_PhiRange_lowerValueChanged(int value)
{
    ui->PhiS->setText(QString::number(value));
    ui->openGLWidget  ->PhiS = (GLfloat)value;
    ui->openGLWidget_2->PhiS = (GLfloat)value;
    ui->openGLWidget_3->PhiS = (GLfloat)value;
}

void MainWindow::on_PhiRange_upperValueChanged(int value)
{
    ui->PhiE->setText(QString::number(value));
    ui->openGLWidget  ->PhiE = (GLfloat)value;
    ui->openGLWidget_2->PhiE = (GLfloat)value;
    ui->openGLWidget_3->PhiE = (GLfloat)value;
}

void MainWindow::on_RRange_lowerValueChanged(int value)
{
    ui->RS->setText(QString::number(value));
    ui->openGLWidget  ->RS = (GLfloat)value;
    ui->openGLWidget_2->RS = (GLfloat)value;
    ui->openGLWidget_3->RS = (GLfloat)value;
}

void MainWindow::on_RRange_upperValueChanged(int value)
{
    ui->RE->setText(QString::number(value));
    ui->openGLWidget  ->RE = (GLfloat)value;
    ui->openGLWidget_2->RE = (GLfloat)value;
    ui->openGLWidget_3->RE = (GLfloat)value;
}

void MainWindow::on_CamRange_lowerValueChanged(int value)
{
    ui->CamS->setText(QString::number(value));
    ui->openGLWidget  ->CamS = (GLfloat)value;
    ui->openGLWidget_2->CamS = (GLfloat)value;
    ui->openGLWidget_3->CamS = (GLfloat)value;
}

void MainWindow::on_CamRange_upperValueChanged(int value)
{
    ui->CamE->setText(QString::number(value));
    ui->openGLWidget  ->CamE = (GLfloat)value;
    ui->openGLWidget_2->CamE = (GLfloat)value;
    ui->openGLWidget_3->CamE = (GLfloat)value;
}


void MainWindow::on_multi_save_clicked()
{
    cout << "Theta : " << ui->openGLWidget->Theta <<"  ";
    cout << "Range : " << ui->openGLWidget->ThetaS << " ~ " << ui->openGLWidget->ThetaE << endl;

    cout << "Phi : " << ui->openGLWidget->Phi <<"  ";
    cout << "Range : " << ui->openGLWidget->PhiS << " ~ " << ui->openGLWidget->PhiE << endl;

    cout << "R : " << ui->openGLWidget->R <<"  ";
    cout << "Range : " << ui->openGLWidget->RS << " ~ " << ui->openGLWidget->RE << endl;

    cout << "Camera : " << ui->openGLWidget->Cam <<"  ";
    cout << "Range : " << ui->openGLWidget->CamS << " ~ " << ui->openGLWidget->CamE << endl;

    cout << "Step : " ;
    cout << ui->openGLWidget->Theta_Step << " ";
    cout << ui->openGLWidget->Phi_Step << " ";
    cout << ui->openGLWidget->R_Step << " ";
    cout << ui->openGLWidget->Cam_Step <<endl;

    GLfloat _ThetaS = ui->openGLWidget->ThetaS;
    GLfloat _ThetaE = ui->openGLWidget->ThetaE;
    int ThetaCount ;
    if(_ThetaE-_ThetaS ==0)
      {ThetaCount = 1;}
    else
      {ThetaCount = (_ThetaE-_ThetaS)/ui->openGLWidget->Theta_Step+1 ;}
    ui->openGLWidget->Theta = _ThetaS;
    ui->openGLWidget_2->Theta = _ThetaS;
    ui->openGLWidget_3->Theta = _ThetaS;

    GLfloat _PhiS = ui->openGLWidget->PhiS;
    GLfloat _PhiE = ui->openGLWidget->PhiE;
    int PhiCount ;
    if(_PhiE-_PhiS ==0)
      {PhiCount = 1;}
    else
      {PhiCount = (_PhiE-_PhiS)/ui->openGLWidget->Phi_Step+1 ;}
    ui->openGLWidget->Phi = _PhiS;
    ui->openGLWidget_2->Phi = _PhiS;
    ui->openGLWidget_3->Phi = _PhiS;

    GLfloat _rS = ui->openGLWidget->RS;
    GLfloat _rE = ui->openGLWidget->RE;
    int RCount;
    if(_rE-_rS==0)
      {RCount = 1;}
    else
      {RCount = (_rE-_rS)/ui->openGLWidget->R_Step+1 ;}
    ui->openGLWidget->R = _rS;
    ui->openGLWidget_2->R = _rS;
    ui->openGLWidget_3->R = _rS;

    GLfloat _CamS = ui->openGLWidget->CamS;
    GLfloat _CamE = ui->openGLWidget->CamE;
    int CamCount;
    if(_CamE-_CamS ==0)
      {CamCount = 1;}
    else
      {CamCount =  (_CamE-_CamS)/ui->openGLWidget->Cam_Step+1;}
    ui->openGLWidget->Cam = _CamS;
    ui->openGLWidget_2->Cam = _CamS;
    ui->openGLWidget_3->Cam = _CamS;


    int total = ThetaCount*PhiCount*RCount*CamCount;
    cout << "Total save : " << ThetaCount << "*"<< PhiCount << "*" << RCount <<"*" << CamCount << "="<< total << endl;
    int Nowcount = 1;
    ui->openGLWidget->update();
    ui->openGLWidget_2->update();
    ui->openGLWidget_3->update();
    QTime t;
    t.start();
    while(t.elapsed()<10)
        QCoreApplication::processEvents();


    for(int i = 0 ;i<ThetaCount;i++)
       {
        ui->openGLWidget->Theta = _ThetaS+ui->openGLWidget->Theta_Step*i;
        ui->openGLWidget_2->Theta = _ThetaS+ui->openGLWidget->Theta_Step*i;
        ui->openGLWidget_3->Theta = _ThetaS+ui->openGLWidget->Theta_Step*i;
        for(int j =0;j<PhiCount;j++)
           {
            ui->openGLWidget->Phi = _PhiS+ui->openGLWidget->Phi_Step*j;
            ui->openGLWidget_2->Phi = _PhiS+ui->openGLWidget->Phi_Step*j;
            ui->openGLWidget_3->Phi = _PhiS+ui->openGLWidget->Phi_Step*j;
            for(int k=0;k<RCount;k++)
               {
                ui->openGLWidget->R = _rS+ui->openGLWidget->R_Step*k;
                ui->openGLWidget_2->R = _rS+ui->openGLWidget->R_Step*k;
                ui->openGLWidget_3->R = _rS+ui->openGLWidget->R_Step*k;
                for(int l=0;l<CamCount;l++)
                   {
                    ui->openGLWidget->Cam = _CamS+ui->openGLWidget->Cam_Step*l;
                    ui->openGLWidget_2->Cam = _CamS+ui->openGLWidget->Cam_Step*l;
                    ui->openGLWidget_3->Cam = _CamS+ui->openGLWidget->Cam_Step*l;
                    ui->progressBar->setValue(Nowcount*100/total);

                    CalRandomLight();
                    CalRandomShift();
                    CalRandomRot();
                    /*
                    qDebug() << "Theta : " << ui->openGLWidget->Theta
                             << "Phi : " << ui->openGLWidget->Phi
                             << "R : " << ui->openGLWidget->R
                             << "Cam : " << ui->openGLWidget->Cam;*/

                    ui->openGLWidget->SetViewANDLight();
                    ui->openGLWidget_2->SetViewANDLight();
                    ui->openGLWidget_3->SetViewANDLight();

                    ui->openGLWidget->update();
                    ui->openGLWidget_2->update();
                    ui->openGLWidget_3->update();
                    QTime s;
                    s.start();
                    while(s.elapsed()<10)
                        QCoreApplication::processEvents();
                    saveOne();
                    Nowcount++;
                   }
               }
           }

       }
    cout << "Save complete..." <<endl;
}

void MainWindow::on_Theta_Step_editingFinished()
{
     ui->openGLWidget->Theta_Step = ui->Theta_Step->text().toInt();
}

void MainWindow::on_Phi_Step_editingFinished()
{
    ui->openGLWidget->Phi_Step = ui->Phi_Step->text().toInt();
}

void MainWindow::on_R_Step_editingFinished()
{
    ui->openGLWidget->R_Step = ui->R_Step->text().toInt();
}

void MainWindow::on_Cam_Step_editingFinished()
{
    ui->openGLWidget->Cam_Step = ui->Cam_Step->text().toInt();
}

void MainWindow::on_ThetaNUM_textChanged(const QString &arg1)
{
    //ui->Theta->setValue(ui->ThetaNUM->text().toInt());
    ui->Theta->setValue(arg1.toInt());
}

void MainWindow::on_save_dir_choose_clicked()
{
    savedir = QFileDialog::getExistingDirectory(this,"Model save dir");

    ui->save_dir->setText(savedir);
}

void MainWindow::on_gray_stateChanged(int arg1)
{
    if(arg1 ==0){gray = false;}
    else{gray = true;}
    //gray = ui->gray->isChecked();
}

void MainWindow::on_TranXBar_valueChanged(int value)
{
    ui->TranXText->setText(QString::number(value));

    CalRandomLight();
    CalRandomShift();
    CalRandomRot();

    ui->openGLWidget->TranX = value;
    ui->openGLWidget_2->TranX =value;
    ui->openGLWidget_3->TranX =value;
    ui->openGLWidget->SetViewANDLight();
    ui->openGLWidget_2->SetViewANDLight();
    ui->openGLWidget_3->SetViewANDLight();

    ui->openGLWidget->update();
    ui->openGLWidget_2->update();
    ui->openGLWidget_3->update();
}



void MainWindow::on_TranYBar_valueChanged(int value)
{
    ui->TranYText->setText(QString::number(value));

    CalRandomLight();
    CalRandomShift();
    CalRandomRot();

    ui->openGLWidget->TranY = value;
    ui->openGLWidget_2->TranY =value;
    ui->openGLWidget_3->TranY =value;
    ui->openGLWidget->SetViewANDLight();
    ui->openGLWidget_2->SetViewANDLight();
    ui->openGLWidget_3->SetViewANDLight();

    ui->openGLWidget->update();
    ui->openGLWidget_2->update();
    ui->openGLWidget_3->update();
}

void MainWindow::on_isRandomLight_stateChanged(int arg1)
{
    if(arg1 ==0){ui->openGLWidget->RandomLight = false;
                 ui->openGLWidget_2->RandomLight= false;
                 ui->openGLWidget_3->RandomLight= false;}
    else{ui->openGLWidget->RandomLight = true;
         ui->openGLWidget_2->RandomLight= true;
         ui->openGLWidget_3->RandomLight= true;}
}

void MainWindow::on_LightRange_thetaDown_editingFinished()
{
    ui->openGLWidget->RandomLMinTheta = ui->LightRange_thetaDown->text().toInt();
    ui->openGLWidget_2->RandomLMinTheta = ui->LightRange_thetaDown->text().toInt();
    ui->openGLWidget_3->RandomLMinTheta = ui->LightRange_thetaDown->text().toInt();
}

void MainWindow::on_LightRange_thetaUp_editingFinished()
{
    ui->openGLWidget->RandomLMaxTheta = ui->LightRange_thetaUp->text().toInt();
    ui->openGLWidget_2->RandomLMaxTheta = ui->LightRange_thetaUp->text().toInt();
    ui->openGLWidget_3->RandomLMaxTheta = ui->LightRange_thetaUp->text().toInt();
}

void MainWindow::on_LightRange_phiDown_editingFinished()
{
    ui->openGLWidget->RandomLMinPhi = ui->LightRange_phiDown->text().toInt();
    ui->openGLWidget_2->RandomLMinPhi = ui->LightRange_phiDown->text().toInt();
    ui->openGLWidget_3->RandomLMinPhi = ui->LightRange_phiDown->text().toInt();
}

void MainWindow::on_LightRange_phiUp_editingFinished()
{
    ui->openGLWidget->RandomLMaxPhi = ui->LightRange_phiUp->text().toInt();
    ui->openGLWidget_2->RandomLMaxPhi = ui->LightRange_phiUp->text().toInt();
    ui->openGLWidget_3->RandomLMaxPhi = ui->LightRange_phiUp->text().toInt();
}

void MainWindow::on_IsRandomShift_stateChanged(int arg1)
{
    if(arg1 ==0){IsRandomShift = false;
                 ui->openGLWidget->RandomShift = false;
                 ui->openGLWidget_2->RandomShift= false;
                 ui->openGLWidget_3->RandomShift= false;}
    else{IsRandomShift = true;
         ui->openGLWidget->RandomShift = true;
         ui->openGLWidget_2->RandomShift= true;
         ui->openGLWidget_3->RandomShift= true;}
}

void MainWindow::on_IsRandomRot_stateChanged(int arg1)
{
    if(arg1 ==0){IsRandomRot = false;
                 ui->openGLWidget->RandomCamRot = false;
                 ui->openGLWidget_2->RandomCamRot= false;
                 ui->openGLWidget_3->RandomCamRot= false;}
    else{IsRandomRot = true;
         ui->openGLWidget->RandomCamRot = true;
         ui->openGLWidget_2->RandomCamRot= true;
         ui->openGLWidget_3->RandomCamRot= true;}

}

void MainWindow::on_IsEuler_stateChanged(int arg1)
{
    if(arg1 ==0){ui->openGLWidget->IsEuler = true;
                 ui->openGLWidget_2->IsEuler= true;
                 ui->openGLWidget_3->IsEuler= true;}
    else{ui->openGLWidget->IsEuler = false;
         ui->openGLWidget_2->IsEuler= false;
         ui->openGLWidget_3->IsEuler= false;}
}
