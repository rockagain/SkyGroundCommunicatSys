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

#include <windows.h>
#include "clientsocket.h"
#include "heartbeatmsg.h"
#include "jitongheartbeat.h"
#include "jitongsocket.h"

#include <mediawidget.h>
#include <menuwidget.h>
#include <senddatawidget.h>
#include <datawidget.h>
#include <QHBoxLayout>
#include <QLabel>
#include <QWebEngineView>
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

public slots:
    void showIfNetIsConnectedOrNot(bool IsConnected);


private:
    Ui::MainWindow_New *ui;

    QStackedWidget* stackedWidget;


    MenuWidget* menu_Widget;
    MediaWidget* media_Widget;
    dataWidget* data_Widget;
    QWebEngineView* mapView_Widget;

    void initGraphic();


    int phoneTime ;//  = PHONE_TIME/1000;
    QTimer* callAgainTimer;


private slots:
    //把传过来的信息显示到主窗口控件上
    void receiveRepliedMsgToShow(QString msg);
    //点击按钮，触发这个槽
    void sendShortMsg();
    //槽函数处理 视频相关的
    void closeTheConversation();
    void openTheConversation(int);
    //void sendMsControlInstruct();


    void openFtp();
    void saveAudioFile();
    //void AudioReplay();
    void sendFileNameToGo(QListWidgetItem *item);
    void receiveSocketReadyFileNameToShow(QString fileName);
    void setPhoneNumberText(QString phoneNum);
    void setMsgText(QString Msg);
    void closeTheConversationResource();
    void phoneCallCountBackwards();
    void canCallAgain();
private:

    //用这个值来判定是啥  通话类型
    int VIDEO_OR_AUDIO;

    QProcess *yateProcess;

    ClientSocket * InstructObject;
    HeartbeatMsg* heartbeatObject;

    JiTongHeartbeat* JtHeartbeat;
    JiTongSocket* JtInstruct;

    int VIDEO_RTP_PORT;
    QString SERVER_ADRESS;
    //看看客户端是哪一端
    QString client_side;
    QProcess* ffplayProcess;
    QProcess* ffmpegProcess;
    //表示是否开启了新进程的，开始了通话
    bool openPhoneFlag;
    //
    void init();
    //暂时不用的 sdp 构建方程
    void constructVideoSdpFile();
    //connect state 根据心跳判断其是否在线
    int connectState;
    QAudioInput* audioInput; //录音 pcm
    QFile* audioFile;
    QAudioOutput* audioOutput;

    QString cameraName;

    void mkAudioFiledir();
signals:
    //把信息发到 instructSocket 那里
    void sendMsgToClientSocket(QString msg);
    void emitMsControlManager(bool,char);
    void sendFileNameToClientSocket(QString);

};

#endif // MAINWINDOW_NEW_H
