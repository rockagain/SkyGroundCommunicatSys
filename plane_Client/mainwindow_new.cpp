#include "mainwindow_new.h"
#include "ui_mainWindow_new.h"

MainWindow_New::MainWindow_New(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow_New)
{   
    initGraphic();
    init();
}

MainWindow_New::~MainWindow_New()
{
    delete ui;
}



void MainWindow_New::initGraphic()
{
    //QDesktopServices::openUrl(QUrl("https://www.baidu.com"));
    ui->setupUi(this);
    ui->statusbar->hide();


    QWidget* mainWidget = new QWidget;
    this->setCentralWidget(mainWidget);

    menu_Widget = new MenuWidget;
    stackedWidget = new QStackedWidget;
    media_Widget = new MediaWidget;
    data_Widget = new dataWidget;
    sendData_Widget = new sendDataWidget;
    mSatellite_Widget = new mSatelliteWidget;
    send429_Widget = new send429class;
    showStatus_Widget = new statusToShow;

    stackedWidget->addWidget(media_Widget);
    stackedWidget->addWidget(data_Widget);
    stackedWidget->addWidget(sendData_Widget);
    stackedWidget->addWidget(mSatellite_Widget);
    stackedWidget->addWidget(send429_Widget);
    stackedWidget->addWidget(showStatus_Widget);

    QHBoxLayout *Layout = new QHBoxLayout;
    Layout->setContentsMargins(0,0,0,0);
    //Layout->setSpacing(0);
    Layout->addWidget(menu_Widget);
    Layout->addWidget(stackedWidget);
    Layout->setStretchFactor(menu_Widget, 2);
    Layout->setStretchFactor(stackedWidget, 10);

    mainWidget->setLayout(Layout);
    mainWidget->setContentsMargins(0,0,0,0);
    // 下面的都改为 触发的 接收左侧按钮选择界面的
    connect(menu_Widget,&MenuWidget::changeWidgetSignals,stackedWidget,&QStackedWidget::setCurrentIndex);
}




