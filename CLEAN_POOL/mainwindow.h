#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QColor>
#include <QVector>
//ui界面
#include <qtmaterialtabs.h>
#include <qtmaterialtextfield.h>
#include <qtmaterialslider.h>
#include <customrocker.h>
#include <QGridLayout>
#include <QMessageBox>
#include <QPalette>
//WEB通信
#include <QWebSocket>
#include <QAbstractSocket>
#include <QTimer>
#include <QJsonObject>
#include <QJsonDocument>
//TCP通信
#include <QTcpSocket>
#include <QAbstractSocket>
#include <QDataStream>
#include <QByteArray>
#include <string>
#include <QString>
//UDP通信
#include <QMovie>
#include <QUdpSocket>
#include <QBuffer>
#include <QImageReader>
//建图
#include <QPainter>
#include <QChart>
#include <QSplineSeries>
#include <QValueAxis>
#include <QSlider>
#include <QPoint>
#include <QVector>
#include <QList>
//地图操作
#include <QChartView>
#include <QMouseEvent>
#include <QScatterSeries>
//健康检测
#include <QCheckBox>
#include <ctime>
#include <random>
//电量预警
#include <battery_warn.h>

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
    void play_gif();
    /******切换界面*******/
    void changewidget();

    /******WEB_SCOKET*****/
    void onConnected();
    void closed();
    void stateChanged();
    void publishtopic();
    void onTextMessageReceived(QString message);

    /******PUB_SUB********/
    //中断
    void pub_stop();
    //继续
    void pub_continue();
    //摇杆
    void pub_run();
    //目标点
    void pub_goal(QPointF goal);

    //订阅
    void sub_tpoic();
    void sub_map_tpoic();

    /******TCP_SCOKET*****/
    void socket_Read_Data();
    void rovsocket_Read_Data();
    void socket_Disconnected();
    std::string DEC_HEX(qint8 DEC);
    void pub_rov_runlock();
    void pub_boat_runlock();
    int unpackdata_Bag_rovstation(unsigned char ucData,int i);//水下状态
    int unpackdata_Bag_boatstation(unsigned char ucData,int i);//水上状态
    int unpackdata_Bag_boatrunlock(unsigned char ucData,int i);//水上运动解锁/锁定
    int unpackdata_Bag_rovrunlock(unsigned char ucData,int i);//水下运动解锁/锁定

    /******UDP_SCOKET*****/
    void udp_inputimage();
    void open_udp();

    /********建图********/
    void initDraw();
    void DrawLine(double x,double y);
    void Drawmap(double x1,double y1,double x2,double y2);
    void clrload_chart();

    /********健康检测*********/
    void load_info();
    void draw_water();
    void draw_dianliu();
    void draw_dianjidianliu();


    /******地图操作*******/
    void mousePressEvent(QMouseEvent *e);

    /********机器人状态*******/
    void change_pic_station();

private slots:
    //连接
    void on_pushButton_connect_clicked();
    //中断
    void on_pushButton_stop_2_pressed();
    void on_pushButton_stop_2_released();
    //继续
    void on_pushButton_connect_2_pressed();
    void on_pushButton_connect_2_released();
    //摇杆
    void on_pushButton_tcp_connect_clicked();
    //标记
    void on_pushButton_biaoji_clicked();

private:
    /******ui界面*****/
    Ui::MainWindow *ui;
    int mode=0;

    /******WEB_SCOKET*****/
    QWebSocket *socket;
    QString URL;
    bool isConnected =false;

    /******PUB_SUB********/
    QTimer *pub_stptimer;
    QTimer *pub_contimer;
    QTimer *pub_runtimer;

    /*********TCP*********/
    QTcpSocket *tcpsocket;
    QTcpSocket *tcpsocket_rov;
    bool tcp_ifconnect=false;
    bool rov_tcp_ifconnect=false;
    int Bag_rovstation_Len = 6;                       //水下状态信息长度
    int Bag_boatstation_Len = 6;                      //水上状态信息长度
    int Bag_rovrunlock_Len = 6;                       //水下运动解锁/锁定
    int Bag_boatrunlock_Len = 6;                      //水上运动解锁/锁定
    /*********UDP*********/
    QUdpSocket *udpsocket;

    /********建图********/
    QChart *chart;                           //画布
    QSplineSeries *series;                   //轨迹线
    QValueAxis *axisX;                       //轴
    QValueAxis *axisY;
    QVector<double> mappoints;
    QList<QLineSeries*>map_series;
    bool map_rev =true;
    double map_max_x=-1000,map_max_y=-1000;
    double map_min_x=1000,map_min_y=1000;

    /*******地图操作********/
    bool ifbiaoji = false;
    QPointF biaojidian;
    QScatterSeries* biaoji_series;

    /********机器人状态*******/
    QTimer* change_timer;
    int page_station=0;

    /******健康检测********/
    QChart *chart_info;                           //画布
    QValueAxis *axisX_info;                       //轴
    QValueAxis *axisY_info;

    QSplineSeries *series_info_dianliu;           //电流总量

    QSplineSeries *series_info_boatdianji_1;      //船电机电流1
    QSplineSeries *series_info_boatdianji_2;      //船电机电流2
    QSplineSeries *series_info_rovdianji_1;      //ROV电机电流1
    QSplineSeries *series_info_rovdianji_2;      //ROV电机电流2
    QSplineSeries *series_info_rovdianji_3;      //ROV电机电流3
    QSplineSeries *series_info_rovdianji_4;      //ROV电机电流4

    QLineSeries *series_info_ph;          //PH值
    QLineSeries *series_info_o2;          //溶解氧
    QLineSeries *series_info_zhuodu;      //浊度

    int battery_count=0;

    /*******电量预警*********/
    battery_warn* battery_warning;

public slots:




};
#endif // MAINWINDOW_H
