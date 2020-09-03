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
#include <QIcon>






class ClientSocket : public QObject
{
    Q_OBJECT
public:
    explicit ClientSocket(QObject *parent = 0);

    //初始化 的 函数
    void init();
    //这个参数是显示接通状态的动画的   //未启用
    QMovie * movie;
    //显示接通状态动画的label     //未启用
    QLabel* statusLabel;
    //记录是那一端，全局用
    int dc_side;
    //帧头
    const char* instructHead;
    //char instructHead[5];
    //这里是通知文件来了，播放用的
    QFile* audiofile;
    //电话号码
    QString phoneNumber;
    //短信那里的号码
    QString MsgNumber;
    //setter  设置是前舱还是后舱
    void setBackOrFrontChamber(int value);

public slots:

    //发起视频通话申请  控制30秒不回信发三次， 之后视为无应答
    void videoCallRequestManager();
    //关机
    void closeSystem();
    //关闭电话
    void hangUpthePhoneInstruct();
    //发短信
    void sentShortMsgInstruct(QString msg,int beidouOrHaishi);
    //发送系统时间
    void sendSystemTime();
    //机通自检
    void JTongSelfCheckMsgManager();
    //海事控制
    void mSatelliteControlManager(bool islink, char netstats);
    //弹药信息
    void gunAndFactoryInstrcut(QString,QString);
    //播放文件函数  //有可能在机通里播放了
    void playAudioRecord();
    //播放语音回放指令到机通
    void sendAudioReplayInstructToCI(QString address);
    //北斗卫星指令
    void BeiDouSatelliteInstruct(bool p1,bool p2);
    //从senddataclass接收到信息，来执行
    void receiveMsgFromSendDataClassToGo(QByteArray Instruct);
    //海事控制指令
    void haishiControlInstruct(bool link, int net);
    //空地状态指令
    void skyGroundStatusInstruct(int);
    //播撒中止指令
    void stopWorkingInstruct(QString,QString);
private slots:

    //未启用
    void showPhoneGif();
    //接收到udp的地方
    void instructUdpSocketReceive();
    //关机指令
    void sendCloseSystemInstruct();
    //通过timer的槽函数就行发送  //已废除
    void everyCircleWork();
    //未接来电信息
    void unAnsweredPhoneSignal();
    //切换画面  //未启用
    void changeVideoCallRequest();
    //同意地面的来电
    void OkToGroundCallInstruct();
    //拒绝地面的来电
    void NotOkToGroundCallInstruct();
private:

    //流水号们
    int phoneWaterNumber;        //视频通话申请流水号
    int statsWaterNumber;        //视频状态包
    int closeSystemWaterNumber;  //关机的流水号
    int msgWaterNumber;          //海事短信流水号
    int beidouWaterNumber;       //北斗流水号  未启用
    int beidouMsgWaterNumber;    //北斗短信流水号
    int skyGroundWaterNumber;    //空地信息流水号
    int audioReplyWaterNumber;   //语音回放流水号
    int haishiControlWaterNumber;//海事控制流水号
    int gunAndPlaneWaterNumber;  //飞机编码和弹药厂
    int bosaWaterNumber;         //播撒信息流水号

    //收到的流水号们
    int msgWaterNumber_02;              //CIDC-02
    int rateWaterNumber_03;             //CIDC-03
    int phoneWaterNumber_04;            //CIDC-04
    int statusWaterNumber_05;            //CIDC-05   //06为周期数据
    int haishiWaterNunber_07;           //CIDC-07
    int fileOkWaterNumber_08;           //CIDC-08
    int unAnsweredWaterNumber_09;       //CIDC-09
    int vocalReplyWaterNumber_0a;       //CIDC-10
    int beidouControlWaterNumber_0b;    //CIDC-11
    int beidouMsgWaterNumber_0c;        //CIDC-12

    //心跳结束我们就可以，清空
    //以下timer都是接收指令用的timer，之后修改玩程序后，把所有不用的都删除调
    QTimer* receiveMsgTimer;            //02
    QTimer* rateChangeTimer;            //03
    QTimer* receivePhoneTimer;          //04
    QTimer* receiveStatsTimer;          //05    //06为周期数据
    QTimer* haishiTimer;                //07
    QTimer* fileOkTimer;                //08
    QTimer* unAnsweredTimer;            //09
    QTimer* vocalReplyTimer;            //10
    QTimer* beidouControlTimer;         //11
    QTimer* receiveBeidouMsgTimer;      //12

