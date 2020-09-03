#ifndef CLIENTSOCKET_H
#define CLIENTSOCKET_H

#include <QObject>
#include <QUdpSocket>
#include <QHostAddress>
#include <QByteArray>
#include <QMessageBox>
#include <QTimer>
#include <config.h>
#include <QMovie>
#include <QLabel>
#include <QTextCodec>
#include <QDate>
#include <QDateTime>
#include <QSettings>
#include <QFile>
#include <QAudioOutput>
#include <QPushButton>

#include <QAudioFormat>
#include <QAudioOutput>
#include <QIcon>




class ClientSocket : public QObject
{
    Q_OBJECT
public:
    explicit ClientSocket(QObject *parent = 0);
    void init();
    QMovie * movie;  //这个参数是显示接通状态的动画的
    QLabel* statusLabel;  //显示接通状态动画的label

    //记录是那一端，全局用
    int dc_side;
    //帧头
    const char* instructHead;
    //char instructHead[5];
    //这里是通知文件来了，播放用的
    QFile* audiofile;
    //
    QString phoneNumber;
    QString MsgNumber;
    //收报地址  //电话那里用了，短信那里可能也需要用，直接填进去就好了啊
    QString receiveAddress;
public slots:
    //发起视频通话申请  控制30秒不回信发三次， 之后视为无应答
    void videoCallRequestManager();
    //关机
    void closeSystem();
    //关闭电话
    void hangUpthePhoneInstruct();
    //发短信
    void sentShortMsgInstruct(QString msg);
    //发送系统时间
    void sendSystemTime();
    //机通自检
    void JTongSelfCheckMsgManager();
    void mSatelliteSelfcheckManager();
    void mSatelliteControlManager(bool islink, char netstats);
    void mSatelliteControlInstruct();
    void playAudioRecord();
    void sendAudioReplayInstructToCI(QString address);
    void changeRateInstruct(int r);
private slots:
    void showPhoneGif();
    void instructUdpSocketReceive();
    //发送的指令
    void sendCloseSystemInstruct();
    //通过timer的槽函数就行发送
    void homeWorkMsgInstruct();
    void everyCircleWork();
    void unAnsweredPhoneSignal();
    void handleAudioFileStateChanged(QAudio::State newState);
    //接电话时候的槽
    void OkToGroundCallInstruct();
    void NotOkToGroundCallInstruct();
private:
    //流水号们
    //视频通话申请流水号
    int phoneWaterNumber;
    int msgWaterNumber;
    int changeRateNumber;
    //收到流水号
    uint phoneWaterNumber_01;
    uint statusWaterNumber_01;
    uint msgWaterNumber_01;

    QTimer* phoneTimer;
    QTimer* statusTimer;
    QTimer* msgTimer;


    QString unAnsweredPhoneNum;
    int Conversation_MODE;  //此参数为客户端的  呼叫模式状态！
    int circleTIME;    //所有指令循环的时间
    //sender 是 客户端，   receiver 是 服务器端   //先用来处理视频和 文字信息的
    //QUdpSocket* sender;  //只有一个连接
    QUdpSocket* instructSocket;

    void initInstructSocket();

    QTimer* closeInstructTimer;   //关机指令
    QTimer* videoInstructTimer;   //视频指令
    QTimer* JtongSelfCheckTimer;   // 机通自检
    QTimer* circle_workSendTimer;     //作业撒播  //变为每项都发送
    QTimer* msSelfCheckTimer;      //海事卫星自检
    QTimer* msControlTimer;    //海事控制
    QTimer* unAnsweredPhoneTimer;  //未接来电计时

    void checkMsg(QByteArray msg);
    void receiveVideoCallOrNot(QByteArray ba);
    void receiveSomeVideoCallOrNot(QByteArray msg);

    //发送指令，封装的视频申请指令
    void sendVideoCallRequest();
    //发送机通自检信息
    void JtongSelfCheckMsg();

    void receiveJtongselfcheckReply();
    void initTimer();

    void SendHomeWorkMsgManager();
    //返给你当前时间
    QString giveCurrentDate();
    QString giveCurrentTime();
    //接收短信息并且显示
    void receiveShortMsgAndShow(QByteArray replyMsg);
    QString serialNumber;
    QString SERVER_ADRESS;
    int INSTRUCT_PORT;
    void mSSelfcheckinstruct();
    void showMsSelfcheckReply(QByteArray msg);

    //海事卫星控制指令相关的参数
    bool islink;
    char netstats;
    //展示网络状态

    //显示msc回复的东西
    void showMsControlReply(QByteArray msg);


    QString giveCurrentDateAndTime();
    int ByteArrayToInt(QByteArray length_4);
    short ByteArrayToShort(QByteArray length_2);
    QByteArray ShortToByte(short num);
    //传过来的Audio，需要播放
    QAudioOutput* audioOutput;
    QString audioFileName;
    //phone Number
    QString unasweredPhoneNumber;
    bool checkInstructLength(QByteArray instruct);
    bool checkCheckSumNum(QByteArray instruct);
    QByteArray IntToByte(int num);


    void incrementWaterNumber(int &number);
    short BLEndianShort(short value);
    uint BLEndianInt(uint x);

    //接电话时候的 QMessageBox
    QMessageBox* reciveMessagebox;

    QPushButton* okBtn ;
    QPushButton* closeBtn ;

signals:
    void sendRepliedMsgtoMainWindow(QString);
    void openConversation(int);
    void sendMsSCMsgtoMwtoShow(QString);
    void sendMsControlMsgtoMwToShow(QString);
    void emitUnAnswerPhone(QString);
    void closeMainWindowResource();
    void sendReadyFilenameToMainWindow(QString);

};
#endif // CLIENTSOCKET_H
