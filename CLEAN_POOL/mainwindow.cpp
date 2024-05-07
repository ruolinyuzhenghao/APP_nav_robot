#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    /*********ui界面**********/
    ui->setupUi(this);
    QPixmap bac_pic(":/base_pic/rec/SEVO_white.png");
    ui->label_icon_sevo->setScaledContents(true);
    ui->label_icon_sevo->setPixmap(bac_pic);
    ui->widget_automodel->setSize(50);
    ui->widget_automodel->setIcon(QIcon(":/base_pic/rec/robot_auto.png"));
    ui->widget_shoudong->setSize(50);
    ui->widget_shoudong->setIcon(QIcon(":/base_pic/rec/shoudong.png"));
    ui->widget_zidingyi->setSize(50);
    ui->widget_zidingyi->setIcon(QIcon(":/base_pic/rec/robot1.png"));
    connect(ui->pushButton_home,&QPushButton::clicked,this,[=](){
        switch (mode) {
        case 0:
            qDebug()<<"lalala";
            break;
        case 1:
            ui->mytab->removeTab(1);
            ui->mytab->removeTab(0);
            ui->mytab->addTab("MODEL_SELECT");
            break;
        case 2:
            ui->mytab->removeTab(1);
            ui->mytab->removeTab(0);
            ui->mytab->addTab("MODEL_SELECT");
            break;
        case 3:
            ui->mytab->removeTab(3);
            ui->mytab->removeTab(2);
            ui->mytab->removeTab(1);
            ui->mytab->removeTab(0);
            ui->mytab->addTab("MODEL_SELECT");
            break;
        default:
            break;
        }
        ui->stackedWidget->setCurrentWidget(ui->page_modeselect);
    });
    //建立tab_widght
    ui->mytab->adjustSize();
    ui->mytab->addTab("MODEL_SELECT");

    connect(ui->mytab,&QtMaterialTabs::currentChanged,[=](){changewidget();});
    //建立ip登陆界面
    ui->lineEdit_ip->setLabel("IP Address");
    ui->lineEdit_ip->setPlaceholderText("192.168.xx.xx");
    ui->lineEdit_port->setLabel("Port");
    ui->lineEdit_port->setPlaceholderText("9090");
    ui->pushButton_connect->setText("CONNECT");

    /********功能模式选择*********/
    connect(ui->pushButton_auto_kapian,&QPushButton::clicked,[=](){
        ui->mytab->addTab("MASTER");
        ui->mytab->addTab("MAP");
        ui->mytab->removeTab(0);
        ui->mytab->setCurrentTab(0);
        qDebug()<<ui->mytab->currentIndex();
        mode=1;
    });
    connect(ui->pushButton_shoudong_kapian,&QPushButton::clicked,[=](){
        ui->mytab->addTab("RUNMODEL");
        ui->mytab->addTab("STATUS");
        ui->mytab->removeTab(0);
        qDebug()<<ui->mytab->currentIndex();
        ui->mytab->setCurrentTab(0);
        ui->stackedWidget->setCurrentWidget(ui->stackedWidgetPage2);
        mode=2;
    });
    connect(ui->pushButton_zidingyi_kapian,&QPushButton::clicked,[=](){
        ui->mytab->addTab("MASTER");
        ui->mytab->addTab("RUNMODEL");
        ui->mytab->addTab("MAP");
        ui->mytab->addTab("STATUS");
        ui->mytab->removeTab(0);
        ui->mytab->setCurrentTab(0);
        mode=3;
    });
    /********建立WEB_SOCKET通信*********/
    socket = new QWebSocket();
    connect(socket, &QWebSocket::connected, [=](){ onConnected(); });
    connect(socket, &QWebSocket::disconnected, [=](){ closed(); });
    connect(socket, &QWebSocket::stateChanged, [=](){ stateChanged(); });
    connect(socket, &QWebSocket::textMessageReceived, [=](const QString message){ onTextMessageReceived(message); });
    connect(socket, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error), [=](QAbstractSocket::SocketError){
        qDebug() << socket->errorString();
    });

    /**************建立TCP通信**************/
    tcpsocket = new QTcpSocket(this);
    connect(tcpsocket, &QTcpSocket::readyRead, this, &MainWindow::socket_Read_Data);//读取数据
    connect(tcpsocket, &QTcpSocket::disconnected, this, &MainWindow::socket_Disconnected);//断开连接
    connect(ui->pushButton_boat_runlock,&QPushButton::clicked,this,[&](){pub_boat_runlock();});

    tcpsocket_rov = new QTcpSocket(this);
    connect(tcpsocket_rov,&QTcpSocket::readyRead,this,&MainWindow::rovsocket_Read_Data);
    connect(tcpsocket_rov, &QTcpSocket::disconnected, this, &MainWindow::socket_Disconnected);//断开连接
    connect(ui->pushButton_rov_runlock,&QPushButton::clicked,this,[&](){pub_rov_runlock();});


    /**************建立UDP通信**************/
    play_gif();
    udpsocket = new QUdpSocket(this);
    connect(ui->pushButton_openvideo,&QPushButton::clicked,this,&MainWindow::open_udp);

    connect(udpsocket,&QUdpSocket::readyRead,this,&MainWindow::udp_inputimage);


    /**************发布话题*****************/
    pub_stptimer = new QTimer(this);//中断
    pub_runtimer = new QTimer(this);//摇杆
    pub_runtimer->start(500);
    connect(pub_stptimer,&QTimer::timeout,[=](){pub_stop();});
    pub_contimer = new QTimer(this);//继续
    connect(pub_contimer,&QTimer::timeout,[=](){pub_continue();});
    connect(pub_runtimer,&QTimer::timeout,[=](){pub_run();});

    /***************接收话题*****************/
    connect(ui->pushButton_sub,&QPushButton::clicked,this,&MainWindow::sub_tpoic);
    connect(ui->pushButton_map,&QPushButton::clicked,this,&MainWindow::sub_map_tpoic);

    /*****************建图******************/
    initDraw();
    ui->horizontalSlider_X->setRange(0,20);
    ui->horizontalSlider_Y->setRange(0,20);
    connect(ui->horizontalSlider_X,&QSlider::valueChanged,[&](){axisX->setRange(-1*ui->horizontalSlider_X->value()-1,
                                                                                   ui->horizontalSlider_X->value()+1);});
    connect(ui->horizontalSlider_Y,&QSlider::valueChanged,[&](){axisY->setRange(-1*ui->horizontalSlider_Y->value()-1,
                                                                                   ui->horizontalSlider_Y->value()+1);});
    connect(ui->pushButton_clear,&QPushButton::clicked,this,&MainWindow::clrload_chart);

    /****************机器人状态****************/
    change_timer =new QTimer();
    change_timer->start(2000);
    QPixmap pixmap_boat(":/base_pic/rec/boat.png");
    QPixmap pixmap_rov(":/base_pic/rec/rov.png");
    ui->label_boat->setScaledContents(true);
    ui->label_boat->setPixmap(pixmap_boat);
    ui->label_rov->setScaledContents(true);
    ui->label_rov->setPixmap(pixmap_rov);
    connect(change_timer,&QTimer::timeout,this,&MainWindow::change_pic_station);

    ui->widget_boat_cabin->setDisabled(true);
    ui->widget_boat_em->setDisabled(true);
    ui->widget_rov_cabin->setDisabled(true);
    ui->widget_rov_em->setDisabled(true);
    ui->widget_rov_initial->setDisabled(true);
    ui->widget_rov_comlink->setDisabled(true);
    ui->widget_boat_initial->setDisabled(true);
    ui->widget_boat_comlink->setDisabled(true);
    ui->widget_boat_cabin->setDisabledColor(ui->widget_boat_cabin->activeColor());
    ui->widget_boat_em->setDisabledColor(ui->widget_boat_cabin->activeColor());
    ui->widget_rov_cabin->setDisabledColor(ui->widget_boat_cabin->activeColor());
    ui->widget_rov_em->setDisabledColor(ui->widget_boat_cabin->activeColor());
    qDebug()<<ui->widget_boat_cabin->activeColor();
    ui->widget_rov_initial->setDisabledColor(QColor(0,255,0,255));
    ui->widget_rov_comlink->setDisabledColor(QColor(0,255,0,255));
    ui->widget_boat_comlink->setDisabledColor(QColor(0,255,0,150));
    ui->widget_boat_initial->setDisabledColor(QColor(0,255,0,255));

    /**************健康检测***************/
    load_info();
    connect(ui->checkBox_water, &QtMaterialCheckBox::toggled, this, &MainWindow::draw_water);

    /**************电量预警**************/
    battery_warning=new battery_warn();

    ui->widget_battery_boat->setAlarmValue(60);
    ui->widget_battery_boat->setValue(100);
    ui->widget_battery_rov->setAlarmValue(60);
    ui->widget_battery_rov->setValue(100);

