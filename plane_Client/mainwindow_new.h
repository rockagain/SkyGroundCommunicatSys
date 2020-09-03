#ifndef MAINWINDOW_NEW_H
#define MAINWINDOW_NEW_H


#include <QMainWindow>
#include <QObject>
#include <QTextCodec>
#include <QProcess>
#include <QDir>
#include <QAudioFormat>
#include <QAudioDeviceInfo>
#include <QAudioInput>
#include <QAudioOutput>
#include <QListWidgetItem>
#include <QStackedWidget>
#include <QRadiobutton>

#include <Windows.h>
#include "clientsocket.h"
#include "heartbeatmsg.h"
#include "jitongheartbeat.h"
#include "jitongsocket.h"

#include "mediawidget.h"
#include "menuwidget.h"
#include "senddatawidget.h"
#include "datawidget.h"
#include "msatellitewidget.h"
#include "send429class.h"
#include "statustoshow.h"    //海事状态
#include <QHBoxLayout>
#include <QLabel>
#include <QCheckBox>
#include <QFileDialog>






namespace Ui {
class MainWindow_New;
}

class MainWindow_New : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow_New(QWidget *parent = 0);
    ~MainWindow_New();


private:
    Ui::MainWindow_New *ui;
    //这个是切换几个模块的
    QStackedWidget* stackedWidget;
    //按钮 ，ui类，自己建的
    MenuWidget* menu_Widget;
    //视频电话的widget ，ui类，自己建的
    MediaWidget* media_Widget;
    //短信及一些其他信息的widget ，ui类，自己建的
    dataWidget* data_Widget;
    //这个类里发送周期性数据 ，ui类，自己建的
    sendDataWidget* sendData_Widget;
    //海事卫星的，  里面配置的web ，ui类，自己建的
    mSatelliteWidget* mSatellite_Widget;
    //429   422相关的widget  ，ui类，自己建的
    send429class* send429_Widget;
    //ui类，自己建的
    statusToShow* showStatus_Widget;
    //选是前面仓
    QRadioButton* FrontChamber;
    //选是后面仓
    QRadioButton* BackChamber;
    //电话的时间
    int phoneTime ;//  = PHONE_TIME/1000;
    //设置啥时候再重来的 timer
    QTimer* callAgainTimer;
    //初始化UI
    void initGraphic();

private slots:
    //把传过来的信息显示到主窗口控件上
    void receiveRepliedMsgToShow(QString msg,int);
    //点击按钮，触发这个槽
    void sendShortMsg();
    //槽函数处理 视频相关的
    void closeTheConversation();
    //打开ffplay资源
    void openTheConversation(int);
    //如名
    void openFtp();
    //下载文件相关的函数，现已被废弃
    void sendFileNameToGo(QListWidgetItem *item);
    //下载文件相关的函数，现在该函数已被废弃
    void receiveSocketReadyFileNameToShow(QString fileName);
    //更新电话号码的
    void setPhoneNumberText(QString phoneNum);
    //发送短信号码的
    void setMsgNumber(QString Msg);
    //显示audio名字再主界面的
    void receiveAudioFileToShow(QString fileName);
    //把来的音频请求传给clientSocket发送的
    void sendAudioFileToGo(QListWidgetItem *item);
    //管卫通信息的
    void phoneOrNot(bool);
    //前面的仓
    void setFrontChamber();
    //后面的仓
    void setBackChamber();
    //把按钮机会，让其可再次来电
    void canCallAgain();
    //电话后面的倒计时
    void phoneCallCountBackwards();
    //显示是否有心跳啊
    void showIfNetIsConnectedOrNot(bool ifItsConnected);
    //只关闭资源
    void closeConversationResource();

private:

    //用这个值来判定是啥  通话类型
    int VIDEO_OR_AUDIO;
    //初始化指令类，发指令收指令用的
    ClientSocket * InstructObject;
    //心跳类
    HeartbeatMsg* heartbeatObject;
    //机通指令类
    JiTongSocket* JtInstruct;
    //视频端口
    int VIDEO_RTP_PORT;
    //java程序 IP
    QString SERVER_ADRESS;
    //看看客户端是哪一端
    QString client_side;
    //ffplay 进程
    QProcess* ffplayProcess;
    //表示是否开启了新进程的，开始了通话
    bool openPhoneFlag;
    //大部分的 信号和槽都在这里面
    void init();
    //暂时不用的 sdp 构建 函数
    void constructVideoSdpFile();
    //connect state 根据心跳判断其是否在线
    int connectState;
    //录音pcm
    QAudioInput* audioInput;
    //如名
    QFile* audioFile;
    //如名
    QAudioOutput* audioOutput;
    //自动创建 testFile 文件夹， 这里面包含 机通发送的音频  sendAudio.pcm   接收的音频
    void mkAudioFiledir();
signals:
    //把信息发到 instructSocket 那里
    void sendMsgToClientSocket(QString msg,int beidouOrHaishi);
    //海事控制信息
    void emitMsControlManager(bool,char);
    //把文件名字发送到 clientsocket
    void sendFileNameToClientSocket(QString);
    //把音频文件发送到 clientsocket
    void sendAudioNameToClientSocket(QString);   

};

#endif // MAINWINDOW_NEW_H