//初始化设定
void MainWindow_New::init() {

    //QListWidgetItem* i= new QListWidgetItem;
    //i->setText("nihaoma.pcm");
    //data_Widget->filelistWidget->addItem(i);
    //ui->playAudioButton->setHidden(true);   //假如地面端没有录制功能，则屏蔽放在地面端初始化那里;
    ffplayProcess = new QProcess;

    mkAudioFiledir();
    //设定是否开启了 通话设备
    openPhoneFlag = false;
    callAgainTimer = new QTimer;
    callAgainTimer->setInterval(1000);
    connect(callAgainTimer,&QTimer::timeout,this,&MainWindow_New::phoneCallCountBackwards);

    //data_Widget->saveAudioButton->setCheckable(true);
    data_Widget->MsgNumber->setText("01234");
    media_Widget->phoneNumber->setText("0123456789");
    //所有指令端的都列在这里
    InstructObject = new ClientSocket();
    InstructObject->phoneNumber = media_Widget->phoneNumber->text();
    InstructObject->MsgNumber = data_Widget->MsgNumber->text();
    connect(media_Widget->phoneNumber,&QLineEdit::textChanged,this,&MainWindow_New::setPhoneNumberText);
    connect(data_Widget->MsgNumber,&QLineEdit::textChanged,this,&MainWindow_New::setMsgNumber);

    //设置前舱后舱的
    FrontChamber = new QRadioButton(media_Widget);
    FrontChamber->setGeometry(860,60,141,51);
    FrontChamber->setText(QStringLiteral("前舱"));
    FrontChamber->setChecked(true);
    InstructObject->setBackOrFrontChamber(FRONT_CHAMBER);

    BackChamber = new QRadioButton(media_Widget);
    BackChamber->setGeometry(940,60,141,51);
    BackChamber->setText(QStringLiteral("后舱"));


    connect(FrontChamber,&QRadioButton::clicked,this,&MainWindow_New::setFrontChamber);
    connect(BackChamber,&QRadioButton::clicked,this,&MainWindow_New::setBackChamber);
    //读取设置
    QSettings* settings = new QSettings(QCoreApplication::applicationDirPath()+"/sky_config.ini", QSettings::IniFormat);
    QVariant var = settings->value("SIDE/CLIENTSIDE");
    QString side = var.toString();

    if((side == "LEFT")||(side == "RIGHT")){

    } else {
        QMessageBox::information(NULL, "Title", QStringLiteral("该端为显控端，请正确填写配置文件，并重启程序"));
        exit(0);
    }


    connect(sendData_Widget,&sendDataWidget::emitBeiDouStatus,InstructObject,&ClientSocket::BeiDouSatelliteInstruct);
    //把client_side 设置上
    client_side = side;

    if(side == "LEFT") {
        //设定指令栏
        this->setWindowTitle(QStringLiteral("机载通信左显控端模拟程序"));
        QVariant var = settings->value("ADRESS/AIR_SERVER_ADRESS");
        SERVER_ADRESS = var.toString();
        VIDEO_RTP_PORT = 50102;
        heartbeatObject = new HeartbeatMsg;
        connect(heartbeatObject,&HeartbeatMsg::sendSystemTimeSignal,InstructObject,&ClientSocket::sendSystemTime);
        //测试 ！  把心跳 和 显示连接起来
        connect(heartbeatObject,&HeartbeatMsg::emitIfNetIsConnectedOrNot,this,&MainWindow_New::showIfNetIsConnectedOrNot);

    } else if(side == "RIGHT"){
        //设定指令栏
        this->setWindowTitle(QStringLiteral("机载通信右显控端模拟程序"));
        QVariant var = settings->value("ADRESS/AIR_SERVER_ADRESS");
        SERVER_ADRESS = var.toString();    //之后可能需要添加上 音频的端口
        VIDEO_RTP_PORT = 50112;
        heartbeatObject = new HeartbeatMsg;
        connect(heartbeatObject,&HeartbeatMsg::sendSystemTimeSignal,InstructObject,&ClientSocket::sendSystemTime);
        //收到指令后发送信息
        //测试 ！  把心跳 和 显示连接起来
        connect(heartbeatObject,&HeartbeatMsg::emitIfNetIsConnectedOrNot,this,&MainWindow_New::showIfNetIsConnectedOrNot);
    }



    //这个判断里 申请机通 object ，假如不是地面端再申请， 是地面端的话就不申请机通
    if(side == "LEFT")  //把机通和左显控放在一起
    {
        JtInstruct = new JiTongSocket;
        connect(InstructObject,&ClientSocket::emitUnAnswerPhoneToJiTong,JtInstruct,&JiTongSocket::UnAnsweredPhoneInstruct);
        connect(InstructObject,&ClientSocket::emitAnswerPhoneToJiTong,JtInstruct,&JiTongSocket::AnsweredPhoneInstruct);
        //除了用指令激活，这个是程序的外部接口
        connect(data_Widget->sendAudioButton,&QPushButton::clicked,JtInstruct,&JiTongSocket::sendAudioRecordManager);
        //收到信息发送名字     双击栏里的item发送 clientSocket请求
        connect(InstructObject,&ClientSocket::sendReadyFilenameToMainWindow,this,&MainWindow_New::receiveSocketReadyFileNameToShow);

        //file这个现在没法下载了
        //connect(data_Widget->filelistWidget,&QListWidget::itemDoubleClicked,this,&MainWindow_New::sendFileNameToGo);

        connect(this,&MainWindow_New::sendFileNameToClientSocket,InstructObject,&ClientSocket::sendAudioReplayInstructToCI);

        connect(JtInstruct,&JiTongSocket::phoneOrNot,this,&MainWindow_New::phoneOrNot);
    }
    //这个是单独出来的特殊功能，看需求选择添加与否
    if(side == "RIGHT") {
        connect(InstructObject,&ClientSocket::sendReadyFilenameToMainWindow,this,&MainWindow_New::receiveSocketReadyFileNameToShow);
        data_Widget->sendAudioButton->hide();
    }


    //通过设定信号和槽来判断是否有心跳，没心跳的话，提示报错，不能触发动作，假如不是飞机端，则隐藏按钮
    //这里为了调试先设定有心跳
    connectState = NET_IS_CONNECTED;

    //发送视频请求
    connect(media_Widget->VedioPhoneCall,&QPushButton::clicked,InstructObject,&ClientSocket::videoCallRequestManager);
    connect(media_Widget->VedioPhoneCall,&QPushButton::clicked,this,&MainWindow_New::canCallAgain);   //设置超过了一定时间才可以再次打电话

    //连接 接听电话之后的signal，暂时开启播放和 接收
    connect(InstructObject,&ClientSocket::openConversation,this,&MainWindow_New::openTheConversation);

    //挂断视频   // 挂断电话相当于
    connect(media_Widget->stopVideo,&QPushButton::clicked,this,&MainWindow_New::closeTheConversation);

    //关闭系统
    connect(menu_Widget,&MenuWidget::shutDownTheSystem,InstructObject,&ClientSocket::closeSystem);

    //接收短信，显示用的
    connect(InstructObject,&ClientSocket::sendRepliedMsgtoMainWindow,this,&MainWindow_New::receiveRepliedMsgToShow);
    //发送短信
    connect(data_Widget->sendMsgButton,&QPushButton::clicked,this,&MainWindow_New::sendShortMsg);
    connect(this,&MainWindow_New::sendMsgToClientSocket,InstructObject,&ClientSocket::sentShortMsgInstruct);


    //FTP  目前测试
    connect(data_Widget->ftpButton,&QPushButton::clicked,this,&MainWindow_New::openFtp);

    //点击录音然后上传音频
    //connect(data_Widget->saveAudioButton,&QPushButton::clicked,this,&MainWindow_New::saveAudioFile);

    //点击按钮，播放上传音频，可能只需要飞机端有
    // connect(data_Widget->playAudioButton,&QPushButton::clicked,InstructObject,&ClientSocket::playAudioRecord);

    //接到对方挂机指令，关闭资源
    connect(InstructObject,&ClientSocket::closeMainWindowResource,this,&MainWindow_New::closeConversationResource);

    //把 sendData类里面的内容 传给  clientSocket  去  发送
    connect(sendData_Widget,&sendDataWidget::giveMsgToClientSocketToGo,InstructObject,&ClientSocket::receiveMsgFromSendDataClassToGo);
    //把音频显示在 mainwindow上
    connect(InstructObject,&ClientSocket::sendAudioListToMainWindow,this,&MainWindow_New::receiveAudioFileToShow);
    //点击音频触发动作
    connect(data_Widget->AudiolistWidget,&QListWidget::itemDoubleClicked,this,&MainWindow_New::sendAudioFileToGo);
    //把音频文件名发给指令Socket
    connect(this,&MainWindow_New::sendAudioNameToClientSocket,InstructObject,&ClientSocket::sendAudioReplayInstructToCI);
    //改变速率
    connect(InstructObject,&ClientSocket::changeCircleRate,sendData_Widget,&sendDataWidget::changeRate);
    //海事卫星 链路状态数据上报 CIDC-06
    connect(InstructObject,&ClientSocket::showMslinkStatusReport,showStatus_Widget,&statusToShow::showMsattilateLinkStatusReport);
    //未接来电 CIDC-09  mediawidget qbytearray
    connect(InstructObject,&ClientSocket::showUnansweredPhoneNumber,media_Widget,&MediaWidget::showUnAnsweredPhone);
    //海事卫星控制 EN-DCCI-22
    connect(sendData_Widget,&sendDataWidget::haishiControlMsg,InstructObject,&ClientSocket::haishiControlInstruct);
    //空地状态 指令
    connect(sendData_Widget,&sendDataWidget::emitSkyGroundMsg,InstructObject,&ClientSocket::skyGroundStatusInstruct);
    //飞机编码和弹厂商
    connect(sendData_Widget,&sendDataWidget::emitGunAndPlane,InstructObject,&ClientSocket::gunAndFactoryInstrcut);
    //播撒信息
    connect(sendData_Widget,&sendDataWidget::emitBoSaMsg,InstructObject,&ClientSocket::stopWorkingInstruct);
    //打开或者关闭北斗作业信息
    connect(InstructObject,&ClientSocket::openBeidouWorkInfo,sendData_Widget,&sendDataWidget::controlBeidouWorkInfo);
    //初始化429
    connect(menu_Widget,&MenuWidget::init429Signals,send429_Widget,&send429class::init429board);
}


