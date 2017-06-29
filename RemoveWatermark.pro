#-------------------------------------------------
#
# Project created by QtCreator 2016-12-19T19:13:52
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = RemoveWatermark
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


SOURCES +=\
    ColorLabel.cpp \
    ThicknessSlider.cpp \
    OpenCVTool.cpp \
    ImageGraphicsview.cpp \
    MainWindow.cpp \
    Main.cpp \
    ImagePair.cpp

HEADERS  += \
    ColorLabel.h \
    ThicknessSlider.h \
    OpenCVTool.h \
    ImageGraphicsview.h \
    MainWindow.h \
    ImagePair.h

FORMS    += \
    ThicknessSlider.ui \
    MainWindow.ui

INCLUDEPATH += F:\opencv_QT\include\opencv\

INCLUDEPATH += F:\opencv_QT\include\opencv2\

INCLUDEPATH += F:\opencv_QT\include\

LIBS += -L F:\opencv_QT\lib\libopencv_*.a

RESOURCES += \   
    Resources.qrc