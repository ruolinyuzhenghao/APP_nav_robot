//#pragma execution_character_set("utf-8")

#include "battery.h"
#include "qpainter.h"
#include "qtimer.h"
#include <QDebug>

Battery::Battery(QWidget *parent) : QWidget(parent)
{
    minValue = 0;
    maxValue = 100;
    value = 0;
    alarmValue = 30;
    step = 0.5;

    borderRadius = 8;
    bgRadius = 5;
    headRadius = 3;

    borderColorStart = QColor(100, 100, 100);
    borderColorEnd = QColor(80, 80, 80);
    alarmColorStart = QColor(250, 118, 113);
    alarmColorEnd = QColor(204, 38, 38);
    normalColorStart = QColor(50, 205, 51);
    normalColorEnd = QColor(60, 179, 133);

    isForward = false;
    currentValue = 0;

    timer = new QTimer(this);
    timer->setInterval(10);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateValue()));
}

Battery::~Battery()
{
    if (timer->isActive())
        timer->stop();
}

void Battery::paintEvent(QPaintEvent *)
{
    //绘制准备工作,启用反锯齿
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    //绘制边框
    drawBorder(&painter);
    //绘制背景
    drawBg(&painter);
    //绘制头部
    drawHead(&painter);
}

void Battery::drawBorder(QPainter *painter)
{
    painter->save();

    qreal headWidth = width() / 10;
    qreal batteryWidth = width() - headWidth;

    //绘制电池边框
    QPointF topLeft(5, 5);
    QPointF bottomRight(batteryWidth, height() - 5);
    batteryRect = QRectF(topLeft, bottomRight);

    painter->setPen(QPen(borderColorStart, 5));
    painter->setBrush(Qt::NoBrush);
    painter->drawRoundedRect(batteryRect, borderRadius, borderRadius);

    painter->restore();
}

void Battery::drawBg(QPainter *painter)
{
    painter->save();

    QLinearGradient batteryGradient(QPointF(0, 0), QPointF(0, height()));
    if (currentValue <= alarmValue)
    {
        batteryGradient.setColorAt(0.0, alarmColorStart);
        batteryGradient.setColorAt(1.0, alarmColorEnd);
    }
    else
    {
        batteryGradient.setColorAt(0.0, normalColorStart);
        batteryGradient.setColorAt(1.0, normalColorEnd);
    }

    int margin = qMin(width(), height()) / 20;
    qreal unit = (batteryRect.width() - (margin * 2)) / 100;
    qreal width = currentValue * unit;
    QPointF topLeft(batteryRect.topLeft().x() + margin, batteryRect.topLeft().y() + margin);
    QPointF bottomRight(width + margin + 5, batteryRect.bottomRight().y() - margin);
    QRectF rect(topLeft, bottomRight);

    painter->setPen(Qt::NoPen);
    painter->setBrush(batteryGradient);
    painter->drawRoundedRect(rect, bgRadius, bgRadius);

    painter->restore();
}

void Battery::drawHead(QPainter *painter)
{
    painter->save();

    QPointF headRectTopLeft(batteryRect.topRight().x(), height() / 3);
    QPointF headRectBottomRight(width(), height() - height() / 3);
    QRectF headRect(headRectTopLeft, headRectBottomRight);

    QLinearGradient headRectGradient(headRect.topLeft(), headRect.bottomLeft());
    headRectGradient.setColorAt(0.0, borderColorStart);
    headRectGradient.setColorAt(1.0, borderColorEnd);

    painter->setPen(Qt::NoPen);
    painter->setBrush(headRectGradient);
    painter->drawRoundedRect(headRect, headRadius, headRadius);

    painter->restore();
}

void Battery::updateValue()
{
    if (isForward)
    {
        currentValue -= step;
        if (currentValue <= value)
            timer->stop();
    }
    else
    {
        currentValue += step;
        if (currentValue >= value)
            timer->stop();
    }
    this->update();
}

qreal Battery::getMinValue() const
{
    return this->minValue;
}

qreal Battery::getMaxValue() const
{
    return this->maxValue;
}

qreal Battery::getValue() const
{
    return this->value;
}

qreal Battery::getAlarmValue() const
{
    return this->alarmValue;
}

qreal Battery::getStep() const
{
    return this->step;
}

int Battery::getBorderRadius() const
{
    return this->borderRadius;
}

int Battery::getBgRadius() const
{
    return this->bgRadius;
}

