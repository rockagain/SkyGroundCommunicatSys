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
    //切换到 电话界面
    void ChangetoMediaWidget();
    //切换到 短信界面
    void ChangetoDataWidget();
    //切换到 周期性数据界面
    void ChangetoSendDataWidget();
    //切换到海事web那里的界面
    void ChangetoMSatelliteWidget();
    //about
    void ChangetoAboutWidget();
    //发送关闭信息
    void sendCloseSignal();
    //如名
    void showTheABout();
    //切换到429模块
    void Changeto429Widget();
    //切换到海事模块
    void ChangetoHaishiWidget();
private:
    Ui::MenuWidget *ui;
signals:
    //发送切换widget的信号
    void changeWidgetSignals(int order);
    //如名
    void shutDownTheSystem();
    //如名
    void init429Signals();
protected:
    //染色
    void paintEvent(QPaintEvent *event);
};

#endif // MENUWIDGET_H