//    QTimer* timer_battery=new QTimer();
//    timer_battery->start(1000);
//    connect(timer_battery,&QTimer::timeout,[=](){
//        ui->widget_battery_boat->setValue(100-battery_count*10);
//        battery_count++;
//    });
//    connect(ui->widget_battery_boat,&Battery::valueChanged,[=](){
//        if(ui->widget_battery_boat->getValue()<60)
//        {
//            battery_warning->setWindowTitle("clean_pool");
//            battery_warning->resize(QSize(261,120));
//            battery_warning->show();
//        }
//    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

//切换界面
void MainWindow::changewidget()
{
    int b=ui->mytab->currentIndex();
    switch (mode) {
    case 1:
        if(b==0)
        {
            ui->stackedWidget->setCurrentWidget(ui->stackedWidgetPage1);
        }
        if(b==1)
        {
            ui->stackedWidget->setCurrentWidget(ui->stackedWidgetPage3);
        }
        break;
    case 2:
        if(b==0)
        {
            ui->stackedWidget->setCurrentWidget(ui->stackedWidgetPage2);
        }
        if(b==1)
        {
            ui->stackedWidget->setCurrentWidget(ui->stackedWidgetPage4);
        }
        break;
    default:
        if(b==0)
        {
            ui->stackedWidget->setCurrentWidget(ui->stackedWidgetPage1);
        }
        if(b==1)
        {
            ui->stackedWidget->setCurrentWidget(ui->stackedWidgetPage2);
        }
        if(b==2)
        {
            ui->stackedWidget->setCurrentWidget(ui->stackedWidgetPage3);
        }
        if(b==3)
        {
            ui->stackedWidget->setCurrentWidget(ui->stackedWidgetPage4);
        }
        break;
    }


}
//接收消息
void MainWindow::onTextMessageReceived(QString message)
{
    QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8());
    QJsonObject obj = doc.object();
    QJsonObject msg_obj = obj["msg"].toObject();
    //pose话题
    QJsonObject pose0_obj = msg_obj["pose"].toObject();
    if(QString(QJsonDocument(pose0_obj).toJson())!="{\n}\n")
    {
        //QJsonObject pose1_obj = pose0_obj["pose"].toObject();//实体导航请注释掉
        QJsonObject position_obj = pose0_obj["position"].toObject();
        if(ui->pushButton_clear->text()=="CLEAR")
        {
            qDebug()<<position_obj["x"].toDouble();
            DrawLine(position_obj["x"].toDouble(),position_obj["y"].toDouble());
        }
    }
    //地图消息
    QString ip = msg_obj["ip"].toString();
    if(ip=="map_mappoints")
    {
        if(map_rev)
        {
            double x1=msg_obj["x1"].toDouble();
            double y1=msg_obj["y1"].toDouble();
            double x2=msg_obj["x2"].toDouble();
            double y2=msg_obj["y2"].toDouble();
            qDebug()<<"###y="<<msg_obj["y1"].toDouble();
            qDebug()<<"###x="<<msg_obj["x1"].toDouble();
            if(std::abs(x1)>1000||std::abs(y1)>1000||std::abs(y2)>1000||std::abs(x2)>1000)
                qDebug()<<"erro map data";
            else
                Drawmap(x1,y1,x2,y2);

        }
    }
}
//正在连接
void MainWindow::onConnected()
{
    qDebug() << socket->state();

    isConnected = true;
    ui->pushButton_connect->setText("Disconnect");
}
//断开连接
void MainWindow::closed()
{
    qDebug() << socket->state();

    isConnected = false;
    ui->pushButton_connect->setText("Connect");
}
//更新连接状态
void MainWindow::stateChanged()
{
    qDebug() << "State changed.";
    qDebug() << socket->state();
    qDebug() << socket->error();

    if(socket->state() == QAbstractSocket::UnconnectedState && socket->error() == QAbstractSocket::RemoteHostClosedError)
    {
       QMessageBox::critical(this, tr("Failed to connect to server"),
                                   tr("Unable to a connect to server, please check your network connection."),
                                   QMessageBox::Ok);

    }
}
//构建连接
void MainWindow::on_pushButton_connect_clicked()
{
    if(ui->lineEdit_ip->text().isEmpty() || ui->lineEdit_port->text().isEmpty())
    {
       QMessageBox::warning(this, tr("Missing inputs"),
                            tr("IP address and port information are needed."),
                            QMessageBox::Ok);
    }
    else
    {

       if(ui->pushButton_connect->text().toStdString() == "Disconnect")
       {
           socket->close();
           ui->pushButton_connect->setText("Connect");
       }
       else
       {
           QString URL;
           URL = "ws://"+QString(ui->lineEdit_ip->text())+":"+QString(ui->lineEdit_port->text());
           qDebug() << URL;
           socket->open((QUrl(URL)));
           if(socket->state() == QAbstractSocket::ConnectedState || socket->state() == QAbstractSocket::ConnectingState)
           {
               ui->pushButton_connect->setText("Disconnect");
           }
       }
    }

}
//发布中断
void MainWindow::pub_stop()
{
    //TCP
    if(tcp_ifconnect)
    {
       QByteArray send_stop_data;
       send_stop_data.resize(6);
       send_stop_data[0]=0XFE;
       send_stop_data[1]=0X01;//水上
       send_stop_data[2]=0XB0;//模式
       send_stop_data[3]=0X01;
       send_stop_data[4]=0X00;//遥控模式
       send_stop_data[5]=0X00;
       for(int num=1;num<5;num++)
       {
           send_stop_data[5]=send_stop_data[5]+send_stop_data[num];
       }
       send_stop_data[5]=~send_stop_data[5];
       qDebug()<<send_stop_data.toHex();
       tcpsocket->write(send_stop_data);
       tcpsocket->flush();
    }
    if(rov_tcp_ifconnect)
    {
       QByteArray send_stopp_data;
       send_stopp_data.resize(6);
       send_stopp_data[0]=0XFE;
       send_stopp_data[1]=0X00;//水下
       send_stopp_data[2]=0XB0;//模式
       send_stopp_data[3]=0X01;
       send_stopp_data[4]=0X00;//遥控模式
       send_stopp_data[5]=0X00;
       for(int num=1;num<5;num++)
       {
           send_stopp_data[5]=send_stopp_data[5]+send_stopp_data[num];
       }
       send_stopp_data[5]=~send_stopp_data[5];
       qDebug()<<send_stopp_data.toHex();
       tcpsocket_rov->write(send_stopp_data);
       tcpsocket_rov->flush();
    }

    //ROS
    if(isConnected)
    {
       QJsonObject json;
       json["op"] = "publish";
       json["topic"] = "/btn_press";
       bool pause =false;
       json["data"] = pause;
       QJsonDocument doc(json);
       QByteArray docByteArray = doc.toJson(QJsonDocument::Compact);
       QString strJson = QLatin1String(docByteArray);

       qDebug() << strJson;

       //QString data_str = "up";
       //QByteArray data = data_str.toUtf8();
       socket->sendTextMessage(strJson);
       qDebug() << socket->errorString();
       qDebug() << socket->error();
    }
}
void MainWindow::on_pushButton_stop_2_pressed()
{
    pub_stptimer->start(30);
}
void MainWindow::on_pushButton_stop_2_released()
{
    pub_stptimer->stop();
}
//发布继续
void MainWindow::pub_continue()
{
    if(tcp_ifconnect)
    {
       //TCP
       if(tcp_ifconnect)
       {
           QByteArray send_continue_data;
           send_continue_data.resize(6);
           send_continue_data[0]=0XFE;
           send_continue_data[1]=0X01;//水上
           send_continue_data[2]=0XB0;//模式
           send_continue_data[3]=0X01;
           send_continue_data[4]=0X01;//遥控+自动模式
           send_continue_data[5]=0X00;
           for(int num=1;num<5;num++)
           {
               send_continue_data[5]=send_continue_data[5]+send_continue_data[num];
           }
           send_continue_data[5]=~send_continue_data[5];
           qDebug()<<send_continue_data.toHex();
           tcpsocket->write(send_continue_data);
           tcpsocket->flush();
       }
       if(rov_tcp_ifconnect)
       {
           QByteArray send_continuee_data;
           send_continuee_data.resize(6);
           send_continuee_data[0]=0XFE;
           send_continuee_data[1]=0X00;//水下
           send_continuee_data[2]=0XB0;//模式
           send_continuee_data[3]=0X01;
           send_continuee_data[4]=0X01;//遥控+自动模式
           send_continuee_data[5]=0X00;
           for(int num=1;num<5;num++)
           {
               send_continuee_data[5]=send_continuee_data[5]+send_continuee_data[num];
           }
           send_continuee_data[5]=~send_continuee_data[5];
           qDebug()<<send_continuee_data.toHex();
           tcpsocket_rov->write(send_continuee_data);
           tcpsocket_rov->flush();
       }

    }

    //ROS
    if(isConnected)
    {
       QJsonObject json;
       json["op"] = "publish";
       json["topic"] = "/btn_press_continue";
       bool continuee =false;
       json["data"] = continuee;
       QJsonDocument doc(json);
       QByteArray docByteArray = doc.toJson(QJsonDocument::Compact);
       QString strJson = QLatin1String(docByteArray);

       qDebug() << strJson;

       socket->sendTextMessage(strJson);
       qDebug() << socket->errorString();
       qDebug() << socket->error();
    }
}
void MainWindow::on_pushButton_connect_2_pressed()
{
    pub_contimer->start(30);
}
void MainWindow::on_pushButton_connect_2_released()
{
    pub_contimer->stop();
}
//发布摇杆
void MainWindow::pub_run()
{
    //TCP
    if(tcp_ifconnect)
    {
       int16_t curr_x=0,curr_y=0,cen_x=0,cen_y=0,x,y;
       QByteArray send_rocker_data;
       send_rocker_data.resize(9);
       for(int num=4;num<=7;num++)
       {
           send_rocker_data[num]=0X00;
       }
       curr_x=(int16_t)ui->rocker->getcurrentPoint().x();
       curr_y=(int16_t)ui->rocker->getcurrentPoint().y();
       cen_x=(int16_t)ui->rocker->getcenterPoint().x();
       cen_y=(int16_t)ui->rocker->getcenterPoint().y();
       x=curr_x-cen_x;
       y=curr_y-cen_y;
       qDebug()<<"x->"<<x;
       qDebug()<<"y->"<<y;
       send_rocker_data[0]=0XFE;
       send_rocker_data[1]=0X01;//水上
       send_rocker_data[2]=0XB1;//app遥控
       send_rocker_data[3]=0X04;
       send_rocker_data[4]=x>>8;
       send_rocker_data[5]=x;
       send_rocker_data[6]=y>>8;
       send_rocker_data[7]=y;
       send_rocker_data[8]=0X00;
       for(int num=1;num<8;num++)
       {
           send_rocker_data[8]=send_rocker_data[8]+send_rocker_data[num];
       }
       send_rocker_data[8]=~send_rocker_data[8];
       //send_rocker_data[8]=0X00;
       // ui->textEdit_tcp_write->setText(string);
       qDebug()<<send_rocker_data.toHex();
       tcpsocket->write(send_rocker_data);
       tcpsocket->flush();
    }
    if(rov_tcp_ifconnect)
    {
       int16_t curr_x=0,curr_y=0,cen_x=0,cen_y=0,x,y;
       QByteArray send_rocker_data;
       send_rocker_data.resize(9);
       for(int num=4;num<=7;num++)
       {
           send_rocker_data[num]=0X00;
       }
       curr_x=(int16_t)ui->rocker->getcurrentPoint().x();
       curr_y=(int16_t)ui->rocker->getcurrentPoint().y();
       cen_x=(int16_t)ui->rocker->getcenterPoint().x();
       cen_y=(int16_t)ui->rocker->getcenterPoint().y();
       x=curr_x-cen_x;
       y=curr_y-cen_y;

       send_rocker_data[0]=0XFE;
       send_rocker_data[1]=0X00;//水上
       send_rocker_data[2]=0XB2;//app遥控
       send_rocker_data[3]=0X04;
       send_rocker_data[4]=x>>8;
       send_rocker_data[5]=x;
       send_rocker_data[6]=y>>8;
       send_rocker_data[7]=y;
       send_rocker_data[8]=0X00;
       for(int num=1;num<8;num++)
       {
           send_rocker_data[8]=send_rocker_data[8]+send_rocker_data[num];
       }
       send_rocker_data[8]=~send_rocker_data[8];
       //send_rocker_data[8]=0X00;
       // ui->textEdit_tcp_write->setText(string);

       tcpsocket_rov->write(send_rocker_data);
       tcpsocket_rov->flush();
    }
    //send_rocker_data[8]=0X00;

}

