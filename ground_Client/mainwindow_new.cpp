#include "mainwindow_new.h"
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
    ui->setupUi(this);
    ui->statusbar->hide();
    setWindowTitle(QStringLiteral("机载通信地面端模拟程序"));


    QWidget* mainWidget = new QWidget;

    this->setCentralWidget(mainWidget);


    menu_Widget = new MenuWidget;
    stackedWidget = new QStackedWidget;
    media_Widget = new MediaWidget;
    data_Widget = new dataWidget;
    mapView_Widget = new QWebEngineView;
    //QWidget* mSatellite_Widget = new QWidget;

    stackedWidget->addWidget(media_Widget);
    stackedWidget->addWidget(data_Widget);

    QSettings* settings = new QSettings(QCoreApplication::applicationDirPath()+"/ground_config.ini", QSettings::IniFormat);




    QVariant var = settings->value("WEB/URL","http://192.168.0.104");
    stackedWidget->addWidget(mapView_Widget);
    QString webUrl =var.toString() ;
    mapView_Widget->load(webUrl);




    QHBoxLayout *Layout = new QHBoxLayout;
    Layout->setContentsMargins(0,0,0,0);
    Layout->addWidget(menu_Widget);
    Layout->addWidget(stackedWidget);
    Layout->setStretchFactor(menu_Widget, 2);
    Layout->setStretchFactor(stackedWidget,10);

    mainWidget->setLayout(Layout);
    mainWidget->setContentsMargins(0,0,0,0);
    // 下面的都改为 触发的
    connect(menu_Widget,&MenuWidget::changeWidgetSignals,stackedWidget,&QStackedWidget::setCurrentIndex);
}






//初始化设定
void MainWindow_New::init() {


    mkAudioFiledir();

    //设定是否开启了 通话设备
    openPhoneFlag = false;
    callAgainTimer = new QTimer;
    callAgainTimer->setInterval(1000);
    connect(callAgainTimer,&QTimer::timeout,this,&MainWindow_New::phoneCallCountBackwards);

    //把电话号码写下来，填进去
    ffmpegProcess = new QProcess;
    ffplayProcess = new QProcess;

    data_Widget->saveAudioButton->setCheckable(true);
    data_Widget->MsgNumber->setText("01234");
    media_Widget->phoneNumber->setText("0123456789");
    // 所有指令端的都列在这里
    InstructObject = new ClientSocket();
    InstructObject->phoneNumber = media_Widget->phoneNumber->text();
    InstructObject->MsgNumber = data_Widget->MsgNumber->text();
    connect(media_Widget->phoneNumber,&QLineEdit::textChanged,this,&MainWindow_New::setPhoneNumberText);
    connect(data_Widget->MsgNumber,&QLineEdit::textChanged,this,&MainWindow_New::setMsgText);

    QSettings* settings = new QSettings(QCoreApplication::applicationDirPath()+"/ground_config.ini", QSettings::IniFormat);

    QString side = "GROUND";


    //ui->mainToolBar->hide();
    QVariant var;
    var = settings->value("DEVICES/CAMERA");
    cameraName = var.toString();

    //把client_side 设置上
    client_side = side;

    if(side == "GROUND") {
        //假如是地面端，则把不能触发的动作给隐藏了，根据动能new对象
        data_Widget->filelistWidget->hide();     //地面没有传程序

        QVariant var = settings->value("ADRESS/LAND_SERVER_ADRESS");
        SERVER_ADRESS = var.toString();
        QVariant port = settings->value("LAND_PORT/VIDEORTP_PORT");  //之后可能需要添加上 音频的端口
        VIDEO_RTP_PORT = port.toInt();  //读取配置
        heartbeatObject = new HeartbeatMsg;
    }

    connect(heartbeatObject,&HeartbeatMsg::emitIfNetIsConnectedOrNot,this,&MainWindow_New::showIfNetIsConnectedOrNot);

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

    //接收短信，显示用的
    connect(InstructObject,&ClientSocket::sendRepliedMsgtoMainWindow,this,&MainWindow_New::receiveRepliedMsgToShow);
    //发送短信
    connect(data_Widget->sendMsgButton,&QPushButton::clicked,this,&MainWindow_New::sendShortMsg);
    connect(this,&MainWindow_New::sendMsgToClientSocket,InstructObject,&ClientSocket::sentShortMsgInstruct);


    //FTP  目前测试
    connect(data_Widget->ftpButton,&QPushButton::clicked,this,&MainWindow_New::openFtp);

    //点击录音然后上传音频
    connect(data_Widget->saveAudioButton,&QPushButton::clicked,this,&MainWindow_New::saveAudioFile);

    //点击按钮，播放上传音频，可能只需要飞机端有
    connect(data_Widget->playAudioButton,&QPushButton::clicked,InstructObject,&ClientSocket::playAudioRecord);

    //接到对方挂机指令，关闭资源
    connect(InstructObject,&ClientSocket::closeMainWindowResource,this,&MainWindow_New::closeTheConversationResource);

    //语音回放
    //connect(ui->audioReplay,&QPushButton::clicked,this,&MainWindow_New::AudioReplay);
    //改变速率
    connect(data_Widget,&dataWidget::changeRate,InstructObject,&ClientSocket::changeRateInstruct);
}


