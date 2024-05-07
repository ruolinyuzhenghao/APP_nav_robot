#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
//ui设计
#include <QPainter>
#include <QPaintEvent>
//健康管家
#include <QChart>
#include <QValueAxis>
#include <QSplineSeries>
#include <QLineSeries>
#include <QCheckBox>
#include <QDateTime>
#include <QDateTimeAxis>
//数据传输
#include <QTcpSocket>
#include <QHostAddress>
#include <RectWaveProgressBar.h>
//物联网
#include <QtWebEngineWidgets/QWebEngineView>
//水质评估
#include <QTimer>
#include <myqthread.h>
#include <iostream>
#include <curl/curl.h>
#include <json-c/json.h>
#include <fstream>
#include <memory>
#include <string>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    /******界面设计*******/
    void paintEvent(QPaintEvent *);
    void change_tab();

    /*******健康管家******/
    void initchart();
    void draw_boatcabin();
    void showxuanxiang();
    void drawallanpei();
    void drawdianjianpei();
    void drawshidu();
    void drawwendu();

    /*******数据传输******/
    void connect_tcp();
    void socket_Read_Data();
    void socket_Disconnected();
    void rovsocket_Read_Data();
    int unpackdata_Bag_rovstation(unsigned char ucData,int i);
    int unpackdata_Bag_boatstation(unsigned char ucData,int i);
    uint bytearray_to_ten(unsigned char data);

    /*******水质评估******/
    double get_canshu(double a,double b);
private slots:
    double get_result(double a,double b);

signals:
    void updatesignal(double a,double b);
private:
    Ui::MainWindow *ui;
    /*******健康管家******/
    QTimer* timerDrawLine;

    QChart *chart_info;                              //画布
    QDateTimeAxis *axisX_info;                            //轴
    QValueAxis *axisY_info;

    QLineSeries *series_info_dianliu;             //电流总量

    QLineSeries *series_info_boatdianji_1;      //船电机电流1
    QLineSeries *series_info_boatdianji_2;      //船电机电流2
    QLineSeries *series_info_rovdianji_1;      //ROV电机电流1
    QLineSeries *series_info_rovdianji_2;      //ROV电机电流2
    QLineSeries *series_info_rovdianji_3;      //ROV电机电流3
    QLineSeries *series_info_rovdianji_4;      //ROV电机电流4
    QLineSeries *series_info_rovdianji_5;      //ROV电机电流5
    QLineSeries *series_info_rovdianji_6;      //ROV电机电流6

    QLineSeries *series_info_ph;                      //PH值
    QLineSeries *series_info_o2;                     //溶解氧
    QLineSeries *series_info_zhuodu;                  //浊度

    QLineSeries *series_boat_wendu;                        //温度
    QLineSeries *series_rov_wendu;                        //温度
    QLineSeries *series_boat_shidu;                        //湿度
    QLineSeries *series_rov_shidu;                        //湿度

    bool boat_or_rov = false;

    QDateTime curDateTIme = QDateTime::currentDateTime();

    /*******数据传输******/
    QTcpSocket *tcpsocket;
    QTcpSocket *tcpsocket_rov;
    bool tcp_ifconnect=false;
    bool rov_tcp_ifconnect=false;
    int boat_station_len=15,rov_station_leb=11;

    /*******物联网******/
    QWebEngineView *webView;

    /******水质评估******/
    double ph=96,wendu=0,result=0;


};
#endif // MAINWINDOW_H
