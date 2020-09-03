#include "clientsocket.h"



ClientSocket::ClientSocket(QObject *parent) : QObject(parent)
{

    init();
}

void ClientSocket::init()
{
    //流水号
    phoneWaterNumber = 1;
    msgWaterNumber = 1;
    changeRateNumber = 1;

    //初始话两个播放源



    QSettings* settings = new QSettings(QCoreApplication::applicationDirPath()+"/ground_config.ini", QSettings::IniFormat);

    QVariant var = settings->value("INSTRUCT_FREQUENCY/GAP_TIME");
    circleTIME = var.toInt()* 1000;  //设置间隔时间

    dc_side = Which_Client::GROUND;

    instructHead = "AA55";
    var = settings->value("ADRESS/LAND_SERVER_ADRESS");
    SERVER_ADRESS = var.toString();
    QVariant port = settings->value("LAND_PORT/INSTRUCT_PORT");
    INSTRUCT_PORT = port.toInt();  //读取配置
    Conversation_MODE = FREE_STATE; //开始的时候是自由模式
    var = settings->value("RECIVE_MSG/ADDRESS","RYHBWHXC");
    //收报信息
    receiveAddress = var.toString();
    initTimer();
    initInstructSocket();
    //这个类的主要要处理的地方是   各种状态的判断及确定！，只要把交流定义好了，flag设立好了，就Ok了！
}

//init timer把所有的timer都 new在这里，不要发一次指令new一次，那样不是很好
void ClientSocket::initTimer()
{
    videoInstructTimer = new QTimer();
    JtongSelfCheckTimer = new QTimer();
    closeInstructTimer = new QTimer();
    circle_workSendTimer  = new QTimer();
    msSelfCheckTimer = new QTimer();
    msControlTimer = new QTimer();
    unAnsweredPhoneTimer = new QTimer();

    //timer 相关的函数
    connect(closeInstructTimer,&QTimer::timeout,this,&ClientSocket::sendCloseSystemInstruct);
    connect(videoInstructTimer,&QTimer::timeout,this,&ClientSocket::sendVideoCallRequest);
    connect(JtongSelfCheckTimer,&QTimer::timeout,this,&ClientSocket::JtongSelfCheckMsg);
    connect(circle_workSendTimer,&QTimer::timeout,this,&ClientSocket::everyCircleWork);
    connect(unAnsweredPhoneTimer,&QTimer::timeout,this,&ClientSocket::unAnsweredPhoneSignal);
    connect(msControlTimer,&QTimer::timeout,this,&ClientSocket::mSatelliteControlInstruct);
    connect(msSelfCheckTimer,&QTimer::timeout,this,&ClientSocket::mSSelfcheckinstruct);

    //////////////////////////////////////////////////////////////////////////////

    phoneTimer = new QTimer;
    statusTimer = new QTimer;
    msgTimer = new QTimer;

    phoneTimer->setInterval(1000);
    statusTimer->setInterval(1000);
    msgTimer->setInterval(1000);

    phoneTimer->setSingleShot(true);
    statusTimer->setSingleShot(true);
    msgTimer->setSingleShot(true);

    phoneWaterNumber_01 = 0;
    statusWaterNumber_01 = 0;
    msgWaterNumber_01 = 0;

    connect(phoneTimer,&QTimer::timeout,this,[=](){
        phoneWaterNumber_01 = 0;
    });
    connect(statusTimer,&QTimer::timeout,this,[=](){
        statusWaterNumber_01 = 0;
    });
    connect(msgTimer,&QTimer::timeout,this,[=](){
        msgWaterNumber_01 = 0;
    });


}



//发送视频申请
void ClientSocket::videoCallRequestManager()
{
    //目前改为三次了，把timer注释了
    //videoInstructTimer->setInterval(5000);  //设为5秒
    sendVideoCallRequest();
    //videoInstructTimer->start();
    //播放电话效果  //暂时没有好的展示效果所以放弃
    //showPhoneGif();
}


void ClientSocket::changeRateInstruct(int r){

    QByteArray changeRate;
    //EN-CIDC-03  改变周期    // 这个数需要修改，先随便改一个值改为05
    changeRate.append(QByteArray::fromHex(instructHead));
    //帧标识
    changeRate.append(QByteArray::fromHex("05"));
    //帧长度 27
    short length = 11;
    length = BLEndianShort(length);
    QByteArray lengthByte = ShortToByte(length);
    changeRate.append(lengthByte);
    //周期
    if(r == 1) {
        changeRate.append(QByteArray::fromHex("01"));
    } else if(r == 2) {
        changeRate.append(QByteArray::fromHex("0A"));
    } else if(r == 3) {
        changeRate.append(QByteArray::fromHex("3C"));
    }

    //流水号
    changeRate.append(IntToByte(BLEndianInt(changeRateNumber)));
    //校验和
    uchar sum = 0;
    for(uchar oneByte:changeRate.mid(2,changeRate.size() -2 )) {
        sum += oneByte;
    }
    changeRate.append(sum);

    instructSocket->writeDatagram(changeRate,QHostAddress(SERVER_ADRESS),INSTRUCT_PORT);
    instructSocket->writeDatagram(changeRate,QHostAddress(SERVER_ADRESS),INSTRUCT_PORT);
    instructSocket->writeDatagram(changeRate,QHostAddress(SERVER_ADRESS),INSTRUCT_PORT);
    incrementWaterNumber(changeRateNumber);
}


