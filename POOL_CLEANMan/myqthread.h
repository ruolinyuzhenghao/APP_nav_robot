#ifndef MYQTHREAD_H
#define MYQTHREAD_H

#include <QThread>
#include <QDebug>
#include <QTimer>
#include <iostream>
#include <curl/curl.h>
#include <json-c/json.h>
#include <fstream>
#include <memory>
#include <string>

class myQthread : public QThread
{
    Q_OBJECT
public:
    myQthread();
    //定义虚函数run()用来执行线程
protected:
    virtual void run();

    //定义函数设置名字
private:
    QString threadName;
    double ph=0,wendu=0;
private slots:
    void get_shuizhicanshu(double a,double b);
public:
    void Assessment();
    void setThreadName(QString);
    void get_cnahu(double a,double b);
signals:
    void updateSignal(double a,double b);



};

#endif // MYQTHREAD_H
