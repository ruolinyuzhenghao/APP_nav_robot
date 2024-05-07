#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    /*******************************
     模块功能：设置背景与tab菜单栏
     *******************************/
    ui->tabwidget->addTab("健康检测");
    ui->tabwidget->addTab("水质监测");

    connect(ui->tabwidget,&QtMaterialTabs::currentChanged,this,&MainWindow::change_tab);
    /*******************************
     模块功能：健康管家
     *******************************/
    timerDrawLine = new QTimer();
    ui->widget->setDisabled(true);
    ui->widget_2->setDisabled(true);
    ui->widget_3->setDisabled(true);
    ui->widget_boatem_station->setDisabled(true);
    ui->widget_rovem_station->setDisabled(true);

    ui->label_boat_cabin->setScaledContents(true);
    ui->label_rov_cabin->setScaledContents(true);
    ui->label_boat_em->setScaledContents(true);
    ui->label_rov_em->setScaledContents(true);
    ui->label_all_anpei->setScaledContents(true);
    ui->label_dianji_anpei->setScaledContents(true);

    QPixmap pixmap(":/rec/boat.png");
    ui->label_boat_cabin->setPixmap(pixmap);
    pixmap.load(":/rec/rov.png");
    ui->label_rov_cabin->setPixmap(pixmap);
    pixmap.load(":/rec/em.png");
    ui->label_boat_em->setPixmap(pixmap);
    ui->label_rov_em->setPixmap(pixmap);
    pixmap.load(":/rec/dianliu_2.png");
    ui->label_all_anpei->setPixmap(pixmap);
    ui->label_dianji_anpei->setPixmap(pixmap);

    initchart();
