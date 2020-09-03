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
    INSTRUCT_PORT = 50131;
    AudioDataToSend = constructSendByteArray();
    //set 发送指令的  Timer
    audioRecordTimer = new QTimer;
    audioRecordTimer->setInterval(5);
    connect(audioRecordTimer,&QTimer::timeout,this,&JiTongSocket::sendAudioRecord);

    instructSocket->bind(INSTRUCT_PORT);
    connect(instructSocket,&QUdpSocket::readyRead,this,&JiTongSocket::receiveInstructResponse);
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
        qDebug()<< "传到结尾了";
        QMessageBox::information(NULL, "Title", QStringLiteral("传到结尾了"));
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
    audioRecordInstuct.append(QByteArray::fromHex("AAFF"));
    audioRecordInstuct.append(QByteArray::fromHex("02"));
    //帧长度
    short i = 66;
    audioRecordInstuct.append(ShortToByte(i));
    // 左acp 还是 右acp
    audioRecordInstuct.append(QByteArray::fromHex("02"));
    //开始或者结束
    if(SEND_AUDIO_FLAG) {
        audioRecordInstuct.append(QByteArray::fromHex("01"));
    } else {
        audioRecordInstuct.append(QByteArray::fromHex("02"));
    }
    //电话16位
    audioRecordInstuct.append("ABCDEF1234567890");
    //话音信息 40   //从什么地方取数据，怎么样发过去
    if(SEND_AUDIO_FLAG) {
        QByteArray data = WhatDataToSend();
        if(WhatDataToSend().size() == 40) {
            audioRecordInstuct.append(data);
            qDebug()<<"传文件";
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
        QMessageBox::information(NULL, "Title", QStringLiteral("机通收到空信息"));
        return;
    }

    bool length = checkInstructLength(reply);
    bool checksum = checkCheckSumNum(reply);
    if(!(length && checksum)) {
        return;
    }

    if(reply.at(2) == 0x05) {
        //播放 话音回放！  目前做成保存的
        QString filename = reply.mid(5,40);
        SaveAudioBytes.append(reply.mid(45,40));
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
    if(reply.at(2) == 0x04) {
        //其中有电话号码，需要看其怎么应用
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
    if(reply.at(2) == 0x03) {
        //心跳   //是否为心跳假如不是心跳的话回复什么？
    }
}





//60秒不接电话(为了调试我这里设置了10秒)   JTCI-01   未接来电
void JiTongSocket::UnAnsweredPhoneInstruct(QString phoneNum){
    if(phoneNum == "") {
        phoneNum = "1234567890abcde";
    }
    QByteArray instruct;
    instruct.append(QByteArray::fromHex("AAFF"));
    instruct.append(QByteArray::fromHex("01"));
    short length =24;
    QByteArray bar = ShortToByte(length);
    instruct.append(bar);
    //电话号码
    instruct.append(phoneNum);
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
    char size = instruct.size();
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
}







