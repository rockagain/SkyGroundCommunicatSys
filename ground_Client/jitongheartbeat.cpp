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
    //SERVER_ADRESS = "";
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
    heartbeat.append(QByteArray::fromHex("AAFF"));
    heartbeat.append(QByteArray::fromHex("00"));
    //帧长度
    short length =14;
    QByteArray bar = ShortToByte(length);
    heartbeat.append(bar);
    //软件版本
    heartbeat.append("0103");
    //两位，看硬件信息
    heartbeat.append("ab");
    char sum = 0;
    //校验和
    for(char i: heartbeat.mid(2,heartbeat.size() - 2)) {
        sum += i;
    }
    heartbeat.append(sum);
    heartbeat.append(QByteArray::fromHex("ABAB"));
    //发送 用udp发送
    heartbeatSocket->writeDatagram(heartbeat,QHostAddress(SERVER_ADRESS),HEARTBEAT_PORT);
    qDebug()<< "JT_HEARTBEAT : " + heartbeat.toHex().toUpper();
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


