int Battery::getHeadRadius() const
{
    return this->headRadius;
}

QColor Battery::getBorderColorStart() const
{
    return this->borderColorStart;
}

QColor Battery::getBorderColorEnd() const
{
    return this->borderColorEnd;
}

QColor Battery::getAlarmColorStart() const
{
    return this->alarmColorStart;
}

QColor Battery::getAlarmColorEnd() const
{
    return this->alarmColorEnd;
}

QColor Battery::getNormalColorStart() const
{
    return this->normalColorStart;
}

QColor Battery::getNormalColorEnd() const
{
    return this->normalColorEnd;
}

QSize Battery::sizeHint() const
{
    return QSize(150, 80);
}

QSize Battery::minimumSizeHint() const
{
    return QSize(30, 10);
}

void Battery::setRange(qreal minValue, qreal maxValue)
{
    //如果最小值大于或者等于最大值则不设置
    if (minValue >= maxValue)
        return;
    this->minValue = minValue;
    this->maxValue = maxValue;

    //如果目标值不在范围值内,则重新设置目标值
    //值小于最小值则取最小值,大于最大值则取最大值
    if (value < minValue)
        setValue(minValue);
     else if (value > maxValue)
        setValue(maxValue);
    this->update();
}

void Battery::setRange(int minValue, int maxValue)
{
    setRange((qreal)minValue, (qreal)maxValue);
}

void Battery::setMinValue(qreal minValue)
{
    setRange(minValue, maxValue);
}

void Battery::setMaxValue(qreal maxValue)
{
    setRange(minValue, maxValue);
}

void Battery::setValue(qreal value)
{
    //值和当前值一致则无需处理
    if (value == this->value)
        return;


    //值小于最小值则取最小值,大于最大值则取最大值
    if (value < minValue)
        value = minValue;
    else if (value > maxValue)
        value = maxValue;

    if (value > currentValue)
        isForward = false;
    else if (value < currentValue)
        isForward = true;
    else
        return;

    this->value = value;
    this->update();
    emit valueChanged(value);
    timer->start();
}

void Battery::setValue(int value)
{
    setValue((qreal)value);
}

void Battery::setAlarmValue(qreal alarmValue)
{
    if (this->alarmValue != alarmValue)
    {
        this->alarmValue = alarmValue;
        this->update();
    }
}

void Battery::setAlarmValue(int alarmValue)
{
    setAlarmValue((qreal)alarmValue);
}

void Battery::setStep(qreal step)
{
    if (this->step != step)
    {
        this->step = step;
        this->update();
    }
}

void Battery::setStep(int step)
{
    setStep((qreal)step);
}

void Battery::setBorderRadius(int borderRadius)
{
    if (this->borderRadius != borderRadius)
    {
        this->borderRadius = borderRadius;
        this->update();
    }
}

void Battery::setBgRadius(int bgRadius)
{
    if (this->bgRadius != bgRadius)
    {
        this->bgRadius = bgRadius;
        this->update();
    }
}

void Battery::setHeadRadius(int headRadius)
{
    if (this->headRadius != headRadius)
    {
        this->headRadius = headRadius;
        this->update();
    }
}

void Battery::setBorderColorStart(const QColor &borderColorStart)
{
    if (this->borderColorStart != borderColorStart)
    {
        this->borderColorStart = borderColorStart;
        this->update();
    }
}

void Battery::setBorderColorEnd(const QColor &borderColorEnd)
{
    if (this->borderColorEnd != borderColorEnd)
    {
        this->borderColorEnd = borderColorEnd;
        this->update();
    }
}

void Battery::setAlarmColorStart(const QColor &alarmColorStart)
{
    if (this->alarmColorStart != alarmColorStart)
    {
        this->alarmColorStart = alarmColorStart;
        this->update();
    }
}

void Battery::setAlarmColorEnd(const QColor &alarmColorEnd)
{
    if (this->alarmColorEnd != alarmColorEnd)
    {
        this->alarmColorEnd = alarmColorEnd;
        this->update();
    }
}

void Battery::setNormalColorStart(const QColor &normalColorStart)
{
    if (this->normalColorStart != normalColorStart)
    {
        this->normalColorStart = normalColorStart;
        this->update();
    }
}

void Battery::setNormalColorEnd(const QColor &normalColorEnd)
{
    if (this->normalColorEnd != normalColorEnd)
    {
        this->normalColorEnd = normalColorEnd;
        this->update();
    }
}

