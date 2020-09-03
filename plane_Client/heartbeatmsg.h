#ifndef HEARTBEATMSG_H
#define HEARTBEATMSG_H

#include <QObject>
#include <QSettings>
#include <QUdpSocket>
#include <QTimer>
#include <QCoreApplication>
#include <QDateTime>
#include <QMessageBox>
#include <QIcon>
#include <QMessageBox>
#include <QGridLayout>
#include <QSpacerItem>


#include "config.h"

class HeartbeatMsg : public QObject
{
    Q_OBJECT
public:

    explicit HeartbeatMsg(QObject *parent = nullptr);

private:

    //设定是什么客户端
    short dc_side;
    //帧头
    const char* instructHead;
    //心跳udp
    QUdpSocket* heartbeatSocket;
    //控制心跳的timer
    QTimer* sendheartbeatTimer;
    //检测未链接的timer
    QTimer* DisLinkClock;
    //检测链接的timer
    QTimer* LinkClock;
    //初始化的函数
    void init();
    //帧序号, 收到回复帧的时候，序号加1
    uchar frameNumber;
    //查看到底是来的什么信息
    void checkMsg(QByteArray instruct);
    //心跳端口
    int HEARTBEAT_PORT;
    //java端程序的  IP
    QString SERVER_ADRESS;
    //如名
    int isLinkedOrNot;
    //为判断是否链接的 count
    int connectedCount;
    // 连接用函数
    void checkConnectCount();
    //设置  count 的函数
    void setConnectCount();
    //如名
    QString giveCurrentTime();
    //转换函数
    QByteArray ShortToByte(short num);
    //short的大小端转换
    ushort BLEndianUshort(ushort value);
    //转换函数
    short ByteArrayToShort(QByteArray _2);

signals:

    //发送系统授时
    void sendSystemTimeSignal();
    //发送是否连接的信号，再接电话那里显示呀
    void emitIfNetIsConnectedOrNot(bool connected);

private slots:

    //封装的  接收udp的功能
    void receiveMsgAndcheck();
    //发送心跳
    void sendheartbeatMsg();
    //看看时间是不是不OK了
    void checkTimeIsOut();
};

#endif // HEARTBEATMSG_H
