#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "platform.h"
#include "net.h"
#include <QMessageBox>
#include <QThread>
using namespace cv;

std::vector<std::string> labels;
CustomizedNet yolo;

char* lables=(char *)"models/yoloV3Lable.txt";
const char* modelParam="models/yolov3-tiny.param";
const char* modelBin="models/yolov3-tiny.bin";


struct Object
{
    cv::Rect_<float> rect;
    int label;
    float prob;
};


static void draw_objects(const Mat &bgr, const std::vector<Object> &objects,
                         std::vector<std::string> &labels) {

  Mat image = bgr;
  for (size_t i = 0; i < objects.size(); i++) {
    const Object &obj = objects[i];

    // fprintf(stderr, "%-3d[%-16s] = %.5f at %.2f %.2f %.2f x %.2f\n", obj.label,
    //         labels[obj.label].c_str(), obj.prob, obj.rect.x, obj.rect.y,
    //         obj.rect.width, obj.rect.height);

    rectangle(image, obj.rect, Scalar(255, 0, 0));
    char text[256];
    sprintf(text, "%s %.1f%%", labels[obj.label].c_str(), double(obj.prob * 100));
    int baseLine = 0;
    Size label_size =
        getTextSize(text, FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseLine);
    int x = int(obj.rect.x);
    int y = int(obj.rect.y - label_size.height - baseLine);
    if (y < 0)
      y = 0;
    if (x + label_size.width > image.cols)
      x = image.cols - label_size.width;
    rectangle(
        image,
        Rect(Point(x, y),
                 Size(label_size.width, label_size.height + baseLine)),
        Scalar(255, 255, 255), CV_FILLED);
    putText(image, text, Point(x, y + label_size.height),
                FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 0));
  }
}

static int detect_yolov(Mat &bgr) {
    ncnn::Input *input = (ncnn::Input *)yolo.get_layer_from_name("data");
    ncnn::Mat in = ncnn::Mat::from_pixels_resize(bgr.data, ncnn::Mat::PIXEL_BGR2RGB, bgr.cols, bgr.rows, input->w, input->h);
    const float norm_vals[3] = {1 / 255.0, 1 / 255.0, 1 / 255.0};
    in.substract_mean_normalize(0, norm_vals);
    int img_h = bgr.rows;
    int img_w = bgr.cols;
    ncnn::Extractor ex = yolo.create_extractor();
    ex.set_num_threads(4);
    ex.input("data", in);
    ncnn::Mat out;
    ncnn::Blob *out_blob = yolo.get_last_layer_output_blob();
    int result = ex.extract(out_blob->name.c_str(), out);
    if (result != 0) {
        printf("ncnn error: %d\n", result);
        return result;
    }
    std::vector<Object> objects;
    for (int i = 0; i < out.h; i++) {
        const float *values = out.row(i);
        Object object;
        object.label = int(values[0]);
        object.prob = values[1];
        object.rect.x = values[2] * img_w;
        object.rect.y = values[3] * img_h;
        object.rect.width = values[4] * img_w - object.rect.x;
        object.rect.height = values[5] * img_h - object.rect.y;
        objects.push_back(object);
    }
    draw_objects(bgr, objects, labels);
    return 0;
}


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    load_classifier_labels(labels, lables);
    register_darknet_layer(yolo);
    yolo.load_param(modelParam);
    yolo.load_model(modelBin);

    ui->videoLabel->setScaledContents(true);
    ui->progressBar->setRange(0, 1000);
    ui->progressBar->setValue(0);

    this->countRunning=false;

    connectSignalsSlots();

}

MainWindow::~MainWindow()
{
    delete ui;
    delete timer;
}


//Mat->QImage
QImage MainWindow::Mat2QImage(cv::Mat cvImg)
{
    QImage qImg;


    std::vector<Object> objects;

    detect_yolov(cvImg);

    cv::cvtColor(cvImg,cvImg,cv::COLOR_BGR2RGB);

    qImg=QImage((const unsigned char*)(cvImg.data),
                             cvImg.cols,cvImg.rows,          //宽，高
                             cvImg.step,                     //步长
                             QImage::Format_RGB888);         //24位图，RGB=[0,1,2]
    //return qImg.rgbSwapped();
    return qImg;
}


void MainWindow::on_pushButtonStart_clicked()
{
    capture.open(0);          //打开默认的摄像头
    if (capture.isOpened())
    {
        capture >> frame;     //获取一帧图像
        if(frame.empty())
        {
            qDebug()<<"error!";
        }
        if(!frame.empty())
        {
            image = Mat2QImage(frame);      //Mat->QImage
            ui->videoLabel->setPixmap(QPixmap::fromImage(image));   //QImage->QPixmap,并将图像显示在label上
            timer = new QTimer(this);  //计时器
//            timer->setInterval(30);    //设置间隔：30毫秒
            timer->setInterval(5);
            connect(timer,SIGNAL(timeout()),this,SLOT(nextFrame()));
                                           //一旦超时则触发nextFrame(),即显示下一帧图像
                                           //SIGNAL(timeout())：每当计时结束，计时器归零并重新计时，并发送一个信号激活slot函数
            timer->start();       //重新开始计时
        }
    }
}

//显示下一帧图像
void MainWindow::nextFrame()
{
    capture >> frame;          //获取的图像frame是Mat数据矩阵
    image = Mat2QImage(frame);//Mat->QImage
    ui->videoLabel->setPixmap(QPixmap::fromImage(image));    //fromImage(image): QImage->QPixmap
                                                        //最后将 QPixmap 将图像显示在label上
}


void MainWindow::on_pushButtonStop_clicked()
{
    timer->stop();                    //停止计时
    capture.release();                //关闭摄像头
    ui->videoLabel->setPixmap(QPixmap());  //释放内存，关闭画面
}

void MainWindow::on_progressBar_valueChanged(int value)
{

}

void MainWindow::updateCount(int cnt)
{
//    qDebug()<<cnt;
    ui->progressBar->setValue(cnt);
}

void MainWindow::startCount()
{

    if (countRunning) {
        QMessageBox::critical(this, "Error", "Count is already running!");
        return;
    }

    workerThread = new QThread;
    worker       = new MyThread();

    worker->moveToThread(workerThread);
    connect(workerThread, SIGNAL(started()), worker, SLOT(doWork()));
    connect(worker, SIGNAL(finished()), workerThread, SLOT(quit()));
    connect(worker, SIGNAL(finished()), worker, SLOT(deleteLater()));
    connect(worker, SIGNAL(finished()), this, SLOT(countFinished()));
    connect(workerThread, SIGNAL(finished()), workerThread, SLOT(deleteLater()));
    connect(worker, SIGNAL(updateCount(int)), this, SLOT(updateCount(int)));
    connect(ui->pushButtonProStop, SIGNAL(clicked()), worker, SLOT(stopWork()));
    workerThread->start();

    countRunning = true;
}

void MainWindow::countFinished()
{
    countRunning = false;
}


void MainWindow::connectSignalsSlots()
{
    connect(ui->pushButtonPro, SIGNAL(clicked()), this, SLOT(startCount()));

}