//TCPclient读取数据
void MainWindow::socket_Read_Data()
{
    QByteArray buffer;
    buffer = tcpsocket->readAll();//读取缓冲区数据
    qDebug()<<buffer.toHex();
    if(buffer.length()<=0)return;
    for(int i=0;i<buffer.size();i++)
    {
       int a = unpackdata_Bag_boatstation(static_cast<unsigned char>(buffer[i]),i);
       int b = unpackdata_Bag_boatrunlock(static_cast<unsigned char>(buffer[i]),i);
       i=std::max(a,b);
    }
}
void MainWindow::rovsocket_Read_Data()
{
    QByteArray buffer;
    buffer = tcpsocket_rov->readAll();//读取缓冲区数据
    if(buffer.length()<=0)return;
    for(int i=0;i<buffer.size();i++)
    {
       int a=unpackdata_Bag_rovstation(static_cast<unsigned char>(buffer[i]),i);
       int b=unpackdata_Bag_rovrunlock(static_cast<unsigned char>(buffer[i]),i);
       i=std::max(a,b);
    }
}
int MainWindow::unpackdata_Bag_rovstation(unsigned char ucData,int i)//水下状态
{
    static uint8_t Bag1Buffer[250];//存储数据
    static uint8_t Bag1Cnt = 0;	//计数存储数据
    static uint8_t Bag1checkbit;
    Bag1Buffer[Bag1Cnt]=ucData;	//将收到的数据存入缓冲区中
    if (Bag1Buffer[0]!=0XFE||Bag1Buffer[1]!=0X00||Bag1Buffer[2]!=0X00)
    {
       if(Bag1Cnt==2)
       {
           Bag1Cnt=0;//重新计数
           Bag1checkbit=0X00;
           return i-2;
       }
    }
    if (Bag1Cnt<Bag_rovstation_Len-1)
    {
       Bag1checkbit+=Bag1Buffer[Bag1Cnt];
       qDebug()<<Bag1Cnt<<"->>"<<Bag1checkbit;
       Bag1Cnt++;
       return i;
    }
    else
    {
       if(~(Bag1checkbit-Bag1Buffer[0])==Bag1Buffer[Bag1Cnt])
       {
           switch (Bag1Buffer[4]) {
           case 0X01:
               ui->widget_rov_initial->setDisabledColor(QColor(255,0,0,1));
               break;
           case 0X02:
               ui->widget_rov_cabin->setDisabledColor(QColor(255,0,0,1));
               break;
           case 0X04:
               ui->widget_rov_comlink->setDisabledColor(QColor(255,0,0,1));
               break;
           case 0X08:
               ui->widget_rov_em->setDisabledColor(QColor(255,0,0,1));
               break;
           default:
               break;
           }
       }
       Bag1Cnt=0;//清空缓存区
       Bag1checkbit=0X00;
       return i;
    }
}
int MainWindow::unpackdata_Bag_boatstation(unsigned char ucData,int i)//水上状态
{
    static uint8_t Bag2Buffer[250];//存储数据
    static uint8_t Bag2Cnt = 0;	//计数存储数据
    static uint8_t Bag2checkbit;
    Bag2Buffer[Bag2Cnt]=ucData;	//将收到的数据存入缓冲区中
    if (Bag2Buffer[0]!=0XFE||Bag2Buffer[1]!=0X01||Bag2Buffer[2]!=0X00)
    {
       if(Bag2Cnt==2)
       {
           Bag2Cnt=0;//重新计数
           Bag2checkbit=0X00;
           return i-2;
       }

    }
    if (Bag2Cnt<Bag_boatstation_Len-1)
    {
       Bag2checkbit+=Bag2Buffer[Bag2Cnt];
       qDebug()<<Bag2Cnt<<"->>"<<Bag2checkbit;
       Bag2Cnt++;
       return i;
    }
    else
    {
       if(~(Bag2checkbit-Bag2Buffer[0])==Bag2Buffer[Bag2Cnt])
       {
           switch (Bag2Buffer[4]) {
           case 0X01:
               ui->widget_boat_initial->setDisabledColor(QColor(255,0,0,200));
               break;
           case 0X02:
               ui->widget_boat_cabin->setDisabledColor(QColor(255,0,0,200));
               break;
           case 0X04:
               ui->widget_boat_comlink->setDisabledColor(QColor(255,0,0,200));
               break;
           case 0X08:
               ui->widget_boat_em->setDisabledColor(QColor(255,0,0,200));
               break;
           default:
               break;
           }

       }
       Bag2Cnt=0;//清空缓存区
       Bag2checkbit=0X00;
       return i;
    }
}
int MainWindow::unpackdata_Bag_rovrunlock(unsigned char ucData,int i)//水下运动解锁/锁定
{
    static uint8_t Bag3Buffer[250];//存储数据
    static uint8_t Bag3Cnt = 0;	//计数存储数据
    static uint8_t Bag3checkbit;
    Bag3Buffer[Bag3Cnt]=ucData;	//将收到的数据存入缓冲区中
    if (Bag3Buffer[0]!=0XFE||Bag3Buffer[1]!=0X01||Bag3Buffer[2]!=0XA0)
    {
       if(Bag3Cnt==2)
       {
           Bag3Cnt=0;//重新计数
           Bag3checkbit=0X00;
           return i-2;
       }
    }
    if (Bag3Cnt<Bag_boatrunlock_Len-1)
    {
       Bag3checkbit+=Bag3Buffer[Bag3Cnt];
       qDebug()<<Bag3Cnt<<"->>"<<Bag3checkbit;
       Bag3Cnt++;
       return i;
    }
    else
    {
       if(~(Bag3checkbit-Bag3Buffer[0])==Bag3Buffer[Bag3Cnt])
       {
           qDebug()<<"806";
           if(Bag3Buffer[4]==0X00)//锁定成功
           {
               ui->pushButton_boat_runlock->setText("UNLOCK");
           }
           if(Bag3Buffer[4]==0X10)//解锁成功
           {
               ui->pushButton_boat_runlock->setText("LOCK");
           }
       }
       Bag3Cnt=0;//清空缓存区
       Bag3checkbit=0X00;
       return i;
    }
}
int MainWindow::unpackdata_Bag_boatrunlock(unsigned char ucData,int i)//水上运动解锁/锁定
{
    static uint8_t Bag4Buffer[250];//存储数据
    static uint8_t Bag4Cnt = 0;	//计数存储数据
    static uint8_t Bag4checkbit = 0X00;
    qDebug()<<Bag4Cnt<<"->"<<Bag4Buffer[Bag4Cnt];
    Bag4Buffer[Bag4Cnt]=ucData;	//将收到的数据存入缓冲区中
    if (Bag4Buffer[0]!=0XFE||Bag4Buffer[1]!=0X01||Bag4Buffer[2]!=0XA0)
    {
       if(Bag4Cnt==2)
       {
           Bag4Cnt=0;//重新计数
           Bag4checkbit=0X00;
           return i-2;
       }
    }
    if (Bag4Cnt<Bag_boatrunlock_Len-1)
    {
       Bag4checkbit+=Bag4Buffer[Bag4Cnt];
       qDebug()<<Bag4Cnt<<"->>"<<Bag4checkbit;
       Bag4Cnt++;
       return i;
    }
    else
    {
       if(~(Bag4checkbit-Bag4Buffer[0])==Bag4Buffer[Bag4Cnt])
       {
           qDebug()<<"806";
           if(Bag4Buffer[4]==0X00)//锁定成功
           {
               ui->pushButton_boat_runlock->setText("UNLOCK");
           }
           if(Bag4Buffer[4]==0X10)//解锁成功
           {
               ui->pushButton_boat_runlock->setText("LOCK");
           }
       }
       Bag4Cnt=0;//清空缓存区
       Bag4checkbit=0X00;
       return i;
    }
}