    //飞机编号
    QString planeNumber;
    //前舱  或者 后舱
    int backOrFrontChamber;
    int closeSystemCount;
    //bool closeSystemOk;
    QString unAnsweredPhoneNum;
    //此参数为客户端的  呼叫模式状态！
    int Conversation_MODE;
    //所有指令循环的时间
    int circleTIME;
    //udp 指令的 发送 和 接收
    QUdpSocket* instructSocket;
    //接电话时候的 messageBox
    QMessageBox* reciveMessagebox ;
    //接电话时候的同意的按钮
    QPushButton* okBtn ;
    //接电话时候的拒绝的按钮
    QPushButton* closeBtn ;
    //初始化 udp 并 配置
    void initInstructSocket();
    //管发送的timer
    QTimer* closeInstructTimer;         //关机指令
    QTimer* videoInstructTimer;         //视频指令
    QTimer* JtongSelfCheckTimer;        // 机通自检
    QTimer* circle_workSendTimer;       //作业撒播  //变为每项都发送
    QTimer* skyGroundStatusTimer;       //海事卫星自检
    QTimer* msControlTimer;             //海事控制
    QTimer* unAnsweredPhoneTimer;       //未接来电计时

    //接收udp，并分析udp的地方
    void checkMsg(QByteArray msg);
    //看人家接不接电话
    void receiveVideoCallOrNot(QByteArray ba);
    //去选择到底接不接电话
    void receiveSomeVideoCallOrNot(QByteArray msg);
    //发送指令，封装的视频申请指令
    void sendVideoCallRequest();
    //接通回放  //需要检查是否被启用
    void receiveJtongselfcheckReply();
    // init 发送的timer 和  检查收到指令的timer ， 及初始化 流水号
    void initTimer();
    //已启用
    void SendCircleManager();
    //给你日期
    QString giveCurrentDate();
    //给你时间
    QString giveCurrentTime();
    //接收短信息并且显示
    void receiveShortMsgAndShow(QByteArray replyMsg);
    //这个参数保存的是  java端 主程序的 IP
    QString SERVER_ADRESS;
    //指令的端口号
    int INSTRUCT_PORT;
    //海事卫星控制指令相关的参数
    bool islink;
    //展示网络状态
    char netstats;
    //显示msc回复的东西
    void showMsControlReply(QByteArray msg);
    //返回当前日期和时间
    QString giveCurrentDateAndTime();
    //如名
    int ByteArrayToInt(QByteArray length_4);
    //如名
    short ByteArrayToShort(QByteArray length_2);
    //如名
    QByteArray ShortToByte(short num);
    //传过来的Audio，需要播放
    QAudioOutput* audioOutput;
    //名字
    QString audioFileName;
    //phone Number   给机通用的
    QString unansweredPhoneNumber;
    //接电话的号码
    QString answeredPhoneNumber;
    //检查指令的长度
    bool checkInstructLength(QByteArray instruct);
    //检查验证码
    bool checkCheckSumNum(QByteArray instruct);
    //如名字
    QByteArray IntToByte(int num);
    //给出当前时间
    QString giveCurrentTimeToShow();
    //change
    QString changeByteToString(QByteArray);
    //给short转换大小端
    short BLEndianShort(short value);
    //给流水号增一
    void incrementWaterNumber(int &number);
    //北斗短信
    void sentShortMsgInstructBeiDou(QString msg);
    //海事短信
    void sentShortMsgInstructHaiShi(QString msg);
    //接收北斗短信，并显示
    void receiveShortMsgAndShow_beidou(QByteArray replyMsg);
    //转换 int 大小端
    uint BLEndianInt(uint x);

signals:
    //接收短信发送给主窗体
    void sendRepliedMsgtoMainWindow(QString,int);
    //打开ffmpeg
    void openConversation(int);
    //把海事卫星selfcontrol信息给主窗体
    void sendMsSCMsgtoMwtoShow(QString);
    //把海事控制信息给主窗体
    void sendMsControlMsgtoMwToShow(QString);
    //把未接来电给机通
    void emitUnAnswerPhoneToJiTong(QString);
    //把接了的来电给机通
    void emitAnswerPhoneToJiTong(QString);
    //关闭主窗体的ffmpeg资源
    void closeMainWindowResource();
    //把文件名字给主窗口
    void sendReadyFilenameToMainWindow(QString);
    //把语音列表给主窗口
    void sendAudioListToMainWindow(QString);
    //改变周期，发给 senddatawidget  类 调节 timer周期的
    void changeCircleRate(int);
    //把状态显示出来
    void showMslinkStatusReport(QByteArray msg);
    //把未接来电show到界面上的
    void showUnansweredPhoneNumber(QByteArray msg);
    //打开北斗信息
    void openBeidouWorkInfo(bool);
};
#endif // CLIENTSOCKET_H