void MainWindow_New::setFrontChamber()
{
    InstructObject->setBackOrFrontChamber(FRONT_CHAMBER);
}


void MainWindow_New::setBackChamber()
{
    InstructObject->setBackOrFrontChamber(BACK_CHAMBER);
}


void MainWindow_New::phoneCallCountBackwards()
{
    media_Widget->phoneCallCount->clear();
    media_Widget->phoneCallCount->setText(QString::number(phoneTime) + QStringLiteral("秒"));
    phoneTime = phoneTime - 1;
    if(phoneTime == 0){
        media_Widget->phoneCallCount->clear();
        media_Widget->VedioPhoneCall->setEnabled(true);
        callAgainTimer->stop();
    }
}


void MainWindow_New::canCallAgain(){
    media_Widget->VedioPhoneCall->setEnabled(false);
    phoneTime = PHONE_TIME/1000;
    callAgainTimer->start();
}


//接收短信去显示
void MainWindow_New::receiveRepliedMsgToShow(QString msg,int what) {
    QString originalMsg = data_Widget->receiveMsgBox->toPlainText();
    data_Widget->receiveMsgBox->clear();
    QString nowMsg;
    if(what == HAISHI_MSG) {
        nowMsg= QStringLiteral("海事:") + msg + "\n" + originalMsg;
    } else if(what == BEIDOU_MSG) {
        nowMsg= QStringLiteral("北斗:") + msg + "\n" + originalMsg;
    }

    data_Widget->receiveMsgBox->setText(nowMsg);  //以前是msg就好了，现在改为累加信息的模式就需要变了
}