//TCP断开连接
void MainWindow::socket_Disconnected()
{
    //修改按键文字
    ui->pushButton_tcp_connect->setText("CONNECT");
    qDebug() << "Disconnected!";
}
//TCP连接
void MainWindow::on_pushButton_tcp_connect_clicked()
{
    if(ui->pushButton_tcp_connect->text() == tr("CONNECT"))
    {
       QString IP,IP_ROV;
       int port,port_rov;
       IP = "192.168.5.2"; //获取IP地址
       port = 2000; //获取端口号
       IP_ROV = "192.168.5.3";
       port_rov = 3000;
       tcpsocket->abort();//取消已有的连接
       tcpsocket_rov->abort();
       tcpsocket->connectToHost(QHostAddress(IP), port);//连接服务器
       tcpsocket_rov->connectToHost(QHostAddress(IP_ROV),port_rov);

       //等待连接成功
       //!tcpsocket->waitForConnected(10)||||!tcpsocket_rov->waitForConnected(10)
       if(!tcpsocket->waitForConnected(10))
       {
           qDebug() << "Connection failed!";
           return;
       }
       //tcp_ifconnect=true;
       rov_tcp_ifconnect=true;
       qDebug() << "Connect successfully!";
       ui->pushButton_tcp_connect->setText("DISCONNECT");
    }
    else
    {
       //断开连接
       tcpsocket->disconnectFromHost();
       tcpsocket_rov->disconnectFromHost();
       tcp_ifconnect=false;
       rov_tcp_ifconnect=false;
       //修改按键文字
       ui->pushButton_tcp_connect->setText("CONNECT");
    }
}
void MainWindow::pub_boat_runlock()
{
    if(tcp_ifconnect)
    {
       if(ui->pushButton_boat_runlock->text()=="UNLOCK")
       {
           QByteArray send_runlock_data;
           send_runlock_data.resize(6);
           send_runlock_data[0]=0XFE;
           send_runlock_data[1]=0X01;//水上
           send_runlock_data[2]=0XA0;//模式
           send_runlock_data[3]=0X01;
           send_runlock_data[4]=0X10;//解锁
           send_runlock_data[5]=0X00;
           for(int num=1;num<5;num++)
           {
               send_runlock_data[5]=send_runlock_data[5]+send_runlock_data[num];
           }
           send_runlock_data[5]=~send_runlock_data[5];
           //qDebug()<<send_runlock_data.toHex();
           tcpsocket->write(send_runlock_data);
           tcpsocket->flush();
       }
       else if(ui->pushButton_boat_runlock->text()=="LOCK")
       {
           QByteArray send_runlock_data;
           send_runlock_data.resize(6);
           send_runlock_data[0]=0XFE;
           send_runlock_data[1]=0X01;//水上
           send_runlock_data[2]=0XA0;//模式
           send_runlock_data[3]=0X01;
           send_runlock_data[4]=0X00;//锁定
           send_runlock_data[5]=0X00;
           for(int num=1;num<5;num++)
           {
               send_runlock_data[5]=send_runlock_data[5]+send_runlock_data[num];
           }
           send_runlock_data[5]=~send_runlock_data[5];
           qDebug()<<send_runlock_data.toHex();
           tcpsocket->write(send_runlock_data);
           tcpsocket->flush();
       }
    }
}
void MainWindow::pub_rov_runlock()
{
    if(tcp_ifconnect)
    {
       if(ui->pushButton_boat_runlock->text()=="UNLOCK")
       {
           QByteArray send_runlock_data;
           send_runlock_data.resize(6);
           send_runlock_data[0]=0XFE;
           send_runlock_data[1]=0X00;//水上
           send_runlock_data[2]=0XA0;//模式
           send_runlock_data[3]=0X01;
           send_runlock_data[4]=0X10;//解锁
           send_runlock_data[5]=0X00;
           for(int num=1;num<5;num++)
           {
               send_runlock_data[5]=send_runlock_data[5]+send_runlock_data[num];
           }
           send_runlock_data[5]=~send_runlock_data[5];
           qDebug()<<send_runlock_data.toHex();
           tcpsocket_rov->write(send_runlock_data);
           tcpsocket_rov->flush();
       }
       else if(ui->pushButton_boat_runlock->text()=="LOCK")
       {
           QByteArray send_runlock_data;
           send_runlock_data.resize(6);
           send_runlock_data[0]=0XFE;
           send_runlock_data[1]=0X00;//水上
           send_runlock_data[2]=0XA0;//模式
           send_runlock_data[3]=0X01;
           send_runlock_data[4]=0X00;//锁定
           send_runlock_data[5]=0X00;
           for(int num=1;num<5;num++)
           {
               send_runlock_data[5]=send_runlock_data[5]+send_runlock_data[num];
           }
           send_runlock_data[5]=~send_runlock_data[5];
           qDebug()<<send_runlock_data.toHex();
           tcpsocket_rov->write(send_runlock_data);
           tcpsocket_rov->flush();
       }
    }
}
//TCP发送数据
//void MainWindow::on_pushButton_tcp_send_clicked()
//{
//获取文本框内容并以ASCII码形式发送
//    tcpsocket->write(ui->lineEdit_tcp_write->text().toLatin1());
//    tcpsocket->flush();
//    QString write_text = ui->textEdit_tcp_write->toPlainText();
//    write_text+="\n已发送:\n";
//    write_text+=ui->lineEdit_tcp_write->text();
//    ui->textEdit_tcp_write->setPlainText(write_text);
//}

