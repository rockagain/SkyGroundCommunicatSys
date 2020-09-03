﻿#include "heartbeatmsg.h"






HeartbeatMsg::HeartbeatMsg(QObject *parent) : QObject(parent)
{

    //设定islinkedOrNot时候 即触发断开和连接的时候应该通知，MainWindow模块， 即做不了各种动作了
    //之后增加   上线提示
    init();

}

//初始话socket 并且 调用Timer发送信息信号槽
void HeartbeatMsg::init()
{

    //初始化 帧数字
    frameNumber = 0;

    //读取settings
    QSettings* settings = new QSettings(QCoreApplication::applicationDirPath()+"/ground_config.ini", QSettings::IniFormat);
    //qDebug()<<QCoreApplication::applicationDirPath();
    //settings->setValue("SIDE/CLIENTSIDE","LEFT");



    dc_side = Which_Client::GROUND;


    if(dc_side == Which_Client::LEFT) {
        instructHead = "AA87";
        QVariant var = settings->value("ADRESS/AIR_SERVER_ADRESS");
        SERVER_ADRESS = var.toString();
        HEARTBEAT_PORT = 50100;
    }else if(dc_side == Which_Client::RIGHT) {
        instructHead = "AA88";
        QVariant var = settings->value("ADRESS/AIR_SERVER_ADRESS");
        SERVER_ADRESS = var.toString();
        HEARTBEAT_PORT = 50110;
    }else if(dc_side == Which_Client::GROUND) {
        instructHead = "AA55";
        QVariant var = settings->value("ADRESS/LAND_SERVER_ADRESS");
        SERVER_ADRESS = var.toString();
        QVariant port = settings->value("LAND_PORT/HEARTBEAT_PORT");  //但是这一功能可能会在别的类内编写
        HEARTBEAT_PORT = port.toInt();  //读取配置
    }


    connectedCount = 0;

    heartbeatSocket = new QUdpSocket();
    bool isBinding =   heartbeatSocket->bind(HEARTBEAT_PORT);
    connect(heartbeatSocket,&QUdpSocket::readyRead,this,&HeartbeatMsg::receiveMsgAndcheck);


    sendheartbeatTimer = new QTimer();
    sendheartbeatTimer->setInterval(1000); //这个时间之后也通过配置爱设定之后在config.ini里面写入
    sendheartbeatTimer->start();
    connect(sendheartbeatTimer,&QTimer::timeout,this,&HeartbeatMsg::sendheartbeatMsg);
    sendheartbeatMsg();  //不要等到计时就开始，从这一刻就发送

    LinkClock = new QTimer();
    LinkClock->setInterval(3600);
    LinkClock->start();
    connect(LinkClock,&QTimer::timeout,this,&HeartbeatMsg::checkConnectCount);


    DisLinkClock = new QTimer();
    DisLinkClock->setInterval(3000);
    DisLinkClock->start();
    //没收到CI的回复帧，就把 isCIokOrNot 重新start 相当于重新计时，否则触发checkTimeIsOut，弹出警示，设定flag 把全局都改变
    //或者整个效果， 红灯路灯， 表示连接与否
    connect(DisLinkClock,&QTimer::timeout,this,&HeartbeatMsg::checkTimeIsOut);
}



//周期被调用  发送心跳信息 据说叫心跳查询帧，  也不知道和 心跳帧有啥区别
void HeartbeatMsg::sendheartbeatMsg(){
    //RDCCI-01
    QByteArray heartbeat; //帧头
    heartbeat.append(QByteArray::fromHex(instructHead));
    //帧标识
    heartbeat.append(QByteArray::fromHex("01"));
    //长度
    short length = 7;
    length = BLEndianUshort(length);
    QByteArray lengthByte = ShortToByte(length);
    // 把长度嵌进去
    heartbeat.append(lengthByte);
    //飞机编号 先随便编一个

    heartbeat.append(frameNumber);
    //校验和
    char sum = 0;
    for(char i:heartbeat.mid(2,heartbeat.size() -2 )) {
        sum += i;
    }
    heartbeat.append(sum);
    heartbeatSocket->writeDatagram(heartbeat,QHostAddress(SERVER_ADRESS),HEARTBEAT_PORT);
    static int i = 0;
    QString time = giveCurrentTime();
    qDebug() << "DM_HEARTBEAT : " +  heartbeat.toHex().toUpper() + QStringLiteral("  时间")+ time;
}

//接收到回复然后做出相应措施 CIDC-01收到这个玩意后， 检查它 然后做出相应反应 (发送授时帧)
void HeartbeatMsg::receiveMsgAndcheck()
{
    //反馈回来的 指令
    QByteArray ba;
    while(heartbeatSocket->hasPendingDatagrams())
    {
        ba.resize(heartbeatSocket->pendingDatagramSize());
        heartbeatSocket->readDatagram(ba.data(), ba.size());
        checkMsg(ba);   //试一试看OK不  //检查收到的内容
    }

}