void MainWindow_New::sendShortMsg(){
    if(data_Widget->sendMsgBox->toPlainText().size() == 0){
        QMessageBox::information(NULL, "Title", QStringLiteral("无法发送空信息"));
        return;
    }
    sendMsgToClientSocket(data_Widget->sendMsgBox->toPlainText(),data_Widget->getHaishi_Or_Beidou());
    //把发送的内容显示在右边
    QString originalMsg = data_Widget->sendedMsgBox->toPlainText();
    data_Widget->sendedMsgBox->clear();
    QString nowMsg;
    if(data_Widget->getHaishi_Or_Beidou() == BEIDOU_MSG) {
        nowMsg = QStringLiteral("北斗:") + data_Widget->sendMsgBox->toPlainText() + "\n" + originalMsg;
    } else {
        nowMsg = QStringLiteral("海事:") + data_Widget->sendMsgBox->toPlainText() + "\n" + originalMsg;
    }
    data_Widget->sendedMsgBox->setText(nowMsg);
    //这里的前提是网络通的情况下，网络不同则提示网络不通，所有的地方都要提示网络不通
    if(connectState == NET_IS_CONNECTED){
        data_Widget->sendMsgBox->clear();   //发送之后即清空
    } else {
        QMessageBox::information(NULL, "Title", QStringLiteral("与系统处于断开状态"));
    }
}


void MainWindow_New::openTheConversation(int videoOrAudio){
    VIDEO_OR_AUDIO = videoOrAudio;
    //假如是video 视频音频 否则只开  音频
    if(VIDEO_OR_AUDIO == VIDEOCHAT) {



        ffplayProcess->setCreateProcessArgumentsModifier([] (QProcess::CreateProcessArguments *args)
        {
            //  这参数在 Windows.h 头文件里了，
            args->flags |= CREATE_NEW_CONSOLE;
            args->startupInfo->dwFlags &= ~STARTF_USESTDHANDLES;
        });
        //constructSdpFile(); //这个函数需要更改参数
        //QString command = "ffplay.exe -protocol_whitelist file,http,https,rtp,udp,tcp,tls " + QDir::currentPath() + "/aaa.sdp";
        QString command = "ffplay.exe -protocol_whitelist file,http,https,rtp,udp,tcp,tls " + QCoreApplication::applicationDirPath();
        if (client_side == "LEFT") {
            command = command + "/left.sdp";
        } else if (client_side == "RIGHT") {
            command = command + "/right.sdp";
        }
        qDebug()<< command;
        ffplayProcess->start(command);
    } else {
        //语音
    }

    openPhoneFlag = true;
}


void MainWindow_New::closeConversationResource()
{
    if(ffplayProcess->isOpen()){
        ffplayProcess->close();
    }
}




void MainWindow_New::closeTheConversation()
{

    InstructObject->hangUpthePhoneInstruct();

    if(ffplayProcess->isOpen())
        ffplayProcess->close();

    openPhoneFlag = false;
}

