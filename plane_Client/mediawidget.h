#ifndef MEDIAWIDGET_H
#define MEDIAWIDGET_H

#include <QWidget>
#include <QDebug>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>

namespace Ui {
class MediaWidget;
}

class MediaWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MediaWidget(QWidget *parent = 0);
    ~MediaWidget();

private:
    Ui::MediaWidget *ui;
    QString bytearrayToQString(QByteArray num);
public:
    //开始电话的申请按钮
    QPushButton* VedioPhoneCall;
    //结束电话的申请按钮
    QPushButton* stopVideo;
    //电话号
    QLineEdit* phoneNumber;
    //显示卫通信息的
    QLabel* phaneLabel;
    //电话后面的倒计时
    QLabel* phoneCallCount;
    //显示是否连接
    QLabel* showConnectedOrNot;
public slots:
    //显示未接来电
    void showUnAnsweredPhone(QByteArray mun);
};

#endif // MEDIAWIDGET_H
