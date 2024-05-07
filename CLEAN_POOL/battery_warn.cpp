#include "battery_warn.h"
#include "ui_battery_warn.h"

battery_warn::battery_warn(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::battery_warn)
{
    ui->setupUi(this);
    QPixmap bac_pic(":/base_pic/rec/dianliang_warn.png");
    ui->label_warn->setScaledContents(true);
    ui->label_warn->setPixmap(bac_pic);
}

battery_warn::~battery_warn()
{
    delete ui;
}