void ClientSocket::sendVideoCallRequest()
{


    QByteArray videoRequst;
    //EN-GCSCI-03 申请
    videoRequst.append(QByteArray::fromHex(instructHead));
    //帧标识
    videoRequst.append(QByteArray::fromHex("03"));
    //帧长度
    short length = 35;
    length = BLEndianShort(length);
    QByteArray lengthByte = ShortToByte(length);
    videoRequst.append(lengthByte);
    //发起人地址,和收信人地址都配置成一个了
    videoRequst.append(receiveAddress);
    //电话号码，默认为 16位
    if(this->phoneNumber.size() >= 16) {
        //把电话号码转化为ascii码

        videoRequst.append(this->phoneNumber.mid(0,16));
    } else {
        while(this->phoneNumber.size() != 16){
            this->phoneNumber = this->phoneNumber + "0";
        }

        videoRequst.append(this->phoneNumber);
    }
    //申请视频通话
    videoRequst.append(QByteArray::fromHex("01"));
    //流水号
    videoRequst.append(IntToByte(BLEndianInt(phoneWaterNumber)));
    //校验和
    uchar sum = 0;
    for(uchar oneByte:videoRequst.mid(2,videoRequst.size() -2 )) {
        sum += oneByte;
    }
    videoRequst.append(sum);

    instructSocket->writeDatagram(videoRequst,QHostAddress(SERVER_ADRESS),INSTRUCT_PORT);
    instructSocket->writeDatagram(videoRequst,QHostAddress(SERVER_ADRESS),INSTRUCT_PORT);
    instructSocket->writeDatagram(videoRequst,QHostAddress(SERVER_ADRESS),INSTRUCT_PORT);
    incrementWaterNumber(phoneWaterNumber);
}



void ClientSocket::initInstructSocket()   //监听
{

    instructSocket = new QUdpSocket(this);
    bool isBinding =   instructSocket->bind(INSTRUCT_PORT);
    connect(instructSocket, &QUdpSocket::readyRead, this, &ClientSocket::instructUdpSocketReceive);
}






void ClientSocket::instructUdpSocketReceive()  //接收反馈处理信息
{
    //ClientMode = TALKING_MODE;
    //反馈回来的 指令
    QByteArray ba;
    while(instructSocket->hasPendingDatagrams())
    {
        ba.resize(instructSocket->pendingDatagramSize());
        instructSocket->readDatagram(ba.data(), ba.size());
        checkMsg(ba);   //试一试看OK不  //检查收到的内容
    }

}

//每条回复的指令开始都要用校验值验证一下 另外还需要看其长度，两者值是否正确
void ClientSocket::checkMsg(QByteArray msg)    //检测收到的信息，然后做出判断
{

    if((msg == "")||(msg.size()<=3)) {
        qDebug()<<QStringLiteral("断点1，空指令或指令长度小于三");  //之后或许抛出异常 之后添加关于长度的检验
        //
        return;
    }

    bool length =  checkInstructLength(msg);
    bool checkSum = checkCheckSumNum(msg);
    if(!length && checkSum) {
        return;
    }

    //接收指令，判断
    if( msg.at(2)==0x12){
        //短信
        msgTimer->start();

        if( msgWaterNumber_01 != BLEndianInt(ByteArrayToInt( msg.mid(63,4))) ) {
            msgWaterNumber_01 = BLEndianInt(ByteArrayToInt( msg.mid(63,4)));
        } else   {
            return;
        }
        receiveShortMsgAndShow(msg);

    } else   if( msg.at(2)==0x02){
        //被叫方
        phoneTimer->start();

        if( phoneWaterNumber_01 != BLEndianInt(ByteArrayToInt( msg.mid(35,4))) ) {
            phoneWaterNumber_01 = BLEndianInt(ByteArrayToInt( msg.mid(35,4)));
        } else
        {
            return;
        }
        if(msg.at(10) == 0x01)   //下面是发送，接不接电话
        {
            receiveSomeVideoCallOrNot(msg);  //因为是被叫方所以在函数内选择发送的指令
        }
        if(msg.at(10) == 0x02)   //挂电话  控制MainWindow 内开的音视频资源关了
        {
            Conversation_MODE = FREE_STATE;
            closeMainWindowResource();
        }
    } else   if( msg.at(2)==0x1a){

        //视频通话状态包
        //CIDC-04 返回接收否    //作为主叫方，看人家接不接电话
        statusTimer->start();
        if( statusWaterNumber_01 != BLEndianInt(ByteArrayToInt( msg.mid(11,4)))) {
            statusWaterNumber_01 = BLEndianInt(ByteArrayToInt( msg.mid(11,4)));
        } else {
            return;
        }
        //videoInstructTimer->stop(); // 接到回信，所以就不继续呼叫了，把计时器暂停了
        receiveVideoCallOrNot(msg);  //作为主叫方收到对方的应答  //这里需要改
    } else   if(msg.at(2)==0x06) {

        //长度28
        //这里是 接收 海事自检上报，与上方相同，定义好了需修改   //需显示
        showMsSelfcheckReply(msg);

    } else   if(msg.at(2)==0x07){

        //if(msg.size() ==10)
        showMsControlReply(msg);   //指令都固定后统一加长度校验
        //else
        //qDebug()<<"length is not right";
    } else   if(msg.at(2)== 0x08) {

        QString name = msg.mid(6,40);
        //name = name.replace(' ','');
        sendReadyFilenameToMainWindow(name);
    } else   if(msg.at(2)==0x66) {

        //EN-CIDC-06  机通显示   //机通自检，可能以后不需要了
        receiveJtongselfcheckReply();  //内容都没有，可能需要过一阵子才能完成
    }

}


