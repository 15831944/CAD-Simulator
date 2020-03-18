#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>
#include "glwidget.h"
#include <QSlider>
#include <QDir>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();  
     int w,h;
     int RandomShiftX,RandomShiftY;
     bool gray;
     bool IsRandomShift,IsRandomRot;
     QString suffix;
     QString savedir;
     QString pathname;
     QSlider *ThetaStart;
     void saveOne();
     void CalRandomLight();
     void CalRandomShift();
     void CalRandomRot();

private slots:
    void on_open_clicked();

    //void closeEvent( QCloseEvent * event);

    void on_Theta_valueChanged(int value);

    void on_Phi_valueChanged(int value);

    void on_R_valueChanged(int value);

    void on_pushButton_clicked();

    //void on_isBack_face_stateChanged(int arg1);

    //void on_isHide_Invisible_stateChanged(int arg1);

    void on_CameraAngle_valueChanged(int value);

    void on_ThetaRange_lowerValueChanged(int value);
    void on_ThetaRange_upperValueChanged(int value);

    void on_PhiRange_lowerValueChanged(int value);
    void on_PhiRange_upperValueChanged(int value);

    void on_RRange_lowerValueChanged(int value);
    void on_RRange_upperValueChanged(int value);

    void on_CamRange_lowerValueChanged(int value);
    void on_CamRange_upperValueChanged(int value);

    void on_multi_save_clicked();

    void on_Theta_Step_editingFinished();

    void on_Phi_Step_editingFinished();

    void on_R_Step_editingFinished();

    void on_Cam_Step_editingFinished();

    void on_ThetaNUM_textChanged(const QString &arg1);

    void on_save_dir_choose_clicked();

    void on_gray_stateChanged(int arg1);

    void on_TranXBar_valueChanged(int value);

    void on_TranYBar_valueChanged(int value);

    void on_isRandomLight_stateChanged(int arg1);

    void on_LightRange_thetaDown_editingFinished();

    void on_LightRange_thetaUp_editingFinished();

    void on_LightRange_phiDown_editingFinished();

    void on_LightRange_phiUp_editingFinished();

    void on_IsRandomShift_stateChanged(int arg1);

    void on_IsRandomRot_stateChanged(int arg1);

    void on_IsEuler_stateChanged(int arg1);

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
