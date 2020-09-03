#include "jitongsocket.h"



JiTongSocket::JiTongSocket(QObject *parent) : QObject(parent)
{
    init();
}


void JiTongSocket::init()
{
    //读取配置
    QSettings* settings = new QSettings(QCoreApplication::applicationDirPath()+"/jt_config.ini", QSettings::IniFormat);
    QVariant var = settings->value("ADRESS/SERVER_ADRESS");
    SERVER_ADRESS  = var.toString();
    //参数
    instructSocket = new QUdpSocket;
    INSTRUCT_PORT = 50130;
    AudioDataToSend = constructSendByteArray();
    //set 发送指令的  Timer
    audioRecordTimer = new QTimer;
    audioRecordTimer->setInterval(5);
    connect(audioRecordTimer,&QTimer::timeout,this,&JiTongSocket::sendAudioRecord);

    instructSocket->bind(8041);
    connect(instructSocket,&QUdpSocket::readyRead,this,&JiTongSocket::receiveInstructResponse);

    //机通心跳
    //发送心跳
    heartbeatTimer = new QTimer;
    heartbeatTimer->setInterval(2000);
    JiTongHeartBeat();
    heartbeatTimer->start();
    connect(heartbeatTimer,&QTimer::timeout,this,&JiTongSocket::JiTongHeartBeat);

    //判断 语音回放帧 是否结束 的 Timer
    audioReplayTimer = new QTimer;
    audioReplayTimer->setInterval(1000);
    //一秒的时间还没有信息传过来，那么就直接保存播放
    connect(audioReplayTimer,&QTimer::timeout,this,&JiTongSocket::audioReplayMsgIsOver);

}


void JiTongSocket::audioReplayMsgIsOver()
{
    QMessageBox::information(NULL, "Title", QStringLiteral("语音回放信息已传输完毕"));
    int i = SaveAudioBytes.size();
    SaveReplayedAudioMsg(SaveAudioBytes,"abc.pcm");
    //清空数据
    SaveAudioBytes.clear();

}



//JTCI-00
void JiTongSocket::JiTongHeartBeat()
{
    QByteArray heartbeat;
    heartbeat.append(QByteArray::fromHex("FFAA"));
    heartbeat.append(QByteArray::fromHex("00"));
    //帧长度   17 - 7  = 10   即  0A
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
    instructSocket->writeDatagram(heartbeat,QHostAddress(SERVER_ADRESS),INSTRUCT_PORT);
    QString time = giveCurrentTime();
    qDebug()<< "JT_HEARTBEAT : " + heartbeat.toHex().toUpper() + QStringLiteral("  时间") +time;
}


QByteArray JiTongSocket::constructSendByteArray(){
    QString FileName = QCoreApplication::applicationDirPath() + "/testFile/sendAudio.pcm";
    QFile* audiofile = new  QFile;
    audiofile->setFileName(FileName);
    audiofile->open(QIODevice::ReadOnly);
    QByteArray bar;
    bar = audiofile->readAll();
    return bar;
}

QByteArray JiTongSocket::WhatDataToSend(){
    QByteArray data = AudioDataToSend;
    static int count = 0;
    //执行这一段等于结束了
    if((count + 40) >data.size()) {
        count = 0 ;
        qDebug()<< QStringLiteral("传到结尾了");
        QMessageBox::information(NULL, "Title", QStringLiteral("机通发送音频数据完成！"));
        SEND_AUDIO_FLAG = false;
        return  "n";
    }

    QByteArray dataToSend = data.mid(count,40);
    for(int i = 0;i <= 39; i++) {
        count ++ ;
    }
    return dataToSend;
}

//控制发送音频的
void JiTongSocket::sendAudioRecordManager()
{
    audioRecordTimer->start();
    SEND_AUDIO_FLAG = true;
}