//    series_info_boatdianji_1->hide();
//    series_info_dianliu->hide();
//    series_boat_wendu->hide();
//    series_boat_shidu->hide();
//    series_rov_shidu->hide();
//    series_rov_wendu->hide();

    ui->checkBox_shidu->hide();
    ui->checkBox_wendu->hide();
    connect(ui->checkBox_boatcabin,&QCheckBox::toggled,this,&MainWindow::showxuanxiang);
    connect(ui->checkBox_rovcabin,&QCheckBox::toggled,this,&MainWindow::showxuanxiang);
    connect(ui->checkBox_allanpei,&QCheckBox::toggled,this,&MainWindow::drawallanpei);
    connect(ui->checkBox_dianjianpei,&QCheckBox::toggled,this,&MainWindow::drawdianjianpei);
    connect(ui->checkBox_shidu,&QCheckBox::toggled,this,&MainWindow::drawshidu);
    connect(ui->checkBox_wendu,&QCheckBox::toggled,this,&MainWindow::drawwendu);

    connect(ui->checkBox_wendu,&QCheckBox::toggled,[=](){
        if(boat_or_rov)//水下
        {
            qDebug()<<"rov";
            series_rov_wendu->show();
        }
        else
        {
            qDebug()<<"boat";
            series_boat_wendu->show();
        }
    });
    connect(ui->checkBox_shidu,&QCheckBox::toggled,[=](){
        if(boat_or_rov)//水下
        {
            qDebug()<<"rov";
            series_rov_shidu->show();
        }
        else
        {
            qDebug()<<"boat";
            series_boat_shidu->show();
        }
    });


    /*******************************
     模块功能：数据传输
     *******************************/
    tcpsocket = new QTcpSocket(this);
    connect(ui->widget_switch,&QtMaterialToggle::toggled,this,&MainWindow::connect_tcp);
    connect(tcpsocket, &QTcpSocket::readyRead, this, &MainWindow::socket_Read_Data);//读取数据
    connect(tcpsocket, &QTcpSocket::disconnected, this, &MainWindow::socket_Disconnected);//断开连接


    tcpsocket_rov = new QTcpSocket(this);
    connect(tcpsocket_rov,&QTcpSocket::readyRead,this,&MainWindow::rovsocket_Read_Data);
    connect(tcpsocket_rov, &QTcpSocket::disconnected, this, &MainWindow::socket_Disconnected);//断开连接

    /*******************************
     模块功能：打开物联网网页
     *******************************/
    //webView = new QWebEngineView(this);
    //webView->load(QUrl("https://www.aliyun.com/?utm_content=se_1013698945"));
    //ui->gridLayout->addWidget(webView);
    //webView->resize(QSize(833,511));
    //webView->show();

    /*******************************
     模块功能：水质评估
     *******************************/
    myQthread* thread_shuizhi=new myQthread();

    ui->widget_shuizhipinggu->setShowPercent(false);
    ui->widget_shuizhipinggu->setUsedColor(QColor(0 ,186 ,211,255));//255 0 186 211
    ui->widget_shuizhipinggu->setValue(45);
    ui->progressBar->hide();
    QTimer* timer_thread=new QTimer();
    connect(timer_thread,&QTimer::timeout,[=](){
        if(thread_shuizhi->isFinished())
        {
            timer_thread->stop();
            ui->progressBar->hide();
            ui->widget_shuizhipinggu->setValue(result);
            ui->textEdit->setText("【水质情况】良好\n【改进建议】水质良好，不需要添加消毒剂等，请持续关注泳池健康");
            qDebug()<<"lalala";

        }
    });
    connect(this,SIGNAL(updatesignal(double,double)),thread_shuizhi,SLOT(get_shuizhicanshu(double,double)));
    connect(ui->pushButton_shuizhipinggu,&QPushButton::clicked,[=](){
        ui->progressBar->show();
        get_canshu(ph,wendu);
        connect(thread_shuizhi,SIGNAL(updateSignal(double,double)),this,SLOT(get_result(double,double)));
        timer_thread->start(10);
        thread_shuizhi->start();

    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

/*******************************
 函数功能：绘制界面背景
*******************************/
void MainWindow::change_tab()
{
    int idex = ui->tabwidget->currentIndex();
    switch (idex) {
    case 0:
        ui->stackedWidget->setCurrentWidget(ui->page_jiankangjiance);
        break;
    case 1:
        ui->stackedWidget->setCurrentWidget(ui->page_shuizhijiance);
        break;
    case 2:
        ui->stackedWidget->setCurrentWidget(ui->page_wulianwang);
        break;
    default:
        break;
    }
}

/*******************************
 函数功能：绘制界面背景
*******************************/
void MainWindow::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    QPixmap pixmap(":/rec/whitebackpic.jpg");
    painter.drawPixmap(this->rect(),pixmap);
}

/*******************************
 函数功能：初始化图表
*******************************/
void MainWindow::initchart()
{
    //设置主题
    QChart::ChartTheme theme_info = static_cast<QChart::ChartTheme>(QChart::ChartThemeLight);
    QChart::AnimationOptions options_info(QChart::GridAxisAnimations);
    QPen penY_info(Qt::lightGray,3,Qt::SolidLine,Qt::RoundCap,Qt::RoundJoin);
    //初始化图表、坐标轴、各种曲线
    chart_info = new QChart();
    series_info_dianliu=new QLineSeries();              //电流总量
    series_info_boatdianji_1=new QLineSeries();      //船电机电流1
    series_info_boatdianji_2=new QLineSeries();      //船电机电流2
    series_info_rovdianji_1=new QLineSeries();      //ROV电机电流1
    series_info_rovdianji_2=new QLineSeries();      //ROV电机电流2
    series_info_rovdianji_3=new QLineSeries();      //ROV电机电流3
    series_info_rovdianji_4=new QLineSeries();      //ROV电机电流4
    series_info_rovdianji_5=new QLineSeries();      //ROV电机电流5
    series_info_rovdianji_6=new QLineSeries();      //ROV电机电流6
    series_info_ph=new QLineSeries();                      //PH值
    series_info_zhuodu=new QLineSeries();                  //浊度
    series_info_o2=new QLineSeries();                     //溶解氧
    series_rov_shidu=new QLineSeries();                         //温度
    series_rov_wendu=new QLineSeries();                         //湿度
    series_boat_shidu=new QLineSeries();                         //温度
    series_boat_wendu=new QLineSeries();                         //湿度
    axisX_info = new QDateTimeAxis();                        //X坐标轴
    axisY_info = new QValueAxis();                        //Y坐标轴
    axisX_info->setTitleText("mm:ss");
    //加载主题
    chart_info->setTheme(theme_info);
    chart_info->setAnimationOptions(options_info);
    axisX_info->setTickCount(1);                     //设置坐标轴格数
    QFont labelsFont;
    labelsFont.setPixelSize(10);                     //参数字号，数字越小，字就越小
    axisX_info->setLabelsFont(labelsFont);
    axisY_info->setLabelsFont(labelsFont);
    axisY_info->setTickCount(1);
    axisX_info->setGridLineVisible(false);
    axisY_info->setGridLineVisible(false);
    axisX_info->setFormat("ss");
    axisX_info->setTickCount(8);
    axisX_info->setTitleText("time");                     //设置X轴范围
    axisY_info->setRange(-2,8);
    axisY_info->setGridLineVisible(true);            //隐藏背景网格Y轴框线
    axisX_info->setGridLineVisible(true);            //隐藏背景网格X轴框线
    axisY_info->setLinePen(penY_info);
    axisX_info->setLinePen(penY_info);
    chart_info->addAxis(axisX_info,Qt::AlignBottom); //设置坐标轴位于chart中的位置
    chart_info->addAxis(axisY_info,Qt::AlignLeft);
    chart_info->legend()->hide();

    //加载曲线
    chart_info->addSeries(series_boat_wendu);
    series_boat_wendu->attachAxis(axisX_info);
    series_boat_wendu->attachAxis(axisY_info);
    series_boat_wendu->setName("温度");
    chart_info->addSeries(series_boat_shidu);
    series_boat_shidu->attachAxis(axisX_info);
    series_boat_shidu->attachAxis(axisY_info);
    series_boat_shidu->setName("湿度");
    chart_info->addSeries(series_rov_wendu);
    series_rov_wendu->attachAxis(axisX_info);
    series_rov_wendu->attachAxis(axisY_info);
    series_rov_wendu->setName("温度");
    chart_info->addSeries(series_rov_shidu);
    series_rov_shidu->attachAxis(axisX_info);
    series_rov_shidu->attachAxis(axisY_info);
    series_rov_shidu->setName("湿度");

    chart_info->addSeries(series_info_boatdianji_1);
    series_info_boatdianji_1->attachAxis(axisX_info);
    series_info_boatdianji_1->attachAxis(axisY_info);
    series_info_boatdianji_1->setName("电机电流");

    //把chart显示到窗口上
    ui->widget_info->setChart(chart_info);
    ui->widget_info->setRenderHint(QPainter::Antialiasing);   //设置抗锯齿
}
/*******************************
 函数功能：显示温湿度选项
*******************************/
void MainWindow::showxuanxiang()
{
    if(ui->checkBox_boatcabin->isChecked())
    {
        ui->checkBox_shidu->show();
        ui->checkBox_wendu->show();
        boat_or_rov=false;
    }
    if(ui->checkBox_rovcabin->isChecked())
    {
        ui->checkBox_shidu->show();
        ui->checkBox_wendu->show();
        boat_or_rov=true;
    }
    if(!(ui->checkBox_boatcabin->isChecked()||ui->checkBox_rovcabin->isChecked()))
    {
        ui->checkBox_shidu->hide();
        ui->checkBox_wendu->hide();
        boat_or_rov=false;
    }
}
/*******************************
 函数功能：加载曲线
*******************************/
void MainWindow::draw_boatcabin()
{

}


/*******************************
 函数功能：连接Tcp
*******************************/
void MainWindow::connect_tcp()
{
    if(ui->widget_switch->isChecked())
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
            ui->widget_switch->setChecked(false);
            return;
        }
        tcp_ifconnect=true;
        rov_tcp_ifconnect=true;
        ui->widget->setDisabledColor(QColor(0,255,0));
        ui->widget_2->setDisabledColor(QColor(0,255,0));
        ui->widget_3->setDisabledColor(QColor(0,255,0));
        ui->widget_rovem_station->setDisabledColor(QColor(0,255,0));
        ui->widget_boatem_station->setDisabledColor(QColor(0,255,0));
    }
    else
    {
        //断开连接
        tcpsocket->disconnectFromHost();
        tcpsocket_rov->disconnectFromHost();
        tcp_ifconnect=false;
        rov_tcp_ifconnect=false;
    }
}
/*******************************
 函数功能：接受boat数据
*******************************/
void MainWindow::socket_Read_Data()
{
    QByteArray buffer;
    buffer = tcpsocket->readAll();//读取缓冲区数据
    qDebug()<<buffer.toHex();
    if(buffer.length()<=0)return;
    for(int i=0;i<buffer.size();i++)
    {
        i=unpackdata_Bag_boatstation(static_cast<unsigned char>(buffer[i]),i);
    }
}
/*******************************
 函数功能：接受rov数据
*******************************/
void MainWindow::rovsocket_Read_Data()
{
    QByteArray buffer;
    buffer = tcpsocket_rov->readAll();//读取缓冲区数据
    if(buffer.length()<=0)return;
    for(int i=0;i<buffer.size();i++)
    {
        i=unpackdata_Bag_rovstation(static_cast<unsigned char>(buffer[i]),i);

    }
}
/*******************************
 函数功能：解包boat数据
*******************************/
int MainWindow::unpackdata_Bag_boatstation(unsigned char ucData,int i)
{
    static uint8_t Bag4Buffer[250];//存储数据
    static uint8_t Bag4Cnt = 0;	//计数存储数据
    static uint8_t Bag4checkbit = 0X00;
    qDebug()<<Bag4Cnt<<"->"<<Bag4Buffer[Bag4Cnt];
    Bag4Buffer[Bag4Cnt]=ucData;	//将收到的数据存入缓冲区中
    if (Bag4Buffer[0]!=0XFE||Bag4Buffer[1]!=0X01||Bag4Buffer[2]!=0X01)
    {
        if(Bag4Cnt==2)
        {
            Bag4Cnt=0;//重新计数
            Bag4checkbit=0X00;
            return i-2;
        }
    }
    if (Bag4Cnt<boat_station_len-1)
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
            //获取当前时间
            QDateTime currentTime = QDateTime::currentDateTime();
            //温度
            int wendu_h=bytearray_to_ten(Bag4Buffer[4])*256;
            int wendu_l=bytearray_to_ten(Bag4Buffer[5]);
            float wendu = (wendu_h+wendu_l)/100;
            series_boat_wendu->append(currentTime.toMSecsSinceEpoch(),wendu);
            //湿度
            int shidu_h=bytearray_to_ten(Bag4Buffer[6])*256;
            int shidu_l=bytearray_to_ten(Bag4Buffer[7]);
            float shidu = (shidu_h+shidu_l)/100;
            //电压
            int dianya_h=bytearray_to_ten(Bag4Buffer[8])*256;
            int dianya_l=bytearray_to_ten(Bag4Buffer[9]);
            float dianya = (dianya_h+dianya_l)/100;
            //电流
            int dianliu_h=bytearray_to_ten(Bag4Buffer[10])*256;
            int dianliu_l=bytearray_to_ten(Bag4Buffer[11]);
            float dianliu = (dianliu_h+dianliu_l)/100;
            //电机电流
            int emdianliu_h=bytearray_to_ten(Bag4Buffer[12])*256;
            int emdianliu_l=bytearray_to_ten(Bag4Buffer[13]);
            float emdianliu = (emdianliu_h+emdianliu_l)/100;

        }
        Bag4Cnt=0;//清空缓存区
        Bag4checkbit=0X00;
        return i;
    }
}
/*******************************
 函数功能：解包rov数据
*******************************/
int MainWindow::unpackdata_Bag_rovstation(unsigned char ucData,int i)
{
    static uint8_t Bag4Buffer[250];//存储数据
    static uint8_t Bag4Cnt = 0;	//计数存储数据
    static uint8_t Bag4checkbit = 0X00;
    qDebug()<<Bag4Cnt<<"->"<<Bag4Buffer[Bag4Cnt];
    Bag4Buffer[Bag4Cnt]=ucData;	//将收到的数据存入缓冲区中
    if (Bag4Buffer[0]!=0XFE||Bag4Buffer[1]!=0X00||Bag4Buffer[2]!=0X01)
    {
        if(Bag4Cnt==2)
        {
            Bag4Cnt=0;//重新计数
            Bag4checkbit=0X00;
            return i-2;
        }
    }
    if (Bag4Cnt<boat_station_len-1)
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
            //温度rov
            int wendu_h=bytearray_to_ten(Bag4Buffer[4])*256;
            int wendu_l=bytearray_to_ten(Bag4Buffer[5]);
            float wendu = (wendu_h+wendu_l)/100;
            //湿度rov
            int shidu_h=bytearray_to_ten(Bag4Buffer[6])*256;
            int shidu_l=bytearray_to_ten(Bag4Buffer[7]);
            float shidu = (shidu_h+shidu_l)/100;
            //电压rov
            int dianya_h=bytearray_to_ten(Bag4Buffer[8])*256;
            int dianya_l=bytearray_to_ten(Bag4Buffer[9]);
            float dianya = (dianya_h+dianya_l)/100;
            //电流rov
            int dianliu_h=bytearray_to_ten(Bag4Buffer[10])*256;
            int dianliu_l=bytearray_to_ten(Bag4Buffer[11]);
            float dianliu = (dianliu_h+dianliu_l)/100;
            //电机电流rov
            int emdianliu_h=bytearray_to_ten(Bag4Buffer[12])*256;
            int emdianliu_l=bytearray_to_ten(Bag4Buffer[13]);
            float emdianliu = (emdianliu_h+emdianliu_l)/100;

        }
        Bag4Cnt=0;//清空缓存区
        Bag4checkbit=0X00;
        return i;
    }

}
/*******************************
 函数功能：十六进制转化十进制
*******************************/
uint MainWindow::bytearray_to_ten(unsigned char data)
{
    QByteArray buffer;
    buffer.resize(1);
    buffer[0]=data;
    //    send_stopp_data[1]=0X00;//水下
    //    send_stopp_data[2]=0XB0;//模式
    QString str = buffer.toHex();
    uint result = str.toInt(NULL,16);
    return result;
}
/*******************************
 函数功能：断开Tcp连接
*******************************/
void MainWindow::socket_Disconnected()
{
    ui->widget_switch->setChecked(false);
    qDebug()<<"Disconnected";
}

