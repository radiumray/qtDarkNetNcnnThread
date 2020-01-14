#include "mythread.h"
#include "portablesleep.h"
#include <QDebug>
#include <QThread>
#include <QApplication>


MyThread::MyThread(QObject *parent) : QObject(parent) {
    m_countStart = 0;
    m_countEnd = 1000;
}

MyThread::~MyThread() {

}



void MyThread::doWork() {
    for (int i = m_countStart; i <= m_countEnd; i++) {

        if(!m_running) {
            emit updateCount(0);
            break;
        }
        emit updateCount(i);
        qApp->processEvents();
        PortableSleep::msleep(5);
    }
    emit finished();
}

void MyThread::stopWork() {
    m_running = false;
}