//先不用 因为里面两个IP的参数不知道怎么设置
void MainWindow_New::constructVideoSdpFile() {
    QFile TheFile("video.sdp");
    TheFile.open(QIODevice::ReadWrite);
    QByteArray sdp;
    sdp.append("SDP:\nv=0o=- 0 0 IN IP4 ");
    sdp.append("127.0.0.1");   //之后可能会需要代替，用本机的ip测试
    sdp.append("s=No Namec=IN IP4 ");
    sdp.append(SERVER_ADRESS);
    sdp.append("t=0 \n");
    sdp.append("0a=tool:libavformat 58.29.100m=video ");
    sdp.append(QString::number(VIDEO_RTP_PORT));
    sdp.append(" RTP/AVP 96b=AS:150a=rtpmap:96 \n");
    sdp.append("H264/90000a=fmtp:96 packetization-mode=1");
    TheFile.write(sdp);
}







void MainWindow_New::openFtp()
{
    qDebug()<<QStringLiteral("调用 FTP");
    QString filepath = QFileDialog::getOpenFileName(this,QStringLiteral("选择单个文件"),"./");

    QProcess *p = new QProcess;
    QProcess *p_2 = new QProcess;

    p->setCreateProcessArgumentsModifier([] (QProcess::CreateProcessArguments *args)
    {
        //  这参数在 Windows.h 头文件里了，
        args->flags |= CREATE_NEW_CONSOLE;
        args->startupInfo->dwFlags &= ~STARTF_USESTDHANDLES;
    });
    QString a = QCoreApplication::applicationDirPath() +  "/create.bat  " + filepath;  //ftp.exe
    qDebug()<<a;
    p->setWorkingDirectory(QCoreApplication::applicationDirPath());
    p->start(a);
    p->waitForFinished();
    a = "ftp -n -s:" +  QCoreApplication::applicationDirPath() + "/ftp.bat\n";
    p_2->setCreateProcessArgumentsModifier([] (QProcess::CreateProcessArguments *args)
    {
        //  这参数在 Windows.h 头文件里了，
        args->flags |= CREATE_NEW_CONSOLE;
        args->startupInfo->dwFlags &= ~STARTF_USESTDHANDLES;
    });
    p_2->start(a);

}



void MainWindow_New::sendAudioFileToGo(QListWidgetItem *item)
{
    //Audio File
    QString fileName = item->text();
    emit sendAudioNameToClientSocket(fileName);
}


void MainWindow_New::sendFileNameToGo(QListWidgetItem *item)
{
    QString fileName = item->text();
    fileName = fileName.split(" ")[0];
    emit sendFileNameToClientSocket(fileName);
}

void MainWindow_New::receiveSocketReadyFileNameToShow(QString fileName)
{
    QListWidgetItem* item = new QListWidgetItem;
    item->setText(fileName);
    data_Widget->filelistWidget->addItem(item);
}

void MainWindow_New::receiveAudioFileToShow(QString fileName)
{
    QListWidgetItem* item = new QListWidgetItem;
    item->setText(fileName);
    data_Widget->AudiolistWidget->addItem(item);
}


void MainWindow_New::setPhoneNumberText(QString phoneNum)
{
    InstructObject->phoneNumber = phoneNum;
}

void MainWindow_New::setMsgNumber(QString Msg)
{
    InstructObject->MsgNumber = Msg;
}


void MainWindow_New::mkAudioFiledir()
{
    QString fullPath = QCoreApplication::applicationDirPath() + "/testFile";
    QDir dir(fullPath);
    if(dir.exists())
    {
        return;
    }
    else
    {
        bool ok = dir.mkdir(fullPath);//只创建一级子目录，即必须保证上级目录存在
    }
}


void MainWindow_New::phoneOrNot(bool ok)
{
    if(ok) {
        media_Widget->VedioPhoneCall->setEnabled(true);
        media_Widget->phaneLabel->setText(QStringLiteral("卫通电话状态: 空闲"));
    } else {
        media_Widget->VedioPhoneCall->setEnabled(false);
        media_Widget->phaneLabel->setText(QStringLiteral("卫通电话状态: 通话中"));
    }
}


void MainWindow_New::showIfNetIsConnectedOrNot(bool IsConnected)
{
    connectState = IsConnected;
    if(IsConnected == NET_IS_CONNECTED) {
        media_Widget->showConnectedOrNot->setText(QStringLiteral("已链接"));
    } else if (IsConnected == NET_IS_NOT_CONNECTED) {
        media_Widget->showConnectedOrNot->setText(QStringLiteral("未链接"));
    }
}


































