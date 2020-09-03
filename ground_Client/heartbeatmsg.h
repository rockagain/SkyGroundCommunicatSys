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
    const char* instructHead;
    //char instructHead[5];
    QUdpSocket* heartbeatSocket;
    QTimer* sendheartbeatTimer;
    QTimer* DisLinkClock;
    QTimer* LinkClock;

    void init();
    //帧序号, 收到回复帧的时候，序号加1
    uchar frameNumber;
    void checkMsg(QByteArray instruct);

    int HEARTBEAT_PORT;
    QString SERVER_ADRESS;
    int isLinkedOrNot;
    int connectedCount;
    // 连接用函数
    void checkConnectCount();
    void setConnectCount();
    QString giveCurrentTime();
    //转换函数
    QByteArray ShortToByte(short num);
    ushort BLEndianUshort(ushort value);
signals:
    void sendSystemTimeSignal();
    void emitIfNetIsConnectedOrNot(bool connected);
public slots:

private slots:
    void receiveMsgAndcheck();
    void sendheartbeatMsg();
    void checkTimeIsOut();
};

#endif // HEARTBEATMSG_H