//收到文件已经准备好回复，播放文件
void ClientSocket::playAudioRecord(){
    //E://testFile/nihao.pcm    播放的为发送过来的文件
    audioFileName = QCoreApplication::applicationDirPath()  + QStringLiteral("/testFile/语音留言.pcm");
    audiofile = new  QFile;
    audiofile->setFileName(audioFileName);
    bool isopen = audiofile->open(QIODevice::ReadOnly);
    if(!isopen)
    {
        QMessageBox::information(NULL, "Title", QStringLiteral("没有找到该文件"));
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
    connect(audioOutput,&QAudioOutput::stateChanged,this,&ClientSocket::handleAudioFileStateChanged);
    audioOutput->start(audiofile);
    //    QAudioDeviceInfo  info = QAudioDeviceInfo::defaultInputDevice();
    //    for(QString i :info.supportedCodecs()) {
    //        qDebug()<<i;
    //    }
}

//收到机通 回复，然后处理
void ClientSocket::receiveJtongselfcheckReply()
{
    JtongSelfCheckTimer->stop();  //既然已经得到回复了，直接就停止就可以了
    //处理，并显示  //这里没内容，之后在商议内容
}

//看看人家接不接你电话
void ClientSocket::receiveVideoCallOrNot(QByteArray instruct)
{

    //判断对方是否同意   //默认为不存在只开视频 不开音频这种行为
    if(instruct.at(10) == 0x01) {
        //Conversation_MODE = PHONING_STATE;
        //接收  //打开视频 和 音频   默认为音视频都开的
        openConversation(VIDEOCHAT);
    } else {
        Conversation_MODE = FREE_STATE;
        //提示拒绝
        QMessageBox::information(NULL, "Title", QStringLiteral("对方拒接"));
    }

}


void ClientSocket::unAnsweredPhoneSignal(){
    //发一次就不发了
    unAnsweredPhoneTimer->stop();
    if(unasweredPhoneNumber.size() != 16) {
        QMessageBox::information(NULL, "Title", QStringLiteral("电话号码不足16位"));
        return;
    }
    emitUnAnswerPhone(unasweredPhoneNumber);
    //这个时候可能应该把那个框子关死
}

// 接到对方来电，显示一个接收或者不接收的框子，来选择 来电方式，这里是被叫方 ，然后发送指令
void ClientSocket::receiveSomeVideoCallOrNot(QByteArray msg)
{
    //关的时候要恢复 FREE STATE,  有两处需要设， 一个是我们自己挂的时候， 一个是人家挂的时候

    if(Conversation_MODE != FREE_STATE)
        return;

    //先设定15秒(需求是60秒) 没接电话的情况下， 激活信号，  使机通发送未接来电 假如是地面端就不用发送了


    Conversation_MODE = CALLING_STATE;

    reciveMessagebox = new QMessageBox(QMessageBox::NoIcon,QStringLiteral("提示"),QStringLiteral("是否接听电话"));

    okBtn = new  QPushButton(QStringLiteral("视频通话"));
    closeBtn = new  QPushButton(QStringLiteral("拒接"));

    connect(okBtn,&QPushButton::clicked,this,&ClientSocket::OkToGroundCallInstruct);
    connect(closeBtn,&QPushButton::clicked,this,&ClientSocket::NotOkToGroundCallInstruct);

    reciveMessagebox->addButton(okBtn,QMessageBox::AcceptRole);
    reciveMessagebox->addButton(closeBtn,QMessageBox::RejectRole);

    reciveMessagebox->setWindowIcon(QIcon(":/plane.png"));
    QTimer::singleShot(PHONE_TIME,reciveMessagebox,SLOT(close()));  //15秒就关闭，需测试
    reciveMessagebox->show();

}


//展现电话效果
void ClientSocket::showPhoneGif()
{
    //之后这种状态栏还是需要改
    movie = new QMovie(":/phone.gif");
    statusLabel = new QLabel();
    statusLabel->setMovie(movie);
    statusLabel->setFixedSize(200,200);
    statusLabel->show();
    movie->start();
}


//控制关机函数，循环三次
void ClientSocket::closeSystem()
{
    closeInstructTimer->setInterval(1000);
    closeInstructTimer->start();

}

//这个才是发送的关机指令
void ClientSocket::sendCloseSystemInstruct()
{
    QByteArray closeInstruct;
    closeInstruct.append(QByteArray::fromHex(instructHead));
    //16
    closeInstruct.append(QByteArray::fromHex("10"));
    closeInstruct.append(QByteArray::fromHex("06"));
    //closeInstruct.append(QByteArray::fromHex("0000000001"));
    closeInstruct.append(QByteArray::fromHex("01"));
    //校验和
    char sum = 0;
    for(char i:closeInstruct.mid(2,closeInstruct.size() - 2)) {
        sum += i;
    }
    closeInstruct.append(sum);
    instructSocket->writeDatagram(closeInstruct,QHostAddress(SERVER_ADRESS),INSTRUCT_PORT);
    //下面是记次数的，发送三次，就不发送了，结束了
    static int count = 0;
    count ++;
    qDebug()<<"static ";
    qDebug() <<count;
    if(3 == count) {
        count = 0 ;
        closeInstructTimer->stop();    //不在发送
    }
}

//管理上报自建重发的调控 EN-DCCI-27
void ClientSocket::JTongSelfCheckMsgManager()
{
    JtongSelfCheckTimer->setInterval(circleTIME);
    //发送一条，100ms没收到回信就继续发送
    JtongSelfCheckMsg();   //先设定一共就发三次
    JtongSelfCheckTimer->start();

}

//发送     还不知道调用方式，先这样  DCCI-27
void ClientSocket::JtongSelfCheckMsg()
{
    static int count = 0;
    count ++;
    if(count == 4){
        JtongSelfCheckTimer->stop();
        count = 0;
        QMessageBox::information(NULL, "Title", QStringLiteral("没有收到自检回复"));
    }

    QByteArray selfcheckInstruck;
    selfcheckInstruck.append(QByteArray(instructHead));
    selfcheckInstruck.append(QByteArray::fromHex("1B"));
    selfcheckInstruck.append(QByteArray::fromHex("0B"));
    selfcheckInstruck.append(QByteArray::fromHex("0000000001"));
    //01 表示自检   N/A不知道是干啥
    selfcheckInstruck.append(QByteArray::fromHex("01"));
    //校验和
    char sum = 0;
    for(char i:selfcheckInstruck.mid(2,selfcheckInstruck.size() - 2)) {
        sum += i;
    }
    selfcheckInstruck.append(sum);
    instructSocket->writeDatagram(selfcheckInstruck,QHostAddress(SERVER_ADRESS),INSTRUCT_PORT);

}




//电话号码暂时为自我填写的内容
void ClientSocket::hangUpthePhoneInstruct()
{
    Conversation_MODE = FREE_STATE;

    //GCSCI-03
    QByteArray tellCIclosed;
    //帧头
    tellCIclosed.append(QByteArray::fromHex(instructHead));
    //帧标识
    tellCIclosed.append(QByteArray::fromHex("03"));
    //帧长度 27
    short length = 35;
    length = BLEndianShort(length);
    QByteArray lengthByte = ShortToByte(length);
    tellCIclosed.append(lengthByte);
    //发起人地址,和收信人地址都配置成一个了
    tellCIclosed.append(receiveAddress);
    //发信人地址，默认为 16位
    if(this->phoneNumber.size() >= 16) {
        tellCIclosed.append(this->phoneNumber.mid(0,16));
    } else {
        while(this->phoneNumber.size() != 16){
            this->phoneNumber = this->phoneNumber + "0";
        }
        tellCIclosed.append(this->phoneNumber);
    }
    //结束电话
    tellCIclosed.append(QByteArray::fromHex("02"));
    //流水号
    tellCIclosed.append(IntToByte(BLEndianInt(phoneWaterNumber)));
    //校验和
    uchar sum = 0;
    for(uchar oneByte:tellCIclosed.mid(2,tellCIclosed.size() -2 )) {
        sum += oneByte;
    }
    tellCIclosed.append(sum);
    instructSocket->writeDatagram(tellCIclosed,QHostAddress(SERVER_ADRESS),INSTRUCT_PORT);
    instructSocket->writeDatagram(tellCIclosed,QHostAddress(SERVER_ADRESS),INSTRUCT_PORT);
    instructSocket->writeDatagram(tellCIclosed,QHostAddress(SERVER_ADRESS),INSTRUCT_PORT);
    incrementWaterNumber(phoneWaterNumber);
}




//之后再考虑是否  循环的指令 和 单独发送的指令冲突不冲突
void ClientSocket::SendHomeWorkMsgManager()
{
    circle_workSendTimer->setInterval(circleTIME);

    everyCircleWork();

    circle_workSendTimer->start(); //

}

//DCCI-22 循环传播 一秒一个 作业信息
void ClientSocket::homeWorkMsgInstruct()
{
    qDebug() <<"HOMEWORK MSG";
    QByteArray homeworkinstruct;
    homeworkinstruct.append(QByteArray::fromHex(instructHead));
    //帧标识
    homeworkinstruct.append(QByteArray::fromHex("11"));
    //帧长度  按 123算的
    homeworkinstruct.append(QByteArray::fromHex("7b"));
    //序号 1- 99999循环， 转换一下
    homeworkinstruct.append("00001");
    //符号
    homeworkinstruct.append(",");
    // 飞机编号
    homeworkinstruct.append("ABCDE");
    //符号
    homeworkinstruct.append(",");   //16
    //日期
    QString date = giveCurrentDate();
    homeworkinstruct.append(date);
    //符号
    homeworkinstruct.append(",");
    //时间
    QString time = giveCurrentTime();
    homeworkinstruct.append(time);
    //符号
    homeworkinstruct.append(",");       //34
    //经度、纬度
    homeworkinstruct.append("113.82490");
    homeworkinstruct.append(",");
    homeworkinstruct.append("34.54030");
    //高度  5位前面不够 0 来凑
    homeworkinstruct.append(",");
    homeworkinstruct.append("03403");
    //航速 3位  不够0来凑
    homeworkinstruct.append(",");
    homeworkinstruct.append("170");
    homeworkinstruct.append(",");
    //航向 3位
    homeworkinstruct.append("045");
    homeworkinstruct.append(",");
    //温度 5 位  不够0来凑
    homeworkinstruct.append("-10.5");
    //湿度 3位  不够0凑
    homeworkinstruct.append(",");
    homeworkinstruct.append("089");
    homeworkinstruct.append(",");
    //焰条 11位
    homeworkinstruct.append("01210305010");
    homeworkinstruct.append(",");
    //焰弹 11位
    homeworkinstruct.append("01210305010");
    homeworkinstruct.append(",");
    //液氮 11位
    homeworkinstruct.append("01210305010");
    homeworkinstruct.append(",");
    //粉剂 11位
    homeworkinstruct.append("01210305010");
    char sum= 0;
    for(char i : homeworkinstruct.mid(2,homeworkinstruct.size() - 2))
    {
        sum += i;
    }
    homeworkinstruct.append(sum);
    instructSocket->writeDatagram(homeworkinstruct,QHostAddress(SERVER_ADRESS),INSTRUCT_PORT);
}


QString ClientSocket::giveCurrentDate()
{
    QDate date = QDate::currentDate();
    QString year = QString::number(date.year());
    QString month = QString::number(date.month());
    QString day = QString::number(date.day());
    if(month.size() == 1) {
        month = "0" + month;
    }
    if(day.size() == 1) {
        day = "0" + day;
    }
    QString entireDate = year + month + day;
    return entireDate;
}



QString ClientSocket::giveCurrentTime()
{
    QTime time = QDateTime::currentDateTime().time();
    QString hour = QString::number(time.hour());
    QString min = QString::number(time.minute());
    QString second = QString::number(time.second());
    if(hour.size() == 1) {
        hour = "0" + hour;
    }
    if(min.size() == 1) {
        min = "0" + min;
    }
    if(second.size() == 1) {
        second = "0" + second;
    }
    QString entireTime = hour + min + second;
    return entireTime;
}

//给出短信那里的时间， 2020,06,12-12,23,03     19个长度的  给出当前的日期和时间
QString ClientSocket::giveCurrentDateAndTime(){
    QDate date = QDate::currentDate();
    QString year = QString::number(date.year());
    QString month = QString::number(date.month());
    QString day = QString::number(date.day());
    if(month.size() == 1) {
        month = "0" + month;
    }
    if(day.size() == 1) {
        day = "0" + day;
    }
    QString entireDate = year + "-" + month + "-" + day;

    QTime time = QDateTime::currentDateTime().time();
    QString hour = QString::number(time.hour());
    QString min = QString::number(time.minute());
    QString second = QString::number(time.second());
    if(hour.size() == 1) {
        hour = "0" + hour;
    }
    if(min.size() == 1) {
        min = "0" + min;
    }
    if(second.size() == 1) {
        second = "0" + second;
    }
    QString entireTime = hour + ":" + min + ":" + second;

    QString entireDateAndTime = entireDate + " " + entireTime;

    return entireDateAndTime;
}



//发送信息   短信  GCSCI-04
void ClientSocket::sentShortMsgInstruct(QString msg)
{
    QByteArray shortMsgInstruct;
    shortMsgInstruct.append(QByteArray::fromHex(instructHead));

    shortMsgInstruct.append(QByteArray::fromHex("04"));  //帧标识
    //长度改完后需修改  206  +  116
    short length  = 322;
    length = BLEndianShort(length);
    QByteArray  lengthByte  = ShortToByte(length);
    shortMsgInstruct.append(lengthByte);  //帧长度
    //发信人地址，默认为 20位
    if(this->MsgNumber.size() >= 20) {
        shortMsgInstruct.append(this->MsgNumber.mid(0,20));
    } else {
        while(this->MsgNumber.size() != 20){
            this->MsgNumber = this->MsgNumber + "0";
        }
        shortMsgInstruct.append(this->MsgNumber);
    }

    shortMsgInstruct.append(giveCurrentDateAndTime());   //发报时戳
    shortMsgInstruct.append(giveCurrentDateAndTime());   //编制时间？？
    //流水号
    shortMsgInstruct.append(IntToByte(BLEndianInt(msgWaterNumber)));

    shortMsgInstruct.append("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");  //ID 32
    shortMsgInstruct.append("abcd12");
    //发送信息
    shortMsgInstruct.append(msg.toUtf8());
    for(int i = msg.toUtf8().size(); i<200;i++) {
        shortMsgInstruct.append('\0');
    }
    //电话号码
    if(this->phoneNumber.size() >= 16) {
        shortMsgInstruct.append(this->phoneNumber.mid(0,16));
    } else {
        while(this->phoneNumber.size() != 16){
            this->phoneNumber = this->phoneNumber + "0";
        }
        shortMsgInstruct.append(this->phoneNumber);
    }
    //校验和
    char sum= 0;
    for(char i : shortMsgInstruct.mid(2,shortMsgInstruct.size() - 2)) {
        sum += i;
    }

    shortMsgInstruct.append(sum);

    instructSocket->writeDatagram(shortMsgInstruct,QHostAddress(SERVER_ADRESS),INSTRUCT_PORT);
    instructSocket->writeDatagram(shortMsgInstruct,QHostAddress(SERVER_ADRESS),INSTRUCT_PORT);
    instructSocket->writeDatagram(shortMsgInstruct,QHostAddress(SERVER_ADRESS),INSTRUCT_PORT);

    incrementWaterNumber(msgWaterNumber);
}



//授时帧  DCCI-02   //读到心跳发授时，  发一次就不发了
void ClientSocket::sendSystemTime()
{
    //qDebug()<<QDateTime::currentDateTime().date().year();
    qDebug()<< QStringLiteral("发送授时了");
    QByteArray sendTime;
    sendTime.append(QByteArray::fromHex(instructHead));
    //帧表示
    sendTime.append(QByteArray::fromHex("02"));
    //17
    sendTime.append(QByteArray::fromHex("0c"));
    //    //飞机编号，随便写为00000000001
    //    sendTime.append(QByteArray::fromHex("0000000001"));
    //年
    QDate date = QDateTime::currentDateTime().date();
    //保存short
    short year = date.year();
    year = BLEndianShort(year);
    QByteArray bar = ShortToByte(year);

    sendTime.append(bar);
    //月
    char mouth = date.month();
    sendTime.append(mouth);
    //日
    char day = date.day();
    sendTime.append(day);
    //时
    QTime time = QDateTime::currentDateTime().time();
    char hour =  time.hour();
    sendTime.append(hour);
    char min = time.minute();
    sendTime.append(min);
    char second = time.second();
    sendTime.append(second);
    //校验
    char sum = 0;
    for(char i :sendTime.mid(2,sendTime.size() -2)) {
        sum += i;
    }
    sendTime.append(sum);
    instructSocket->writeDatagram(sendTime,QHostAddress(SERVER_ADRESS),INSTRUCT_PORT);
}


void ClientSocket::receiveShortMsgAndShow(QByteArray replyMsg){
    QByteArray msg = replyMsg.mid(105,100);
    QTextCodec *tc = QTextCodec::codecForName("utf-8");
    QString ok  = tc->toUnicode(msg.data());   //转化为能显示的中文信息
    sendRepliedMsgtoMainWindow(ok);
}

//海事卫星链路状态自检管理
void ClientSocket::mSatelliteSelfcheckManager()
{
    msSelfCheckTimer->setInterval(circleTIME);  //假如没收到回复的信息就重新发送，收到了停止发送，显示信息
    mSSelfcheckinstruct();
    msSelfCheckTimer->start();  //收到回复或者发三次无回应则停止
}

//海事卫星链路状态自检指令
void ClientSocket::mSSelfcheckinstruct()
{
    static int count = 0;
    count ++;
    if(count == 4){
        msSelfCheckTimer->stop();
        QMessageBox::information(NULL, "Title", QStringLiteral("没有收到回复"));
        count = 0;
    }
    QByteArray MSselfcheck;   //EN-DCCI-24
    MSselfcheck.append(QByteArray::fromHex(instructHead));
    //帧标识
    MSselfcheck.append(QByteArray::fromHex("18"));
    // 帧长度
    MSselfcheck.append(QByteArray::fromHex("0b"));
    //飞机编号，先默认取一个值
    MSselfcheck.append(QByteArray::fromHex("0000000001"));
    //咨询指令
    MSselfcheck.append(QByteArray::fromHex("01"));
    //校验和
    char sum= 0;
    for(char i: MSselfcheck.mid(2,MSselfcheck.size() -2)) {
        sum += i;
    }
    MSselfcheck.append(sum);
    instructSocket->writeDatagram(MSselfcheck,QHostAddress(SERVER_ADRESS),INSTRUCT_PORT);
}

//把接收到海事卫星自检上报的信息显示出来
void ClientSocket::showMsSelfcheckReply(QByteArray msg)
{
    msSelfCheckTimer->stop();    //收到回复，停止重发
    //下面为显示内容
    QString showText = "设备状态: ";
    if(msg.at(4) == 0x1)
        showText.append("正常");
    if(msg.at(4) == 0x2)
        showText.append("故障\n");
    showText.append("建链状态: ");
    if(msg.at(5) == 0x1)
        showText.append("建链");
    if(msg.at(5) == 0x2)
        showText.append("断开\n");
    showText.append("信号质量: ");
    int charToint = msg.at(6);  //此方式待验证
    showText.append(QString::number(charToint));
    showText.append(".");
    charToint = msg.at(7);
    showText.append(QString::number(charToint));
    showText.append("\n数据连接时长: ");
    charToint = msg.at(8);
    showText.append(QString::number(charToint));
    showText.append(".");
    charToint = msg.at(9);
    showText.append(QString::number(charToint));
    showText.append(".");
    charToint = msg.at(10);
    showText.append("上传速度 : ");
    showText.append(QString::number(charToint));
    showText.append("\n");
    short speed = ByteArrayToShort(msg.mid(11,2)); //可能需要转换
    showText.append(QString::number(speed));
    showText.append("\n");
    showText.append("下载速度: ");
    speed = ByteArrayToShort(msg.mid(13,2)); //可能需要转换
    showText.append(QString::number(speed));
    showText.append("\n");
    showText.append("总发送数据: ");
    int data = ByteArrayToInt(msg.mid(15,4)); //可能需要转换
    showText.append(QString::number(data));
    showText.append("\n总接收数据: ");
    data = ByteArrayToInt(msg.mid(19,4));
    showText.append(QString::number(data));
    //    showText.append("\n故障代码:");
    //    data = msg.mid(23,4);  //最后这个代码待定
    sendMsSCMsgtoMwtoShow(showText);
}


//EN-DCCI-25
void ClientSocket::mSatelliteControlManager(bool Islinked,char Netstats) {
    this->islink = Islinked;
    this->netstats = Netstats;
    msControlTimer->setInterval(circleTIME);  //假如没收到回复的信息就重新发送，收到了停止发送，显示信息
    mSSelfcheckinstruct();
    msControlTimer->start();  //收到回复或者发三次无回应则停止
}






//EN-DCCI-25  海事卫星链路控制指令数据帧
void ClientSocket::mSatelliteControlInstruct()
{
    static int count = 0;
    count ++;
    if(count == 4){
        msControlTimer->stop();
        QMessageBox::information(NULL, "Title", QStringLiteral("没有收到回复"));
        count = 0;
    }
    QByteArray controlMsg;
    controlMsg.append(QByteArray::fromHex(instructHead));
    //帧标识
    controlMsg.append(QByteArray::fromHex("19"));
    // 帧长度
    controlMsg.append(QByteArray::fromHex("0c"));
    //飞机编号，先默认取一个值
    controlMsg.append(QByteArray::fromHex("0000000001"));
    //建链 1表示建立，2表示拆链
    if(this->islink) {
        controlMsg.append(QByteArray::fromHex("01"));
    } else {
        controlMsg.append(QByteArray::fromHex("02"));
    }

    //0，1，2，3 分别表示网络状态
    controlMsg.append(this->netstats);
    //校验和
    char sum= 0;
    for(char i: controlMsg.mid(2,controlMsg.size() - 2)) {
        sum += i;
    }
    sum = sum & 0xFF;
    controlMsg.append(sum);
    instructSocket->writeDatagram(controlMsg,QHostAddress(SERVER_ADRESS),INSTRUCT_PORT);
}


void ClientSocket::showMsControlReply(QByteArray msg)
{
    QString showText;
    showText.append("链路连接状态: ");
    if(msg.at(4) == 0x01)
        showText.append("建链\n");
    if(msg.at(4) == 0x02)
        showText.append("断开链接\n");
    showText.append("带宽状态: ");
    if(msg.at(5) == 0x00)
        showText.append("共享宽带\n");
    if(msg.at(5) == 0x01)
        showText.append("64kbps独享宽带\n");
    if(msg.at(5) == 0x02)
        showText.append("128kbps独享宽带\n");
    if(msg.at(5) == 0x03)
        showText.append("384kbps独享宽带\n");
    sendMsControlMsgtoMwToShow(showText);
}



short ClientSocket::ByteArrayToShort(QByteArray length_2)
{
    short num;
    memcpy(&num,length_2.data(),sizeof(num));
    return num;
}

int ClientSocket::ByteArrayToInt(QByteArray length_4)
{
    int num;
    memcpy(&num,length_4.data(),sizeof(num));
    return num;
}


QByteArray ClientSocket::ShortToByte(short num){
    QByteArray bar;
    bar.resize(2);
    memcpy(bar.data(),&num,sizeof(num));
    return bar;
}

QByteArray ClientSocket::IntToByte(int num){
    QByteArray bar;
    bar.resize(4);
    memcpy(bar.data(),&num,sizeof(num));
    return bar;
}

void ClientSocket::everyCircleWork()
{
    homeWorkMsgInstruct();
}

//DCCI-14 回音
void ClientSocket::sendAudioReplayInstructToCI(QString fileName)
{
    QByteArray instruct;
    //QByteArray::fromHex(instructHead)
    instruct.append(QByteArray::fromHex(instructHead));
    instruct.append(QByteArray::fromHex("14"));
    //帧长度 23  17位 17
    instruct.append(QByteArray::fromHex("2e"));
    instruct.append(fileName);
    int size = 40 - fileName.size(); //默认小于40
    for(int i = 0 ;i<size;i++) {
        instruct.append(QByteArray::fromHex("00"));
    }
    instruct.append(QByteArray::fromHex("00"));
    //校验和
    char sum= 0;
    for(char i: instruct.mid(2,instruct.size() - 2)) {
        sum += i;
    }
    instruct.append(sum);
    //qDebug() << "11111 : " +  instruct.toHex().toUpper();
    instructSocket->writeDatagram(instruct,QHostAddress(SERVER_ADRESS),INSTRUCT_PORT);
    //在下面清空一下

}



//检测指令长度
bool ClientSocket::checkInstructLength(QByteArray instruct)
{
    short size = instruct.size();
    //把这个变为  short

    // QByteArray byte_2 = instruct.mid(3,2);
    short length = ByteArrayToShort(instruct.mid(3,2));


    if(size != length) {

        length = BLEndianShort(length); //大小端转换。。。。
        if(size != length) {
            QMessageBox::information(NULL, "Title", QStringLiteral("显控指令长度报错"));
            qDebug()<<  instruct.toHex().toUpper() << "&" << size <<"&"<<length;
        }
    }
    return (size == length);
}





bool ClientSocket::checkCheckSumNum(QByteArray instruct)
{
    char sum = 0;
    //校验和
    for(char i: instruct.mid(2,instruct.size() - 3)) {
        sum += i;
    }
    if(sum != instruct.at(instruct.size() - 1)){
        QMessageBox::information(NULL, "Title", QStringLiteral("显控指令校验和报错"));
        return false;
    } else {
        return true;
    }
}

void ClientSocket::handleAudioFileStateChanged(QAudio::State newState)
{
    switch (newState) {
    case QAudio::IdleState:
        audioOutput->stop();
        audiofile->close();
        delete audioOutput;
        break;

    default:
        break;
    }
}


//short 转换大小端
short ClientSocket::BLEndianShort(short value)
{
    return ((value & 0x00FF) << 8 ) | ((value & 0xFF00) >> 8);
}


//更新流水号
void ClientSocket::incrementWaterNumber(int &number){
    if(number == 99999) {
        number = 1;
    } else {
        number ++;
    }
}

//转换大小端
uint ClientSocket::BLEndianInt(uint x)
{
    return (((uint)(x) & 0xff000000) >> 24) |  (((uint)(x) & 0x00ff0000) >> 8) | \
            (((uint)(x) & 0x0000ff00) << 8) | (((uint)(x) & 0x000000ff) << 24);
}


void ClientSocket::OkToGroundCallInstruct()
{

    //Conversation_MODE = PHONING_STATE;
    //GCSCI-02
    unAnsweredPhoneTimer->stop();
    //
    QByteArray ReceiveOrNot;
    ReceiveOrNot.append(QByteArray::fromHex(instructHead));
    //帧标识
    ReceiveOrNot.append(QByteArray::fromHex("02"));
    //帧长度  //帧长度需要修改
    short length = 36;
    length = BLEndianShort(length);
    QByteArray lengthByte = ShortToByte(length);
    ReceiveOrNot.append(lengthByte);
    //电话号码
    if(this->phoneNumber.size() >= 16) {
        //把电话号码转化为ascii码

        ReceiveOrNot.append(this->phoneNumber.mid(0,16));
    } else {
        while(this->phoneNumber.size() != 16){
            this->phoneNumber = this->phoneNumber + "0";
        }

        ReceiveOrNot.append(this->phoneNumber);
    }
    //接收地址
    ReceiveOrNot.append(receiveAddress);
    //视频
    ReceiveOrNot.append(QByteArray::fromHex("01"));
    //备用
    ReceiveOrNot.append(QByteArray::fromHex("00"));
    //流水号
    ReceiveOrNot.append(IntToByte(BLEndianInt(phoneWaterNumber)));
    //校验和
    char sum = 0;
    for(char i:ReceiveOrNot.mid(2,ReceiveOrNot.size() -2 )) {
        sum += i;
    }
    ReceiveOrNot.append(sum);
    instructSocket->writeDatagram(ReceiveOrNot,QHostAddress(SERVER_ADRESS),INSTRUCT_PORT);
    instructSocket->writeDatagram(ReceiveOrNot,QHostAddress(SERVER_ADRESS),INSTRUCT_PORT);
    instructSocket->writeDatagram(ReceiveOrNot,QHostAddress(SERVER_ADRESS),INSTRUCT_PORT);
    openConversation(VIDEOCHAT);
    incrementWaterNumber(phoneWaterNumber);

}




void  ClientSocket::NotOkToGroundCallInstruct()
{

    Conversation_MODE = FREE_STATE;
    //CIDC-04
    unAnsweredPhoneTimer->stop();
    //
    QByteArray ReceiveOrNot;
    ReceiveOrNot.append(QByteArray::fromHex(instructHead));
    //帧标识
    ReceiveOrNot.append(QByteArray::fromHex("02"));
    //帧
    short length = 36;
    length = BLEndianShort(length);
    QByteArray lengthByte = ShortToByte(length);
    ReceiveOrNot.append(lengthByte);
    //电话号码
    if(this->phoneNumber.size() >= 16) {
        //把电话号码转化为ascii码

        ReceiveOrNot.append(this->phoneNumber.mid(0,16));
    } else {
        while(this->phoneNumber.size() != 16){
            this->phoneNumber = this->phoneNumber + "0";
        }

        ReceiveOrNot.append(this->phoneNumber);
    }
    //接收地址
    ReceiveOrNot.append(receiveAddress);
    //视频
    ReceiveOrNot.append(QByteArray::fromHex("02"));
    //备用
    ReceiveOrNot.append(QByteArray::fromHex("00"));
    //流水号
    ReceiveOrNot.append(IntToByte(BLEndianInt(phoneWaterNumber)));
    //校验和
    char sum = 0;
    for(char i:ReceiveOrNot.mid(2,ReceiveOrNot.size() -2)) {
        sum += i;
    }
    ReceiveOrNot.append(sum);
    instructSocket->writeDatagram(ReceiveOrNot,QHostAddress(SERVER_ADRESS),INSTRUCT_PORT);
    instructSocket->writeDatagram(ReceiveOrNot,QHostAddress(SERVER_ADRESS),INSTRUCT_PORT);
    instructSocket->writeDatagram(ReceiveOrNot,QHostAddress(SERVER_ADRESS),INSTRUCT_PORT);
    incrementWaterNumber(phoneWaterNumber);

}


