//JTCI-02  每5ms向通信控制设备打包发送EN-JTCI-02，连发3包EN-JTCI-02“音频记录”结束标志位 //这个叫音频记录，但是其实是
//发送音频 接了电话就播放这个   EN-JTCI-02
void JiTongSocket::sendAudioRecord()
{

    //SEND_AUDIO_FLAG  点击后把这个设置为 真
    if(SEND_AUDIO_FLAG == false) {
        static int count = 0;
        count ++;
        if(count == 4){
            count = 0;
            audioRecordTimer->stop(); //计数三次后停止
        }
    }
    QByteArray audioRecordInstuct;
    audioRecordInstuct.append(QByteArray::fromHex("FFAA"));
    audioRecordInstuct.append(QByteArray::fromHex("02"));
    //帧长度 65 - 7 = 58
    audioRecordInstuct.append(QByteArray::fromHex("3A"));
    // 左acp 还是 右acp
    audioRecordInstuct.append(QByteArray::fromHex("02"));
    //开始或者结束
    if(SEND_AUDIO_FLAG) {
        audioRecordInstuct.append(QByteArray::fromHex("01"));
    } else {
        audioRecordInstuct.append(QByteArray::fromHex("02"));
    }
    //电话16位
    audioRecordInstuct.append(this->comingPhoneNumber);
    //话音信息 40   //从什么地方取数据，怎么样发过去
    if(SEND_AUDIO_FLAG) {
        QByteArray data = WhatDataToSend();
        if(WhatDataToSend().size() == 40) {
            audioRecordInstuct.append(data);
            qDebug()<<QStringLiteral("传文件");
        }
    } else {
        QByteArray bar = "0000000000000000000000000000000000000000";
        audioRecordInstuct.append(bar);
    }

    //校验和
    char sum = 0;
    for(char i: audioRecordInstuct.mid(2,audioRecordInstuct.size() - 2)) {
        sum += i;
    }
    audioRecordInstuct.append(sum);
    //帧尾
    audioRecordInstuct.append(QByteArray::fromHex("ABAB"));
    //发送 用udp发送
    instructSocket->writeDatagram(audioRecordInstuct,QHostAddress(SERVER_ADRESS),INSTRUCT_PORT);
}

//接收指令
void JiTongSocket::receiveInstructResponse()
{
    //反馈回来的 指令     //这里先啥也不做
    QByteArray ba;
    while(instructSocket->hasPendingDatagrams())
    {
        ba.resize(instructSocket->pendingDatagramSize());
        instructSocket->readDatagram(ba.data(), ba.size());
        checkMsg(ba);   //试一试看OK不  //检查收到的内容
    }

}


//语音回放这里  语音回放
void JiTongSocket::SaveReplayedAudioMsg(QByteArray msg,QString fileName)
{
    playedAudioFile = new QFile();
    savedAudiofFile = new QFile();
    playedAudioFile->setFileName(QCoreApplication::applicationDirPath() + "/testFile/savedAudio.pcm");//设置其实设置音频文件的存放路径(输入音频名及存放路径)
    savedAudiofFile->setFileName(QCoreApplication::applicationDirPath() + "/testFile/" + fileName);
    bool is_open = playedAudioFile->open(QIODevice::ReadWrite | QIODevice::Truncate);
    bool isopen = savedAudiofFile->open(QIODevice::ReadWrite | QIODevice::Truncate);
    if(!is_open)
    {
        QMessageBox::information(NULL, "Title", QStringLiteral("文件打开失败"));
    }
    if(!isopen)
    {
        QMessageBox::information(NULL, "Title", QStringLiteral("文件打开失败"));
    }

    playedAudioFile->write(msg);
    playedAudioFile->close();
    savedAudiofFile->write(msg);
    savedAudiofFile->close();
    if(fileName.split(".")[1] == "pcm") {
        AudioReplay();
    }
}

void JiTongSocket::clearSaveAudioBytes()
{
    SaveAudioBytes.clear();
}


void JiTongSocket::checkMsg(QByteArray reply){

    if(reply.size() == 0) {
        qDebug()<< QStringLiteral("空指令3，机通接收数据不正确");
        return;
    }

    bool length = checkInstructLength(reply);
    bool checksum = checkCheckSumNum(reply);
    if(!(length && checksum)) {
        return;
    }

    if(reply.at(2) == 0x03) {
        //心跳   //是否为心跳假如不是心跳的话回复什么？
    }

    if(reply.at(2) == 0x04) {
        //其中有电话号码，需要看其怎么应用
        qDebug() << QStringLiteral("机通收到信息: ")  +  reply.toHex().toUpper();
        if(reply.at(4) == 1) {
            //打开音频  EN-JTCI-02 //ffmpeg? 还是发送指令，发送指令的内容怎么取
            SEND_AUDIO_FLAG = true;
            sendAudioRecord();
            audioRecordTimer->start();
        } else if(reply.at(4) == 2) {
            SEND_AUDIO_FLAG = false;
            //通话结束  EN-JTCI-02  //执行三次停止键，然后突出任务 //主动挂机也显示这个！
        }
    }

    if(reply.at(2) == 0x05) {
        //播放 话音回放！  目前做成保存的 ，一秒之后要是没有再次收到信息 ，那就保存播放
        audioReplayTimer->start();

        QString filename = "abc.pcm";
        SaveAudioBytes.append(reply.mid(6,800));
        static int a = 0;
        a++;
        qDebug()<< "------------------------" + QString::number(a);
        if(reply.at(4) == 0x03) {
            QMessageBox::information(NULL, "Title", QStringLiteral("语音回放信息已传输完毕"));
            int i = SaveAudioBytes.size();
            SaveReplayedAudioMsg(SaveAudioBytes,filename);
            //清空数据
            SaveAudioBytes.clear();
            a = 0;
        }

    }

    if(reply.at(2) == 0x06) {
        bool ok = reply.at(4);
        emit phoneOrNot(!ok);     //反了没有
    }
}





