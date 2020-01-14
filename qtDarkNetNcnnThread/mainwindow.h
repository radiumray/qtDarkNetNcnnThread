#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QLabel>
#include <QWidget>
#include <QImage>
#include <QPixmap>
#include <QDebug>
#include <QTimer>

#include <fstream>
#include <algorithm>
#include <vector>

#include <QPainter>
#include <iostream>
#include <stdio.h>

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
//#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui_c.h>


#include "mythread.h"

#include "darknet2ncnn.h"
#include "layer/input.h"
#include "ncnn_tools.h"
#include "benchmark.h"

using namespace cv;
using namespace std;


#define TIME_OUT 30 //视频播放间隔时间
#define FPS 30 //播放频率


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void dealSignal();
    void dealClose();

signals:
    void startThread();

private slots:

    void nextFrame();
    void on_pushButtonStart_clicked();
    void on_pushButtonStop_clicked();
    void on_progressBar_valueChanged(int value);

//    void on_pushButtonPro_clicked();

    void updateCount(int cnt);
    void startCount();
    void countFinished();


//    void on_pushButtonProStop_clicked();

private:

    Ui::MainWindow *ui;
    VideoCapture capture;
    Mat frame;
    QImage image;
    QTimer *timer;
    QImage Mat2QImage(Mat cvImg);

    void connectSignalsSlots();
    bool countRunning;

    QThread     *workerThread;
    MyThread *worker;


};
#endif // MAINWINDOW_H

