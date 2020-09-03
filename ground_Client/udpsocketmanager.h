#ifndef TCPSOCKETMANAGER_H
#define TCPSOCKETMANAGER_H

#include <QObject>

//#include <QTcpServer>
//#include <QTcpSocket>
#include <QDataStream>
#include <QDir>
#include <QFileDialog>
#include <QCryptographicHash>
#include <QFileInfo>
#include <QUdpSocket>
#include <QTextCodec>
#include <QTimer>
#include <QCoreApplication>
#include <QtEndian>

#include <QSettings>


#include "config.h"

class UdpSocketManager : public QObject
{
    Q_OBJECT
public:
    explicit UdpSocketManager(QObject *parent = nullptr);



public slots:
    //这个函数的意思是说，发送文件申请，算帧数，发送文件
    void selectFileAndSendFileInfo();

private slots:
    void sendDataInstruct();

    void controlInstructReceive();

    void dataInstructionReceive();

    void controlInstructSender();
private:

    QUdpSocket* dataPortSocket;  //负责上传数据 ，  或者接收数据
    QUdpSocket* controlPortSocket;   //负责调整状态

    bool IsNetlinked;  //之后用作网络是否连接判断

    //记录时间的 Timer 之后在根据是否只有一个来进行设置   //之后也可以用来操作心跳这样！
    QTimer* dataSenderTimer;  //rendTimer  和 receiveTimer 都是发送数据频率的

    //收信息放的 Timer
    QTimer* controlSenderTimer;

    //过了3秒没反应 给反应，  过了10秒没反应 给反应  记这个时的！
    QTimer* timeCounterTimer;    //可能需要加几个flag了  有网络连接的  有是否接到回复的，  有各种通话状态的

    //当文件发送方连续两次没有收到文件接收方回复的“文件接收状态”帧，则认为文件传输失败，上报文件传输失败// 记录失败的次数
    int reSendTime;

    //有视频通话模式  //语音通话模式  及  自由模式   //三种模式的  发送方式不一样，  接收模式也不一样
    int phoneCallMode;

    CurrentExchangedFile* currentUploadFile;

    CurrentExchangedFile* currentRecivedFile;
    //转化int 为 byte array
    QByteArray intToByte(int i);
    //发送
    void sendFileDescriptionInstruct();

    ushort currentSendFrame;   //这个作为外面传过来的 Frame

    ushort currentRecivedFrame;

    void checkMsg(QByteArray byteArray);
    void checkDataMsg(QByteArray byteArray);

    QFile* savedfile;

    QString receivedFileName;

};

#endif // TCPSOCKETMANAGER_H