//JTCI-01   未接来电
void JiTongSocket::UnAnsweredPhoneInstruct(QString phoneNum){
    if(phoneNum == "") {
        phoneNum = "1234567891234567";
    }
    QByteArray instruct;
    instruct.append(QByteArray::fromHex("FFAA"));
    instruct.append(QByteArray::fromHex("01"));
    //帧长度 24-7=17
    instruct.append(QByteArray::fromHex("11"));
    //来电超时
    instruct.append(QByteArray::fromHex("12"));
    //电话号码
    instruct.append(phoneNum);    //这个地方需要改，改为 数字

    char sum = 0;
    //校验和
    for(char i: instruct.mid(2,instruct.size() - 2)) {
        sum += i;
    }
    instruct.append(sum);
    instruct.append(QByteArray::fromHex("ABAB"));
    instructSocket->writeDatagram(instruct,QHostAddress(SERVER_ADRESS),INSTRUCT_PORT);
}


void JiTongSocket::AnsweredPhoneInstruct(QString phoneNum){
    if(phoneNum == "") {
        phoneNum = "1234567891234567";
    }
    this->comingPhoneNumber = phoneNum;
    QByteArray instruct;
    instruct.append(QByteArray::fromHex("FFAA"));
    instruct.append(QByteArray::fromHex("01"));
    //帧长度 24-7=17
    instruct.append(QByteArray::fromHex("11"));
    //来电未超时
    instruct.append(QByteArray::fromHex("11"));
    //电话号码
    instruct.append(phoneNum);    //这个地方需要改，改为 数字

    char sum = 0;
    //校验和
    for(char i: instruct.mid(2,instruct.size() - 2)) {
        sum += i;
    }
    instruct.append(sum);
    instruct.append(QByteArray::fromHex("ABAB"));
    instructSocket->writeDatagram(instruct,QHostAddress(SERVER_ADRESS),INSTRUCT_PORT);
}





QByteArray JiTongSocket::ShortToByte(short num){
    QByteArray bar;
    bar.resize(2);
    memcpy(bar.data(),&num,sizeof(num));
    return bar;
}


//检测指令长度
bool JiTongSocket::checkInstructLength(QByteArray instruct)
{
    char size = instruct.size() - 7 ;
    char length = instruct.at(3);

    if(size != length) {
        QMessageBox::information(NULL, "Title", QStringLiteral("机通指令长度报错"));
        return false;
    } else {
        return true;
    }

}

bool JiTongSocket::checkCheckSumNum(QByteArray instruct)
{
    char sum = 0;
    //校验和
    for(char i: instruct.mid(2,instruct.size() - 5)) {
        sum += i;
    }
    if(sum != instruct.at(instruct.size() -3)){
        QMessageBox::information(NULL, "Title", QStringLiteral("机通指令校验和报错"));
        return false;
    }else {
        return true;
    }
}


void JiTongSocket::AudioReplay()
{
    QString audioFileName = QCoreApplication::applicationDirPath() + "/testFile/savedAudio.pcm";
    QFile* audiofile = new  QFile;
    audiofile->setFileName(audioFileName);
    bool isopen = audiofile->open(QIODevice::ReadOnly);
    if(!isopen) {
        QMessageBox::information(NULL, "Title", QStringLiteral("打开文件失败，没有收到该文件"));
        return;
    }
    QAudioFormat format;
    format.setSampleRate(8000);
    format.setChannelCount(1);
    format.setSampleSize(8);   //从8位 mlaw转化为16的可以
    format.setCodec("audio/pcm");
    //format.setCodec("audio/G711-0");//设置编解码器
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::UnSignedInt);
    audioOutput = new QAudioOutput(format);
    audioOutput->start(audiofile);
    //这里可以建立信号和槽，添加关闭动作
}


//给心跳提供时间的
QString JiTongSocket::giveCurrentTime()
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