//订阅话题
void MainWindow::sub_tpoic()
{
    QJsonObject sub_json;

    QString pose_sub_id = QUuid::createUuid().toString();
    sub_json["op"] = "subscribe";
    sub_json["id"] = pose_sub_id;
    sub_json["topic"] = "/tracked_pose";
    sub_json["type"] = "geometry_msgs/PoseStamped";
    //sub_json["topic"] = "/amcl_pose";
    //sub_json["type"] = "geometry_msgs/PoseWithCovarianceStamped";
    sub_json["queue_length"] = 5;

    QJsonDocument doc(sub_json);
    QByteArray docByteArray = doc.toJson(QJsonDocument::Compact);
    QString strJson = QLatin1String(docByteArray);

    socket->sendTextMessage(strJson);
}
//订阅地图节点话题
void MainWindow::sub_map_tpoic()
{
    QJsonObject sub_json;

    QString pose_sub_id = QUuid::createUuid().toString();
    sub_json["op"] = "subscribe";
    sub_json["id"] = pose_sub_id;
    sub_json["topic"] = "/chatter_map";
    sub_json["type"] = "pub_map/Map";
    sub_json["queue_length"] = 1;

    QJsonDocument doc(sub_json);
    QByteArray docByteArray = doc.toJson(QJsonDocument::Compact);
    QString strJson = QLatin1String(docByteArray);

    socket->sendTextMessage(strJson);
    if(ui->pushButton_map->text()=="MAPP")
    {
       map_rev=true;
       ui->pushButton_map->setText("STOP");
    }
    else if(ui->pushButton_map->text()=="STOP")
    {
       map_rev=false;
       ui->pushButton_map->setText("MAPP");
    }

}
//初始化画布
void MainWindow::initDraw()
{
    QChart::ChartTheme theme = static_cast<QChart::ChartTheme>(QChart::ChartThemeLight);
    QChart::AnimationOptions options(QChart::GridAxisAnimations);
    QPen penY(Qt::lightGray,3,Qt::SolidLine,Qt::RoundCap,Qt::RoundJoin);

    chart = new QChart();
    series = new QSplineSeries;
    axisX = new QValueAxis();
    axisY = new QValueAxis();
    chart->setTheme(theme);
    chart->setAnimationOptions(options);
    biaoji_series = new QScatterSeries;              // 创建一个散点数据集对象
    biaoji_series->setName("圆点样式");
    biaoji_series->setMarkerShape(QScatterSeries::MarkerShapeCircle);  // 设置绘制的散点的样式为圆
    biaoji_series->setMarkerSize(10);                                  // 设置绘制的点的大小
    chart->addSeries(biaoji_series);                     // 将创建的series添加经chart中
    biaoji_series->attachAxis(axisX);                    //把数据添加到坐标轴上
    biaoji_series->attachAxis(axisY);
    biaoji_series->setColor(QColor(198,255,45));
    chart->legend()->hide();                             //隐藏图例
    chart->addSeries(series);                            //把线添加到chart
    axisX->setTickCount(1);                             //设置坐标轴格数
    QFont labelsFont;
    labelsFont.setPixelSize(10);   //参数字号，数字越小，字就越小
    axisX->setLabelsFont(labelsFont);
    axisY->setLabelsFont(labelsFont);
    axisY->setTickCount(1);
    axisX->setGridLineVisible(false);
    axisY->setGridLineVisible(false);
    axisX->setRange(-5,10);                              //设置X轴范围
    axisY->setRange(-4,6);
//    axisY->setLinePenColor(QColor(Qt::darkBlue));        //设置坐标轴颜色样式
//    axisY->setGridLineColor(QColor(Qt::darkBlue));
    axisY->setGridLineVisible(true); //隐藏背景网格Y轴框线
    axisX->setGridLineVisible(true); //隐藏背景网格X轴框线
    axisY->setLinePen(penY);
    axisX->setLinePen(penY);
    chart->addAxis(axisX,Qt::AlignBottom);               //设置坐标轴位于chart中的位置
    chart->addAxis(axisY,Qt::AlignLeft);
    series->attachAxis(axisX);                           //把数据添加到坐标轴上
    series->attachAxis(axisY);
    series->setColor(QColor(198,255,45));

    //把chart显示到窗口上
    ui->widget_map->setChart(chart);
    ui->widget_map->setRenderHint(QPainter::Antialiasing);   //设置抗锯齿
}
//画数据、动态更新数据
void MainWindow::DrawLine(double x,double y)
{
    series->append(x,y);
}
void MainWindow::Drawmap(double x1,double y1,double x2,double y2)
{
    map_max_x=std::max(x1,map_max_x);
    map_max_x=std::max(x2,map_max_x);
    map_max_y=std::max(y2,map_max_y);
    map_max_y=std::max(y1,map_max_y);
    map_min_x=std::min(x1,map_min_x);
    map_min_x=std::min(x2,map_min_x);
    map_min_y=std::min(y2,map_min_y);
    map_min_y=std::min(y1,map_min_y);
    axisX->setMax(map_max_x+1);
    axisX->setMin(map_min_x-1);
    axisY->setMax(map_max_y+1);
    axisY->setMin(map_min_y-1);

    qDebug()<<"x-->"<<axisX->max()<<"max--->"<<map_max_x;
    qDebug()<<"x-->"<<axisX->min()<<"min--->"<<map_min_x;
    qDebug()<<"x-->"<<axisY->max()<<"maxy--->"<<map_max_y;
    qDebug()<<"x-->"<<axisY->min()<<"miny--->"<<map_min_y;
    QLineSeries* line=new QLineSeries;
    map_series.append(line);
    if(!map_series.isEmpty())
    {
       chart->addSeries(map_series.back());
       map_series.back()->attachAxis(axisX);                           //把数据添加到坐标轴上
       map_series.back()->attachAxis(axisY);
       map_series.back()->setColor(QColor(198,255,45));
       map_series.back()->append(x1,y1);
       map_series.back()->append(x2,y2);
    }
}
//清空/重新加载曲线
void MainWindow::clrload_chart()
{
    chart->removeAllSeries();
    initDraw();
}
//UDP通信
void MainWindow::open_udp()
{ 
    if(ui->pushButton_openvideo->text()=="OPEN")
    {
       udpsocket->bind(QHostAddress::Any,6666);
       if(udpsocket->state()==QUdpSocket::ConnectedState)
       {
           ui->pushButton_openvideo->setText("CLOSE");
       }
    }
    else if(ui->pushButton_openvideo->text()=="CLOSE")
    {
       udpsocket->close();
       ui->pushButton_openvideo->setText("OPEN");
       play_gif();
    }
}
void MainWindow::udp_inputimage()
{
    ui->label_video->clear();
    quint64 size = udpsocket->pendingDatagramSize();
    QByteArray byte;
    byte.resize(size);

    QHostAddress* address=nullptr;
    quint16* port=nullptr;

    udpsocket->readDatagram(byte.data(),byte.size(),address,port);
    byte = qUncompress(byte);
    QBuffer buffer(&byte);
    QImageReader read_image(&buffer,"JPEG");
    QImage image = read_image.read();
    QImage img = image.rgbSwapped();
    ui->label_video->setScaledContents(true);
    ui->label_video->setPixmap(QPixmap::fromImage(img));
}
//添加水波GIF
void MainWindow::play_gif()
{
    ui->label_video->clear();
    QMovie *movie = new QMovie(":/base_pic/rec/water.gif");
    ui->label_video->setScaledContents(true);
    ui->label_video->setMovie(movie);
    movie->start();
}

