#ifndef BATTERY_WARN_H
#define BATTERY_WARN_H

#include <QWidget>

namespace Ui {
class battery_warn;
}

class battery_warn : public QWidget
{
    Q_OBJECT

public:
    explicit battery_warn(QWidget *parent = nullptr);
    ~battery_warn();

private:
    Ui::battery_warn *ui;
};

#endif // BATTERY_WARN_H
