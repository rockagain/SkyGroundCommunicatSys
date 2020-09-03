#include "jitongheartbeat.h"

JiTongHeartbeat::JiTongHeartbeat(QObject *parent) : QObject(parent)
{
    init();
}



void JiTongHeartbeat::init()
{
    //读取配置
    QSettings* settings = new QSettings(QCoreApplication::applicationDirPath()+"/jt_config.ini", QSettings::IniFormat);
    QVariant var = settings->value("ADRESS/SERVER_ADRESS");
    SERVER_ADRESS  = var.toString();
    //参数
    HEARTBEAT_PORT = 50130;
    heartbeatSocket = new QUdpSocket;
    heartbeatSocket->bind(HEARTBEAT_PORT);

    //发送心跳
    heartbeatTimer = new QTimer;
    heartbeatTimer->setInterval(2000);
    JiTongHeartBeat();
    heartbeatTimer->start();
    connect(heartbeatTimer,&QTimer::timeout,this,&JiTongHeartbeat::JiTongHeartBeat);
    //读取udp信息
    connect(heartbeatSocket,&QUdpSocket::readyRead,this,&JiTongHeartbeat::heartbeatPortReceived);
}


//JTCI-00
void JiTongHeartbeat::JiTongHeartBeat()
{
    QByteArray heartbeat;
    heartbeat.append(QByteArray::fromHex("FFAA"));
    heartbeat.append(QByteArray::fromHex("00"));
    //帧长度
    heartbeat.append(QByteArray::fromHex("0A"));
    //版本号
    short code = 2;
    QByteArray lengthByte = ShortToByte(code);
    heartbeat.append(lengthByte);
    code = 19;
    lengthByte = ShortToByte(code);
    heartbeat.append(lengthByte);
    //装订时间
    heartbeat.append(QByteArray::fromHex("14"));
    heartbeat.append(QByteArray::fromHex("14"));
    heartbeat.append(QByteArray::fromHex("05"));
    heartbeat.append(QByteArray::fromHex("10"));
    //机器状态
    code = 0;
    heartbeat.append(ShortToByte(code));

    char sum = 0;
    //校验和
    for(char i: heartbeat.mid(2,heartbeat.size() - 2)) {
        sum += i;
    }
    heartbeat.append(sum);
    heartbeat.append(QByteArray::fromHex("ABAB"));
    //发送 用udp发送
    heartbeatSocket->writeDatagram(heartbeat,QHostAddress(SERVER_ADRESS),HEARTBEAT_PORT);
    QString time = giveCurrentTime();
    qDebug()<< "JT_HEARTBEAT : " + heartbeat.toHex().toUpper() + QStringLiteral("  时间") +time;
}


void JiTongHeartbeat::heartbeatPortReceived()
{
    //反馈回来的 指令     //这里先啥也不做
    QByteArray ba;
    while(heartbeatSocket->hasPendingDatagrams())
    {
        ba.resize(heartbeatSocket->pendingDatagramSize());
        heartbeatSocket->readDatagram(ba.data(), ba.size());
        checkMsg(ba);   //试一试看OK不  //检查收到的内容
    }

}

void JiTongHeartbeat::checkMsg(QByteArray reply){

}





QByteArray JiTongHeartbeat::ShortToByte(short num){
    QByteArray bar;
    bar.resize(2);
    memcpy(bar.data(),&num,sizeof(num));
    return bar;
}


QString JiTongHeartbeat::giveCurrentTime()
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

























