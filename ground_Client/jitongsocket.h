#ifndef JITONGSOCKET_H
#define JITONGSOCKET_H

#include <QObject>

#include <QUdpSocket>
#include <QHostAddress>
#include <QByteArray>
#include <QMessageBox>
#include <QTimer>
#include <QMovie>
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

    void clearSaveAudioBytes();

signals:

private:

    QByteArray AudioDataToSend;
    int INSTRUCT_PORT;
    QString SERVER_ADRESS;
    QUdpSocket* instructSocket;

    QTimer* audioRecordTimer;

    void init();
    void checkMsg(QByteArray reply);
    //用这个控制关闭
    bool SEND_AUDIO_FLAG;

    QByteArray ShortToByte(short num);
    QByteArray constructSendByteArray();
    QByteArray WhatDataToSend();
    void SaveReplayedAudioMsg(QByteArray msg,QString name);
    QFile* playedAudioFile;
    QFile* savedAudiofFile;
    QByteArray SaveAudioBytes;
    bool checkInstructLength(QByteArray instruct);
    bool checkCheckSumNum(QByteArray instruct);
    void AudioReplay();
    QAudioOutput* audioOutput;
public slots:

    void sendAudioRecord();
    //未接来电
    void UnAnsweredPhoneInstruct(QString phoneNum);
    void sendAudioRecordManager();
private slots:
    void receiveInstructResponse();
};

#endif // JITONGSOCKET_H