//添加背景图片
void MainWindow::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    QPixmap pixmap(":/base_pic/rec/whitebackpic.jpg");
    painter.drawPixmap(this->rect(),pixmap);
}

//画出标记点
void MainWindow::mousePressEvent(QMouseEvent *e)
{
    if(ui->mytab->currentIndex()==2)
    {
       //打印鼠标位置处的点的坐标值
       qDebug()<<"shubiaoPoint-->"<<e->globalPosition();
       QPointF widgetPoint = ui->widget_map->mapFromGlobal(e->globalPosition());
       qDebug()<<"widegetPoint-->"<<widgetPoint;
       if(ifbiaoji)
       {
           biaojidian = chart->mapToValue(widgetPoint);
           qDebug()<<"axix_max"<<axisX->max();
           double x=biaojidian.x()*(axisX->max()-axisX->min())+axisX->min();
           double y=biaojidian.y()*(axisY->max()-axisY->min())+axisY->min();
           biaoji_series->append(biaojidian);
           ifbiaoji = false;//停止画点
           pub_goal(QPointF(x,y));
       }
    }
}
//发布标记点
void MainWindow::on_pushButton_biaoji_clicked()
{
    ifbiaoji=true;
}
//发布goal
void MainWindow::pub_goal(QPointF goal)
{
    if(isConnected){
       QJsonObject json;
       QJsonObject msgjson;
       QJsonObject headerjson;
       QJsonObject posejson;
       QJsonObject positionjson;
       QJsonObject orientationjson;

       json["op"] = "publish";
       json["topic"] = "/move_base_simple/goal";
       positionjson["x"] = goal.x();
       positionjson["y"] = goal.y();
       positionjson["z"] = 0.0;
       orientationjson["x"] = 0.0;
       orientationjson["y"] = 0.0;
       orientationjson["z"] = 0.0;
       orientationjson["w"] = 1.0;

       headerjson["frame_id"]="map";
       posejson["position"]=positionjson;
       posejson["orientation"]=orientationjson;

       msgjson["header"] = headerjson;
       msgjson["pose"] = posejson;
       json["msg"] = msgjson;

       QJsonDocument doc(json);
       QByteArray docByteArray = doc.toJson(QJsonDocument::Compact);
       QString strJson = QLatin1String(docByteArray);

       qDebug() << strJson;

       socket->sendTextMessage(strJson);
       qDebug() << socket->errorString();
       qDebug() << socket->error();
    }
}

