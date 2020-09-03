#ifndef SENDDATAWIDGET_H
#define SENDDATAWIDGET_H

#include <QWidget>
#include <QDebug>
#include <QButtonGroup>

namespace Ui {
class sendDataWidget;
}

class sendDataWidget : public QWidget
{
    Q_OBJECT

public:
    explicit sendDataWidget(QWidget *parent = 0);
    ~sendDataWidget();

private:
    Ui::sendDataWidget *ui;
};

#endif // SENDDATAWIDGET_H
