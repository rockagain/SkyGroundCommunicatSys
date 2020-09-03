#ifndef STATUSTOSHOW_H
#define STATUSTOSHOW_H

#include <QWidget>
#include <QHBoxLayout>

namespace Ui {
class statusToShow;
}

class statusToShow : public QWidget
{
    Q_OBJECT

public:
    explicit statusToShow(QWidget *parent = 0);
    ~statusToShow();

public slots:
    //显示海事链接信息的
    void showMsattilateLinkStatusReport(QByteArray msg);
private:
    Ui::statusToShow *ui;
    //初始化函数
    void init();
    //下面都是转换函数 上面两个是bytearray to 数字的  下面俩是大小端转换的
    short ByteArrayToShort(QByteArray length_2);
    int ByteArrayToInt(QByteArray length_4);
    short BLEndianShort(short value);
    uint BLEndianInt(uint x);
};

#endif // STATUSTOSHOW_H
