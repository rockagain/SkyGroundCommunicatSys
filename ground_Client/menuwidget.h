#ifndef MENUWIDGET_H
#define MENUWIDGET_H

#include <QWidget>
#include <QDebug>
#include <QPainter>
#include <QMessageBox>
#include <QSpacerItem>
#include <QGridLayout>

namespace Ui {
class MenuWidget;
}

class MenuWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MenuWidget(QWidget *parent = 0);
    ~MenuWidget();

private slots:
    void ChangetoMediaWidget();
    void ChangetoDataWidget();
    void ChangetoSendDataWidget();

    void ChangetoMSatelliteWidget();
    void ChangetoAboutWidget();
    void showTheABout();
private:
    Ui::MenuWidget *ui;

signals:
    void changeWidgetSignals(int order);
protected:
    void paintEvent(QPaintEvent *event);
};

#endif // MENUWIDGET_H
