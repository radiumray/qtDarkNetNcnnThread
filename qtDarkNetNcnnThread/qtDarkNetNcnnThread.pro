QT += gui
QT += core
QT += widgets

CONFIG += c++11
CONFIG -= app_bundle

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        main.cpp \
        mainwindow.cpp \
        mythread.cpp

usrLocInc = /usr/local/include
usrLocLib = /usr/local/lib
NCNN_DIR = /home/ray/ncnn
DARKNET_DIR = /home/ray/darknet2ncnn


INCLUDEPATH += $${usrLocInc} \
    $${usrLocInc}/opencv4 \
    $${NCNN_DIR}/build/install/include/ncnn \
    $${DARKNET_DIR}/include \
    $${NCNN_DIR}/src


LIBS += $${usrLocLib}/libopencv_highgui.so \
    $${usrLocLib}/libopencv_core.so \
    $${usrLocLib}/libopencv_imgproc.so \
    $${usrLocLib}/libopencv_imgcodecs.so \
    $${usrLocLib}/libopencv_video.so \
    $${usrLocLib}/libopencv_videoio.so \
    $${usrLocLib}/libopencv_calib3d.so \
    $${usrLocLib}/libopencv_features2d.so \
    $${usrLocLib}/libopencv_flann.so \
    $${usrLocLib}/libopencv_objdetect.so \
    $${usrLocLib}/libopencv_photo.so \
    $${usrLocLib}/libopencv_stitching.so \
    $${NCNN_DIR}/build/install/lib/libncnn.a \
    $${DARKNET_DIR}/libdarknet2ncnn.a


QMAKE_CXXFLAGS += -fopenmp
LIBS += -fopenmp


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

FORMS += \
    mainwindow.ui

HEADERS += \
    mainwindow.h \
    mythread.h \
    portablesleep.h