//CIDC-01 接收的为 机载控制的自检状态，接收了之后可能需要显示出来
void HeartbeatMsg::checkMsg(QByteArray instruct)
{
    if(instruct == "") {
        qDebug()<<QStringLiteral("空指令2，端口不正确或未连接");
        return;
    }

    if(instruct.at(2) == 0x01) { //此为心跳帧，收到后回复授时帧
        //收到时间，帧号，及设备状态 假如设备有问题， 提出警告 //这个警告可能需要5分钟一提醒等等，需要个flag
        //收到了，isCIokOrNot 重新计时, start了，应该算重新计时了
        QString time = giveCurrentTime();
        qDebug()<< "GCSCI_HEARTBEAT : " + instruct.toHex().toUpper() + QStringLiteral("  时间") +time;
        setConnectCount();

        DisLinkClock->start();
        //第一次接收到信息
        static bool firstTime = true;
        if(firstTime == true) {
            //发送signal， 激发系统授时，同时关闭flag
            //sendSystemTimeSignal();  //地面端不授时
            firstTime = false;
        }
        frameNumber++;
    }

    //qDebug()<<"断点2";
}



void HeartbeatMsg::checkTimeIsOut()
{
    //    QMessageBox messagebox(QMessageBox::NoIcon,QStringLiteral("提示"),QStringLiteral("<br><br><b>失去控制端心跳<b><br><br>"));
    //    messagebox.setStandardButtons(QMessageBox::Yes);
    //    messagebox.setButtonText(QMessageBox::Yes,QStringLiteral("确认"));
    //    messagebox.setWindowIcon(QIcon(":/plane.png"));
    //    QSpacerItem* horizontalSpacer = new QSpacerItem(230, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    //    QGridLayout* layout = (QGridLayout*)messagebox.layout();
    //    layout->addItem(horizontalSpacer, layout->rowCount(), 0, 1, layout->columnCount());
    //    messagebox.exec();

    //测心跳
    DisLinkClock->stop();  //注释了该函数就能持续报警   //或者改为警示灯的形式
    isLinkedOrNot = NET_IS_NOT_CONNECTED;
    emit emitIfNetIsConnectedOrNot(NET_IS_NOT_CONNECTED);
    //当显控左/右持续三个周期没有收到通信控制设备的心跳响应帧，就显示断开
}

void HeartbeatMsg::checkConnectCount(){
    if(connectedCount < 3) {
        connectedCount = 0;
    }
}


void HeartbeatMsg::setConnectCount(){
    connectedCount ++;
    //在规定时间内 不到三就不算连接上
    if(connectedCount == 3) {
        connectedCount = 0;
        isLinkedOrNot = NET_IS_CONNECTED;
        emit emitIfNetIsConnectedOrNot(NET_IS_CONNECTED);
    }
}



QString HeartbeatMsg::giveCurrentTime()
{
    QTime time = QDateTime::currentDateTime().time();
    QString hour = QString::number(time.hour());
    QString min = QString::number(time.minute());
    QString second = QString::number(time.second());
    if(hour.size() == 1) {
        hour = "0" + hour;
    }
    if(min.size() == 1) {
        min = "0" + min;
    }
    if(second.size() == 1) {
        second = "0" + second;
    }
    QString entireTime = hour + QStringLiteral("点") + min  + QStringLiteral("分")  + second  + QStringLiteral("秒");
    return entireTime;
}



//short 转换大小端
ushort HeartbeatMsg::BLEndianUshort(ushort value)
{
    return ((value & 0x00FF) << 8 ) | ((value & 0xFF00) >> 8);
}




//uint HeartbeatMsg::bswap_32(uint x)
//{
//    return (((uint)(x) & 0xff000000) >> 24) |  (((uint)(x) & 0x00ff0000) >> 8) | \
//            (((uint)(x) & 0x0000ff00) << 8) | (((uint)(x) & 0x000000ff) << 24);
//}


//和机器相关,可能还得判断机器的大小端

//QByteArray  HeartbeatMsg::intToByte(int i)
//{
//    QByteArray abyte0;
//    abyte0.resize(4);
//    abyte0[0] = (uchar)  (0x000000ff & i);
//    abyte0[1] = (uchar) ((0x0000ff00 & i) >> 8);
//    abyte0[2] = (uchar) ((0x00ff0000 & i) >> 16);
//    abyte0[3] = (uchar) ((0xff000000 & i) >> 24);
//    return abyte0;
//}



QByteArray HeartbeatMsg::ShortToByte(short num){
    QByteArray bar;
    bar.resize(2);
    memcpy(bar.data(),&num,sizeof(num));
    return bar;
}