void MainWindow_New::receiveRepliedMsgToShow(QString msg) {
    //data_Widget->receiveMsgBox->clear();
    //data_Widget->receiveMsgBox->setText(msg);
    QString originalMsg = data_Widget->receiveMsgBox->toPlainText();
    data_Widget->receiveMsgBox->clear();
    QString nowMsg= msg + "\n" + originalMsg;
    data_Widget->receiveMsgBox->setText(nowMsg);  //以前是msg就好了，现在改为累加信息的模式就需要变了
}

void MainWindow_New::sendShortMsg(){
    if(data_Widget->sendMsgBox->toPlainText().size() == 0){
        QMessageBox::information(NULL, "Title", QStringLiteral("无法发送空信息"));
        return;
    }
    sendMsgToClientSocket(data_Widget->sendMsgBox->toPlainText());
    //把发送的内容显示在右边
    //data_Widget->sendedMsgBox->clear();
    //data_Widget->sendedMsgBox->setText(data_Widget->sendMsgBox->toPlainText());
    QString originalMsg = data_Widget->sendedMsgBox->toPlainText();
    data_Widget->sendedMsgBox->clear();
    QString nowMsg= data_Widget->sendMsgBox->toPlainText() + "\n" + originalMsg;
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
        //开启 ffmpeg 和 ffplay 两个进程   //假如是飞机端，只开ffplay就可以了

        //QString command0 = "ffmpeg.exe -f dshow -i video=hm1091_techfront -vcodec libx264  -r 15  -b:v  150k  -s  352*288  -preset:v ultrafast  -tune:v zerolatency -f rtp  rtp://127.0.0.1:6666>sd.sdp";
        QString command0 = "ffmpeg.exe -f dshow -i video=\"" + cameraName + "\" -vcodec libx264  -r 15  -b:v  150k  -s  352*288  -preset:v ultrafast  -tune:v zerolatency -f rtp  rtp://"+
                SERVER_ADRESS + ":" + QString::number(VIDEO_RTP_PORT) +">sd.sdp";
        ffmpegProcess->setCreateProcessArgumentsModifier([] (QProcess::CreateProcessArguments *args)
        {
            //  这参数在 Windows.h 头文件里了，
            args->flags |= CREATE_NEW_CONSOLE;
            args->startupInfo->dwFlags &= ~STARTF_USESTDHANDLES;
        });
        ffmpegProcess->start(command0);

        //创建读取的sdp
        //constructVideoSdpFile() ;


        ffplayProcess->setCreateProcessArgumentsModifier([] (QProcess::CreateProcessArguments *args)
        {
            //  这参数在 Windows.h 头文件里了，
            args->flags |= CREATE_NEW_CONSOLE;
            args->startupInfo->dwFlags &= ~STARTF_USESTDHANDLES;
        });
        //constructSdpFile(); //这个函数需要更改参数
        //QString command = "ffplay.exe -protocol_whitelist file,http,https,rtp,udp,tcp,tls " + QDir::currentPath() + "/aaa.sdp";
        QString command = "ffplay.exe -protocol_whitelist file,http,https,rtp,udp,tcp,tls " + QCoreApplication::applicationDirPath();
        if(client_side == "GROUND"){
            command = command + "/ground.sdp";
        } else if (client_side == "LEFT") {
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


void MainWindow_New::closeTheConversationResource()
{
    if(ffplayProcess->isOpen()) {
        ffplayProcess->close();
    }
    if(ffmpegProcess->isOpen()) {
        ffmpegProcess->close();
    }
}

void MainWindow_New::closeTheConversation()
{

    InstructObject->hangUpthePhoneInstruct();
    //关闭 ffmpeg 和 ffplay 两个进程   //假如是飞机端，那就值关闭ffplay就可以了
    if(ffmpegProcess->isOpen()) {
        ffmpegProcess->close();
    }
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

    QProcess *mkScrip = new QProcess;
    QProcess *executeFtp = new QProcess;

    mkScrip->setCreateProcessArgumentsModifier([] (QProcess::CreateProcessArguments *args)
    {
        //  这参数在 Windows.h 头文件里了，
        args->flags |= CREATE_NEW_CONSOLE;
        args->startupInfo->dwFlags &= ~STARTF_USESTDHANDLES;
    });
    QString command = QCoreApplication::applicationDirPath() +  "/create.bat  " + filepath;  //ftp.exe
    qDebug()<<command;
    mkScrip->setWorkingDirectory(QCoreApplication::applicationDirPath());
    mkScrip->start(command);
    mkScrip->waitForFinished();
    command = "ftp -n -s:" +  QCoreApplication::applicationDirPath() + "/ftp.bat\n";
    executeFtp->setCreateProcessArgumentsModifier([] (QProcess::CreateProcessArguments *args)
    {
        //  这参数在 Windows.h 头文件里了，
        args->flags |= CREATE_NEW_CONSOLE;
        args->startupInfo->dwFlags &= ~STARTF_USESTDHANDLES;
    });
    executeFtp->start(command);
}

// 保存音频文件
void MainWindow_New::saveAudioFile()
{
    //
    qDebug()<<QStringLiteral("语音留言");
    if(data_Widget->saveAudioButton->isChecked()){ //开始录音
        data_Widget->saveAudioButton->setStyleSheet("color: red");
        data_Widget->saveAudioButton->setText(QStringLiteral("正在录音，结束请再次点击"));
        //录音代码
        audioFile =new QFile();
        audioFile->setFileName( QCoreApplication::applicationDirPath() + QStringLiteral("/testFile/语音留言.pcm"));//设置其实设置音频文件的存放路径(输入音频名及存放路径)
        bool is_open = audioFile->open(QIODevice::WriteOnly | QIODevice::Truncate);
        if(!is_open)
        {
            QMessageBox::information(NULL, "Title", QStringLiteral("保存出现问题"));
        }
        QAudioFormat format;
        format.setSampleRate(8000); //设置采样的赫兹
        format.setChannelCount(1); //设置通道数通道
        format.setSampleSize(8);   //设置样本大小，一般为8或者16
        //上面的要从16位 liner pcm 压缩为8位的 ulaw
        format.setCodec("audio/pcm");//设置编解码器
        //format.setCodec("audio/G711-0");//设置编解码器
        format.setByteOrder(QAudioFormat::LittleEndian);//
        format.setSampleType(QAudioFormat::UnSignedInt);//
        QAudioDeviceInfo info = QAudioDeviceInfo::defaultInputDevice();
        QString str = info.deviceName();
        qDebug()<<QStringLiteral("使用的录音设备是:")<<str;
        if(!info.isFormatSupported(format))
        {
            format = info.nearestFormat(format);
        }
        audioInput = new QAudioInput(format, this);

        audioInput->start(audioFile);

    } else {  //暂停录音
        data_Widget->saveAudioButton->setStyleSheet("");
        data_Widget->saveAudioButton->setText(QStringLiteral("语音留言"));
        audioInput->stop();
        audioFile->close();
    }
}

////语音回放
//void MainWindow_New::AudioReplay()
//{
//    QString audioFileName = QCoreApplication::applicationDirPath() + "/testFile/savedAudio.pcm";
//    QFile* audiofile = new  QFile;
//    audiofile->setFileName(audioFileName);
//    bool isopen = audiofile->open(QIODevice::ReadOnly);
//    if(!isopen) {
//        QMessageBox::information(NULL, "Title", QStringLiteral("打开文件失败，没有收到该文件"));
//        return;
//    }
//    QAudioFormat format;
//    format.setSampleRate(8000);
//    format.setChannelCount(1);
//    format.setSampleSize(8);   //从8位 mlaw转化为16的可以
//    format.setCodec("audio/pcm");
//    //format.setCodec("audio/G711-0");//设置编解码器
//    format.setByteOrder(QAudioFormat::LittleEndian);
//    format.setSampleType(QAudioFormat::UnSignedInt);
//    audioOutput = new QAudioOutput(format);
//    audioOutput->start(audiofile);
//}


void MainWindow_New::sendFileNameToGo(QListWidgetItem *item)
{
    QString fileName = item->text();
    emit sendFileNameToClientSocket(fileName);
}

void MainWindow_New::receiveSocketReadyFileNameToShow(QString fileName)
{
    QListWidgetItem* item = new QListWidgetItem;
    item->setText(fileName);
    data_Widget->filelistWidget->addItem(item);
}


void MainWindow_New::setPhoneNumberText(QString phoneNum)
{
    InstructObject->phoneNumber = phoneNum;
}

void MainWindow_New::setMsgText(QString Msg)
{
    InstructObject->phoneNumber = Msg;
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


void MainWindow_New::showIfNetIsConnectedOrNot(bool IsConnected)
{
    connectState = IsConnected;
    if(IsConnected == NET_IS_CONNECTED) {
        media_Widget->showConnectedOrNot->setText(QStringLiteral("已链接"));
    } else if (IsConnected == NET_IS_NOT_CONNECTED) {
        media_Widget->showConnectedOrNot->setText(QStringLiteral("未链接"));
    }
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





