//切换机器人情况
void MainWindow::change_pic_station()
{
    if(page_station==0)
    {
       ui->stackedWidget_station->setCurrentWidget(ui->page_boat);
       page_station=1;
    }
    else if(page_station==1)
    {
       ui->stackedWidget_station->setCurrentWidget(ui->page_rov);
       page_station=0;
    }
}

//加载更多信息
void MainWindow::load_info()
{
    QChart::ChartTheme theme_info = static_cast<QChart::ChartTheme>(QChart::ChartThemeLight);
    QChart::AnimationOptions options_info(QChart::GridAxisAnimations);
    QPen penY_info(Qt::lightGray,3,Qt::SolidLine,Qt::RoundCap,Qt::RoundJoin);

    chart_info = new QChart();
    series_info_dianliu=new QSplineSeries();           //电流总量

    series_info_boatdianji_1=new QSplineSeries();      //船电机电流1
    series_info_boatdianji_2=new QSplineSeries();      //船电机电流2
    series_info_rovdianji_1=new QSplineSeries();      //ROV电机电流1
    series_info_rovdianji_2=new QSplineSeries();      //ROV电机电流2
    series_info_rovdianji_3=new QSplineSeries();      //ROV电机电流3
    series_info_rovdianji_4=new QSplineSeries();      //ROV电机电流4

    series_info_ph=new QSplineSeries();          //PH值
    series_info_o2=new QSplineSeries();          //溶解氧
    series_info_zhuodu=new QSplineSeries();      //浊度

    axisX_info = new QValueAxis();
    axisY_info = new QValueAxis();

    chart_info->setTheme(theme_info);
    chart_info->setAnimationOptions(options_info);
    axisX_info->setTickCount(1);                                   //设置坐标轴格数
    QFont labelsFont;
    labelsFont.setPixelSize(10);                                   //参数字号，数字越小，字就越小
    axisX_info->setLabelsFont(labelsFont);
    axisY_info->setLabelsFont(labelsFont);
    axisY_info->setTickCount(1);
    axisX_info->setGridLineVisible(false);
    axisY_info->setGridLineVisible(false);
    axisX_info->setRange(0,20);                                     //设置X轴范围
    axisY_info->setRange(6,8);
    axisY_info->setGridLineVisible(true); //隐藏背景网格Y轴框线
    axisX_info->setGridLineVisible(true); //隐藏背景网格X轴框线
    axisY_info->setLinePen(penY_info);
    axisX_info->setLinePen(penY_info);
    chart_info->addAxis(axisX_info,Qt::AlignBottom);               //设置坐标轴位于chart中的位置
    chart_info->addAxis(axisY_info,Qt::AlignLeft);

    //把chart显示到窗口上
    ui->widget_infochart->setChart(chart_info);
    ui->widget_infochart->setRenderHint(QPainter::Antialiasing);   //设置抗锯齿

}
//水质监测
void MainWindow::draw_water()
{
    if (ui->checkBox_water->isChecked()) // "选中"
    {
       chart_info->addSeries(series_info_ph);
       series_info_ph->attachAxis(axisX_info);                           //把数据添加到坐标轴上
       series_info_ph->attachAxis(axisY_info);

       series_info_ph->setColor(QColor(120,0,255));
       series_info_ph->setName("ph");

       std::default_random_engine e;
       std::normal_distribution<double> u(0,0.5); // 均值为0，标准差为1
       e.seed(time(0));

       series_info_ph->append(0, 7.12);
       series_info_ph->append(2, 7.56);
       series_info_ph->append(3, 7.36);
       series_info_ph->append(7, 7.63);
       series_info_ph->append(10, 7.56);
       *series_info_ph << QPointF(11, 7.22) << QPointF(13, 7.45) << QPointF(17, 7.33) << QPointF(18, 7.44) << QPointF(20, 7.55);
    }
}
//电机电流
void MainWindow::draw_dianliu()
{
    chart_info->addSeries(series_info_ph);
    chart_info->addSeries(series_info_o2);

    series_info_ph->attachAxis(axisX_info);                           //把数据添加到坐标轴上
    series_info_o2->attachAxis(axisY_info);
    series_info_ph->setColor(QColor(0,0,255));
    series_info_o2->setColor(QColor(0,190,210));




}
//电机电流
void MainWindow::draw_dianjidianliu()
{
    chart_info->addSeries(series_info_ph);
    chart_info->addSeries(series_info_o2);

    series_info_ph->attachAxis(axisX_info);                           //把数据添加到坐标轴上
    series_info_o2->attachAxis(axisY_info);
    series_info_ph->setColor(QColor(0,0,255));
    series_info_o2->setColor(QColor(0,190,210));
    if (ui->checkBox_dainjidianliu->isChecked()) // "选中"
    {

       qDebug() << "apple:Checked";

    }

}
