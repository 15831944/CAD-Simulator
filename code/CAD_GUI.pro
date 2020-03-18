#-------------------------------------------------
#
# Project created by QtCreator 2017-05-17T11:01:26
#
#-------------------------------------------------

QT       += core gui
QT       += opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CAD_GUI
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
        mainwindow.cpp \
        dl_dxf.cpp \
        dl_writer_ascii.cpp \
        test_creationclass.cpp \
    qxtspanslider.cpp \
    model.cpp \
    glwidget.cpp \
    background.cpp \
    wireframe.cpp \
    cad_model.cpp \
    cad_background.cpp \
    cad_wireframe.cpp \
    cad_mask.cpp

HEADERS  += mainwindow.h \
         dl_attributes.h \
         dl_codes.h \
         dl_creationadapter.h \
         dl_creationinterface.h \
         dl_dxf.h \
         dl_entities.h \
         dl_exception.h \
         dl_extrusion.h \
         dl_global.h \
         dl_writer.h \
         dl_writer_ascii.h \
         test_creationclass.h \
         block_obj.h \
         pointdata.h \
    qxtspanslider.h \
    qxtspanslider_p.h \
    model.h \
    glwidget.h \
    background.h \
    wireframe.h \
    cad_model.h \
    cad_background.h \
    cad_wireframe.h \
    cad_mask.h

FORMS    += mainwindow.ui

INCLUDEPATH+=C:\OpenCV3.0\include\opencv\
C:\OpenCV3.0\include\opencv2\
C:\OpenCV3.0\include


LIBS += -lOpengl32 \
        -lglut \
        -lglut32\
        -lglu32\
        C:\OpenCV3.0\lib\libopencv_calib3d320.dll.a\
        C:\OpenCV3.0\lib\libopencv_core320.dll.a\
        C:\OpenCV3.0\lib\libopencv_features2d320.dll.a\
        C:\OpenCV3.0\lib\libopencv_flann320.dll.a\
        C:\OpenCV3.0\lib\libopencv_highgui320.dll.a\
        C:\OpenCV3.0\lib\libopencv_imgcodecs320.dll.a\
        C:\OpenCV3.0\lib\libopencv_imgproc320.dll.a\
        C:\OpenCV3.0\lib\libopencv_ml320.dll.a\
        C:\OpenCV3.0\lib\libopencv_objdetect320.dll.a\
        C:\OpenCV3.0\lib\libopencv_photo320.dll.a\
        C:\OpenCV3.0\lib\libopencv_shape320.dll.a\
        C:\OpenCV3.0\lib\libopencv_stitching320.dll.a\
        C:\OpenCV3.0\lib\libopencv_superres320.dll.a\
        C:\OpenCV3.0\lib\libopencv_video320.dll.a\
        C:\OpenCV3.0\lib\libopencv_videoio320.dll.a\
        C:\OpenCV3.0\lib\libopencv_videostab320.dll.a

DISTFILES += \
    icon.rc

RC_FILE += \
    icon.rc

RESOURCES += \
    Textures.qrc \
    Shaders.qrc
