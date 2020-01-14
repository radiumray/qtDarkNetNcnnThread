#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <QObject>
//#include <QDebug>
//#include <QThread>

class MyThread : public QObject {
    Q_OBJECT
public:
    explicit MyThread(QObject *parent = nullptr);
//    MyThread(int start, int end);
    ~MyThread();

public slots:
    void doWork();
    void stopWork();

signals:
    void updateCount(int);
    void finished();


private:
    int m_countStart;
    int m_countEnd;
    bool m_running=true;
};



#endif // MYTHREAD_H