/*******************************
 函数功能：获取水质参数
*******************************/
double MainWindow::get_result(double a,double b)
{
    result = a;
    qDebug()<<"zhu result-->"<<result;
    qDebug()<<"zhu result-->"<<b;
}
double MainWindow::get_canshu(double a,double b)
{
    qDebug()<<"zhu canshu-->"<<a;
    qDebug()<<"zhu canshu-->"<<b;

    emit updatesignal(a,b);
}

/*******************************
 函数功能：绘制曲线
*******************************/
void MainWindow::drawallanpei()
{

    if(ui->checkBox_allanpei->isChecked())
    {
        axisY_info->setRange(-2,8);

        timerDrawLine->start(1000);
        chart_info->addSeries(series_info_dianliu);
        series_info_dianliu->attachAxis(axisX_info);
        series_info_dianliu->attachAxis(axisY_info);
        series_info_dianliu->setName("电流");
        connect(timerDrawLine,&QTimer::timeout,[=](){
            series_info_dianliu->setColor(QColor(120,0,255));
            // 更新X轴的范围
            axisX_info->setMin(QDateTime::currentDateTime().addSecs(-1*10));
            axisX_info->setMax(QDateTime::currentDateTime().addSecs(0));
            //在线上添加点
            series_info_dianliu->append(QDateTime::currentDateTime().toMSecsSinceEpoch(),(rand()%100)/60+4);
        });

    }
    else
    {
        timerDrawLine->stop();
        chart_info->removeSeries(series_info_dianliu);
    }
}
void MainWindow::drawdianjianpei()
{
    if(ui->checkBox_dianjianpei->isChecked())
    {
        axisY_info->setRange(-2,8);
        timerDrawLine->start(1000);
        chart_info->addSeries(series_info_boatdianji_1);
        series_info_boatdianji_1->attachAxis(axisX_info);
        series_info_boatdianji_1->attachAxis(axisY_info);
        series_info_boatdianji_1->setName("电机电流");
        connect(timerDrawLine,&QTimer::timeout,[=](){
            series_info_boatdianji_1->setColor(QColor(0,122,255));
            // 更新X轴的范围
            axisX_info->setMin(QDateTime::currentDateTime().addSecs(-1*10));
            axisX_info->setMax(QDateTime::currentDateTime().addSecs(0));
            //在线上添加点
            series_info_boatdianji_1->append(QDateTime::currentDateTime().toMSecsSinceEpoch(),(rand()%100)/60+3);
        });

    }
    else
    {
        timerDrawLine->stop();
        chart_info->removeSeries(series_info_boatdianji_1);
    }
}
void MainWindow::drawshidu()
{
    if(ui->checkBox_shidu->isChecked())
    {
        axisY_info->setMax(80);
        axisY_info->setMin(40);
        timerDrawLine->start(1000);
        chart_info->addSeries(series_info_rovdianji_1);
        series_info_rovdianji_1->attachAxis(axisX_info);
        series_info_rovdianji_1->attachAxis(axisY_info);
        series_info_rovdianji_1->setName("湿度");
        connect(timerDrawLine,&QTimer::timeout,[=](){
            series_info_rovdianji_1->setColor(QColor(255,122,0));
            // 更新X轴的范围
            axisX_info->setMin(QDateTime::currentDateTime().addSecs(-1*10));
            axisX_info->setMax(QDateTime::currentDateTime().addSecs(0));
            //在线上添加点
            series_info_rovdianji_1->append(QDateTime::currentDateTime().toMSecsSinceEpoch(),(rand()%100)/60+60);
        });

    }
    else
    {
        timerDrawLine->stop();
        chart_info->removeSeries(series_info_rovdianji_1);
    }

}
void MainWindow::drawwendu()
{
    if(ui->checkBox_wendu->isChecked())
    {
        axisY_info->setMax(30);
        axisY_info->setMin(15);
        timerDrawLine->start(1000);
        chart_info->addSeries(series_boat_wendu);
        series_boat_wendu->attachAxis(axisX_info);
        series_boat_wendu->attachAxis(axisY_info);
        series_boat_wendu->setName("温度");
        connect(timerDrawLine,&QTimer::timeout,[=](){
            series_boat_wendu->setColor(QColor(122,255,0));
            // 更新X轴的范围
            axisX_info->setMin(QDateTime::currentDateTime().addSecs(-1*10));
            axisX_info->setMax(QDateTime::currentDateTime().addSecs(0));
            //在线上添加点
            series_boat_wendu->append(QDateTime::currentDateTime().toMSecsSinceEpoch(),(rand()%100)/60+20);
        });

    }
    else
    {
        timerDrawLine->stop();
        chart_info->removeSeries(series_boat_wendu);
    }
}







