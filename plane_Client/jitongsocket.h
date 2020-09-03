#ifndef JITONGSOCKET_H
#define JITONGSOCKET_H

#include <QObject>

#include <QUdpSocket>
#include <QHostAddress>
#include <QByteArray>
#include <QMessageBox>
#include <QTimer>
#include <QCoreApplication>
#include <QLabel>
#include <QTextCodec>
#include <QDate>
#include <QDateTime>
#include <QSettings>
#include <QFile>
#include <windows.h>
#include <QAudioOutput>
#include <QAudioFormat>

class JiTongSocket : public QObject
{
    Q_OBJECT

public:
    explicit JiTongSocket(QObject *parent = nullptr);

    //清除音频信息
    void clearSaveAudioBytes();

private:
    //需要被发送的data
    QByteArray AudioDataToSend;
    //端口号
    int INSTRUCT_PORT;
    //java程序的机通端口
    QString SERVER_ADRESS;
    //指令的udp， 管 接收和发送
    QUdpSocket* instructSocket;
    //发送信息的timer
    QTimer* audioRecordTimer;
    QTimer* heartbeatTimer;
    QTimer* audioReplayTimer;
    QString comingPhoneNumber;
    //初始化函数
    void init();
    //检查 来的信息的
    void checkMsg(QByteArray reply);
    //用这个控制关闭音频的
    bool SEND_AUDIO_FLAG;
    //转换函数
    QByteArray ShortToByte(short num);
    //把要发送的音频给组织起来
    QByteArray constructSendByteArray();
    //发送音频的， 分段的函数
    QByteArray WhatDataToSend();
    //语音回放
    void SaveReplayedAudioMsg(QByteArray msg,QString name);
    //
    QFile* playedAudioFile;
    //
    QFile* savedAudiofFile;
    //
    QByteArray SaveAudioBytes;
    //如名
    bool checkInstructLength(QByteArray instruct);
    //如名
    bool checkCheckSumNum(QByteArray instruct);
    //
    void AudioReplay();
    //
    QAudioOutput* audioOutput;
    //给时间
    QString giveCurrentTime();
public slots:
    //发送audio纪录
    void sendAudioRecord();
    void sendAudioRecordManager();
    //未接来电  和 来电
    void UnAnsweredPhoneInstruct(QString phoneNum);
    //来电
    void AnsweredPhoneInstruct(QString phoneNum);
private slots:
    //接收udp信息的
    void receiveInstructResponse();
    //机通的心跳信息
    void JiTongHeartBeat();
    // 语音回放结束后的弹窗
    void audioReplayMsgIsOver();
signals:
    //这里可能需要仔细看一下
    void phoneOrNot(bool);
};

#endif // JITONGSOCKET_H
