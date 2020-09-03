#include "clientsocket.h"



ClientSocket::ClientSocket(QObject *parent) : QObject(parent)
{
    init();
}

void ClientSocket::init()
{
    //初始化参数
    phoneWaterNumber = 1;
    statsWaterNumber = 1;

    closeSystemWaterNumber = 1;
    msgWaterNumber = 1;
    beidouWaterNumber = 1;
    beidouMsgWaterNumber = 1;
    skyGroundWaterNumber = 1;
    haishiControlWaterNumber = 1;
    gunAndPlaneWaterNumber = 1;

    audioReplyWaterNumber = 1;
    bosaWaterNumber = 1;


    backOrFrontChamber = FRONT_CHAMBER;
    QSettings* settings = new QSettings(QCoreApplication::applicationDirPath()+"/sky_config.ini", QSettings::IniFormat);
    QVariant var = settings->value("SIDE/CLIENTSIDE");
    QString side = var.toString ();
    var = settings->value("INSTRUCT_FREQUENCY/GAP_TIME");
    circleTIME = var.toInt()* 1000;  //设置间隔时间
    //settings->value("WEB/URL","http://192.168.0.104");
    //飞机编号
    var = settings->value("PLANE_NUMBER/NUM","00001");
    planeNumber = var.toString();

    if(side == "LEFT") {
        dc_side = Which_Client::LEFT;
    } else if(side == "RIGHT"){
        dc_side = Which_Client::RIGHT;
    }

    if(dc_side == Which_Client::LEFT) {
        instructHead = "AA87";
        QVariant var = settings->value("ADRESS/AIR_SERVER_ADRESS");
        SERVER_ADRESS = var.toString();
        INSTRUCT_PORT = 50101;
    }else if(dc_side == Which_Client::RIGHT) {
        instructHead = "AA88";
        QVariant var = settings->value("ADRESS/AIR_SERVER_ADRESS");
        SERVER_ADRESS = var.toString();
        INSTRUCT_PORT = 50111;
    }
    Conversation_MODE = FREE_STATE; //开始的时候是自由模式
    initTimer();
    initInstructSocket();
}

//init timer把所有的timer都 new在这里
void ClientSocket::initTimer()
{
    //下面的timer需要选择性删除
    //    videoInstructTimer = new QTimer();
    //    JtongSelfCheckTimer = new QTimer();
    //    closeInstructTimer = new QTimer();
    //    circle_workSendTimer  = new QTimer();
    //    skyGroundStatusTimer = new QTimer();
    //    msControlTimer = new QTimer();
    unAnsweredPhoneTimer = new QTimer();

    //timer 相关的函数
    //    connect(closeInstructTimer,&QTimer::timeout,this,&ClientSocket::sendCloseSystemInstruct);
    //    connect(videoInstructTimer,&QTimer::timeout,this,&ClientSocket::sendVideoCallRequest);
    //    connect(JtongSelfCheckTimer,&QTimer::timeout,this,&ClientSocket::stopWorkingInstruct);
    //    connect(circle_workSendTimer,&QTimer::timeout,this,&ClientSocket::everyCircleWork);
    connect(unAnsweredPhoneTimer,&QTimer::timeout,this,&ClientSocket::unAnsweredPhoneSignal);
    //connect(msControlTimer,&QTimer::timeout,this,&ClientSocket::gunAndFactoryInstrcut);
    // connect(skyGroundStatusTimer,&QTimer::timeout,this,&ClientSocket::skyGroundStatusInstruct);

    //////////////////////////////////////////////////////////////////////////////////////
    //之后需要把不用的代码删除掉

    receiveMsgTimer = new QTimer;            //02
    rateChangeTimer = new QTimer;            //03
    receivePhoneTimer = new QTimer;          //04
    receiveStatsTimer = new QTimer;          //05    //06为周期数据
    haishiTimer = new QTimer;                //07
    fileOkTimer = new QTimer;                //08
    unAnsweredTimer = new QTimer;            //09
    vocalReplyTimer = new QTimer;            //10
    beidouControlTimer = new QTimer;         //11
    receiveBeidouMsgTimer = new QTimer;      //12

    receiveMsgTimer->setInterval(1000);
    rateChangeTimer->setInterval(1000);
    receivePhoneTimer->setInterval(1000);
    receiveStatsTimer->setInterval(1000);
    haishiTimer->setInterval(1000);
    fileOkTimer->setInterval(1000);
    unAnsweredTimer->setInterval(1000);
    vocalReplyTimer->setInterval(1000);
    beidouControlTimer->setInterval(1000);
    receiveBeidouMsgTimer->setInterval(1000);

    receiveMsgTimer->setSingleShot(true);
    rateChangeTimer->setSingleShot(true);
    receivePhoneTimer->setSingleShot(true);
    receiveStatsTimer->setSingleShot(true);
    haishiTimer->setSingleShot(true);
    fileOkTimer->setSingleShot(true);
    unAnsweredTimer->setSingleShot(true);
    vocalReplyTimer->setSingleShot(true);
    beidouControlTimer->setSingleShot(true);
    receiveBeidouMsgTimer->setSingleShot(true);


    connect(receiveMsgTimer,&QTimer::timeout,this,[=](){
        msgWaterNumber_02 = 0;
    });
    connect(rateChangeTimer,&QTimer::timeout,this,[=](){
        rateWaterNumber_03 = 0;
    });
    connect(receivePhoneTimer,&QTimer::timeout,this,[=](){
        phoneWaterNumber_04 = 0;
    });
    connect(receiveStatsTimer,&QTimer::timeout,this,[=](){
        statusWaterNumber_05 = 0;
    });
    connect(haishiTimer,&QTimer::timeout,this,[=](){
        haishiWaterNunber_07 = 0;
    });
    connect(fileOkTimer,&QTimer::timeout,this,[=](){
        fileOkWaterNumber_08 = 0;
    });
    connect(unAnsweredTimer,&QTimer::timeout,this,[=](){
        unAnsweredWaterNumber_09 = 0;
    });
    connect(vocalReplyTimer,&QTimer::timeout,this,[=](){
        vocalReplyWaterNumber_0a = 0;
    });
    connect(beidouControlTimer,&QTimer::timeout,this,[=](){
        beidouControlWaterNumber_0b = 0;
    });

    connect(receiveBeidouMsgTimer,&QTimer::timeout,this,[=](){
        beidouMsgWaterNumber_0c = 0;
    });


    //把参数要在这里初始化一下
    msgWaterNumber_02 = 0;              //CIDC-02
    rateWaterNumber_03 = 0;             //CIDC-03
    phoneWaterNumber_04 = 0;            //CIDC-04
    statusWaterNumber_05 = 0;            //CIDC-05   //06为周期数据
    haishiWaterNunber_07 = 0;           //CIDC-07
    fileOkWaterNumber_08 = 0;           //CIDC-08
    unAnsweredWaterNumber_09 = 0;       //CIDC-09
    vocalReplyWaterNumber_0a = 0;       //CIDC-10
    beidouControlWaterNumber_0b = 0;    //CIDC-11
    beidouMsgWaterNumber_0c = 0;        //CIDC-12


}



//发送视频申请
void ClientSocket::videoCallRequestManager()
{

    //videoInstructTimer->setInterval(5000);  //设为10秒
    sendVideoCallRequest();
    //videoInstructTimer->start();
    //播放电话效果  //暂时没有好的展示效果所以放弃
    //showPhoneGif();
}


void ClientSocket::sendVideoCallRequest()
{
    //    static int videoRecallCounter = 0;
    //    videoRecallCounter ++;
    //    if(videoRecallCounter == 4) {
    //        videoInstructTimer->stop();
    //        QMessageBox::information(NULL, "Title", QStringLiteral("呼叫超时"));
    //        Conversation_MODE = FREE_STATE;
    //        videoRecallCounter = 0;
    //        //更新流水号
    //        //incrementWaterNumber(phoneWaterNumber);
    //    }

    QByteArray videoRequest;


    //帧头//   EN-DCCI-03
    videoRequest.append(QByteArray::fromHex(instructHead));
    //帧标识
    videoRequest.append(QByteArray::fromHex("03"));
    //帧长度 34
    short length = 34;
    length = BLEndianShort(length);
    QByteArray lengthByte = ShortToByte(length);
    videoRequest.append(lengthByte);
    //飞机编号，先默认取一个值
    videoRequest.append(planeNumber);

    if(dc_side == Which_Client::LEFT) {
        videoRequest.append(QByteArray::fromHex("01"));
    } else if(dc_side == Which_Client::RIGHT) {
        videoRequest.append(QByteArray::fromHex("02"));
    }
    //申请视频通话
    videoRequest.append(QByteArray::fromHex("01"));
    //前后仓
    if(backOrFrontChamber == FRONT_CHAMBER){
        videoRequest.append(QByteArray::fromHex("01"));
    } else if(backOrFrontChamber == BACK_CHAMBER) {
        videoRequest.append(QByteArray::fromHex("02"));
    }
    //申请人电话16 //
    if(this->phoneNumber.size() >= 16) {
        //videoRequest.append(this->phoneNumber.mid(0,16));
        QString num = this->phoneNumber.mid(0,16);
        for(int i = 0;i<16;i++) {
            videoRequest.append(num.at(i).toLatin1());
        }
    } else {
        while(this->phoneNumber.size() != 16){
            this->phoneNumber = this->phoneNumber + "0";
        }
        //videoRequest.append(this->phoneNumber);
        QString num = this->phoneNumber;
        for(int i = 0;i<16;i++) {
            videoRequest.append(num.at(i).toLatin1());
        }
    }
    //流水号
    videoRequest.append(IntToByte(BLEndianInt(phoneWaterNumber)));
    //校验和
    uchar sum = 0;
    for(uchar oneByte:videoRequest.mid(2,videoRequest.size()- 2)) {
        sum += oneByte;
        //qDebug()<< sum;
    }
    videoRequest.append(sum);

    instructSocket->writeDatagram(videoRequest,QHostAddress(SERVER_ADRESS),INSTRUCT_PORT);
    instructSocket->writeDatagram(videoRequest,QHostAddress(SERVER_ADRESS),INSTRUCT_PORT);
    instructSocket->writeDatagram(videoRequest,QHostAddress(SERVER_ADRESS),INSTRUCT_PORT);

    incrementWaterNumber(phoneWaterNumber);
}


//切换画面 todo
void ClientSocket::changeVideoCallRequest()
{

    QByteArray videoRequest;

    //帧头//   EN-DCCI-03
    videoRequest.append(QByteArray::fromHex(instructHead));
    //帧标识
    videoRequest.append(QByteArray::fromHex("03"));
    //帧长度 29   //需要改了
    short length = 34;
    length = BLEndianShort(length);
    QByteArray lengthByte = ShortToByte(length);
    videoRequest.append(lengthByte);
    //飞机编号，先默认取一个值
    videoRequest.append(planeNumber);

    if(dc_side == Which_Client::LEFT) {
        videoRequest.append(QByteArray::fromHex("01"));
    } else if(dc_side == Which_Client::RIGHT) {
        videoRequest.append(QByteArray::fromHex("02"));
    }
    //切换视频
    videoRequest.append(QByteArray::fromHex("03"));
    //前后仓
    if(backOrFrontChamber == FRONT_CHAMBER){
        videoRequest.append(QByteArray::fromHex("01"));
    } else if(backOrFrontChamber == BACK_CHAMBER) {
        videoRequest.append(QByteArray::fromHex("02"));
    }
    //申请人电话16 //
    if(this->phoneNumber.size() >= 16) {
        //videoRequest.append(this->phoneNumber.mid(0,16));
        QString num = this->phoneNumber.mid(0,16);
        for(int i = 0;i<16;i++) {
            videoRequest.append(num.at(i).toLatin1());
        }
    } else {
        while(this->phoneNumber.size() != 16){
            this->phoneNumber = this->phoneNumber + "0";
        }
        //videoRequest.append(this->phoneNumber);
        QString num = this->phoneNumber;
        for(int i = 0;i<16;i++) {
            videoRequest.append(num.at(i).toLatin1());
        }
    }
    //流水号
    videoRequest.append(IntToByte(BLEndianInt(phoneWaterNumber)));
    //校验和
    uchar sum = 0;
    for(uchar oneByte:videoRequest.mid(2,videoRequest.size()- 2)) {
        sum += oneByte;
        //qDebug()<< sum;
    }
    videoRequest.append(sum);

    instructSocket->writeDatagram(videoRequest,QHostAddress(SERVER_ADRESS),INSTRUCT_PORT);
    instructSocket->writeDatagram(videoRequest,QHostAddress(SERVER_ADRESS),INSTRUCT_PORT);
    instructSocket->writeDatagram(videoRequest,QHostAddress(SERVER_ADRESS),INSTRUCT_PORT);

    incrementWaterNumber(phoneWaterNumber);
}

void ClientSocket::setBackOrFrontChamber(int value)
{
    backOrFrontChamber = value;
}


//更新流水号
void ClientSocket::incrementWaterNumber(int &number){
    if(number == 99999) {
        number = 1;
    } else {
        number ++;
    }
}

void ClientSocket::initInstructSocket()   //初始化 并 监听
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
        qDebug()<<QStringLiteral("空指令1，端口不正确或未连接");
        //之后或许抛出异常 之后添加关于长度的检验
        return;
    }

    qDebug()<< QStringLiteral("收到事件指令:")<<msg.toHex().toUpper();

    bool length =  checkInstructLength(msg);
    bool checkSum = checkCheckSumNum(msg);
    if(!length && checkSum) {
        return;
    }



    if( msg.at(2)==0x2) {

        //海事短信
        receiveMsgTimer->start();
        if( msgWaterNumber_02 != BLEndianInt(ByteArrayToInt( msg.mid(63,4)))) {
            msgWaterNumber_02 = BLEndianInt(ByteArrayToInt( msg.mid(63,4)));
        }   else {
            return;
        }

        receiveShortMsgAndShow(msg);

    } else   if( msg.at(2)==0x0c){

        //北斗短信
        receiveBeidouMsgTimer->start();
        if( beidouMsgWaterNumber_0c != BLEndianInt(ByteArrayToInt( msg.mid(81,4)))) {
            beidouMsgWaterNumber_0c = BLEndianInt(ByteArrayToInt( msg.mid(81,4)));
        } else {
            return;
        }
        receiveShortMsgAndShow_beidou(msg);

    } else   if(msg.at(2)==0x03) {

        //上报地面指令，调节周期
        rateChangeTimer->start();
        if( rateWaterNumber_03 != BLEndianInt(ByteArrayToInt( msg.mid(6,4)))) {
            rateWaterNumber_03 = BLEndianInt(ByteArrayToInt( msg.mid(6,4)));
        } else {
            return;
        }
        char rate = msg.at(5);
        emit changeCircleRate(rate);

    } else   if( msg.at(2)==0x04){

        //视频通话状态包
        receivePhoneTimer->start();
        if( phoneWaterNumber_04 != BLEndianInt(ByteArrayToInt( msg.mid(6,4)))) {
            phoneWaterNumber_04 = BLEndianInt(ByteArrayToInt( msg.mid(6,4)));
        } else {
            return;
        }
        //videoInstructTimer->stop(); // 接到回信，所以就不继续呼叫了，把计时器暂停了
        receiveVideoCallOrNot(msg);  //作为主叫方收到对方的应答

    } else   if( msg.at(2)==0x05){

        //地面发起视频包
        receiveStatsTimer->start();

        if( statusWaterNumber_05 != BLEndianInt(ByteArrayToInt( msg.mid(22,4)))) {
            statusWaterNumber_05 = BLEndianInt(ByteArrayToInt( msg.mid(22,4)));

            //            if(Conversation_MODE != FREE_STATE)  //因为下面的弹出框， 所以这里的判断需要加一个
            //                return;

        } else {
            return;
        }

        if(msg.at(21)== 0x01){
            // 接不接电话
            receiveSomeVideoCallOrNot(msg);
            //因为是被叫方所以在函数内选择发送的指令
        }
        if(msg.at(21) == 0x02){
            //停止通话  控制MainWindow 内开的音视频资源关了
            Conversation_MODE = FREE_STATE;
            closeMainWindowResource();
        }

    }  else   if(msg.at(2)==0x06) {

        //这里是 接收 海事自检上报，与上方相同，定义好了需修改   //需显示
        emit showMslinkStatusReport(msg);

    }  else  if(msg.at(2)==0x07){

        //收到海事卫星控制回复   //需显示   //可能需要修改因为人家有备用内容
        haishiTimer->start();
        if( haishiWaterNunber_07 != BLEndianInt(ByteArrayToInt( msg.mid(10,4)))) {
            haishiWaterNunber_07 = BLEndianInt(ByteArrayToInt( msg.mid(10,4)));
        } else {
            return;
        }
        showMsControlReply(msg);

    }  else  if(msg.at(2)==0x08) {

        //EN-CIDC-08   //文件准备好！ //
        fileOkTimer->start();
        if( fileOkWaterNumber_08 != BLEndianInt(ByteArrayToInt( msg.mid(54,4)))) {
            fileOkWaterNumber_08 = BLEndianInt(ByteArrayToInt( msg.mid(54,4)));
        } else {
            return;
        }
        QString name = msg.mid(14,40);
        QString whiltSpace = " ";
        //请求的时候用空格去 把名字分开
        QString time = whiltSpace + giveCurrentTimeToShow();
        sendReadyFilenameToMainWindow(name + time);

    }  else  if(msg.at(2)==0x09){

        //EN-CIDC-09   todo  未接来电
        unAnsweredTimer->start();
        if( unAnsweredWaterNumber_09 != BLEndianInt(ByteArrayToInt( msg.mid(22,4)))) {
            unAnsweredWaterNumber_09 = BLEndianInt(ByteArrayToInt( msg.mid(22,4)));
        } else {
            return;
        }
        //逻辑
        QByteArray phoneNum = msg.mid(6,16);
        emit showUnansweredPhoneNumber(phoneNum);

    }  else   if(msg.at(2)==0x0a) {

        //语音回放数据帧
        vocalReplyTimer->start();
        if( vocalReplyWaterNumber_0a != BLEndianInt(ByteArrayToInt( msg.mid(28,4)))) {
            vocalReplyWaterNumber_0a = BLEndianInt(ByteArrayToInt( msg.mid(28,4)));
        } else {
            return;
        }
        //
        QString audiolist = msg.mid(5,16);
        audiolist = audiolist + " ";
        QString year =  QString::number(BLEndianShort( msg.mid(21,2).toShort()));
        QString month = QString::number(msg.at(23));
        QString day = QString::number(msg.at(24));

        QString hour = QString::number(msg.at(25));
        QString min = QString::number(msg.at(26));
        QString second = QString::number(msg.at(27));
        audiolist = audiolist +year +"-" + month + "-" + day + "," + hour + ":" + min + ":" + second;
        emit sendAudioListToMainWindow(audiolist);

    }  else  if(msg.at(2)==0x0b) {

        //北斗通道使用通知
        beidouControlTimer->start();
        if( beidouControlWaterNumber_0b != BLEndianInt(ByteArrayToInt( msg.mid(6,4)))) {
            beidouControlWaterNumber_0b = BLEndianInt(ByteArrayToInt( msg.mid(6,4)));
        } else {
            return;
        }

        //修改理解其中流程，并修改处理
        char ok = msg.at(5);  //1 开通  2 停止
        QString beidou;
        if(ok == 0x01) {
            beidou = QStringLiteral("通信控制设备开通北斗");
        } else {
            beidou = QStringLiteral("通信控制设备停止北斗");
        }
        QMessageBox* info = new  QMessageBox(QMessageBox::NoIcon,QStringLiteral("北斗通道使用通知！"),beidou);
        info->setStandardButtons(QMessageBox::Yes);

        info->setButtonText(QMessageBox::Yes,QStringLiteral("确定"));

        info->setWindowIcon(QIcon(":/plane.png"));

        info->show();

    }  else  if(msg.at(2)==0x66) {

        //EN-CIDC-06  机通显示   //机通自检，可能以后不需要了
        receiveJtongselfcheckReply();  //内容都没有，可能需要过一阵子才能完成
    }

}




//收到文件已经准备好回复，播放文件
void ClientSocket::playAudioRecord(){
    //E://testFile/nihao.pcm    播放的为发送过来的文件
    audioFileName = QCoreApplication::applicationDirPath()  + "/testFile/savedAudio.pcm";
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
    if(instruct.at(5) == 0x01) {
        //Conversation_MODE = PHONING_STATE;
        //接收  //打开视频 和 音频   默认为音视频都开的
        openConversation(VIDEOCHAT);
    }/* else if(instruct.at(6) == 0x01) {
        //Conversation_MODE = PHONING_STATE;
        //接收音频电话  //打开音频
        QMessageBox::information(NULL, "Title", QStringLiteral("对方申请语音通话"));
        openConversation(AUDIOCHAT);
    }*/ else {
        Conversation_MODE = FREE_STATE;
        //提示拒绝
        QMessageBox::information(NULL, "Title", QStringLiteral("对方拒接"));
    }

}


void ClientSocket::unAnsweredPhoneSignal(){
    //发一次就不发了
    Conversation_MODE = FREE_STATE;    //发完了未接来电就可以继续呼叫了
    unAnsweredPhoneTimer->stop();
    //    if(unansweredPhoneNumber.size() != 16) {
    //        QMessageBox::information(NULL, "Title", QStringLiteral("电话号码不足16位"));
    //        return;
    //    }
    emit emitUnAnswerPhoneToJiTong(unansweredPhoneNumber);
    //这个时候可能应该把那个框子关死
}

// 接到对方来电，显示一个接收或者不接收的框子，来选择 来电方式，这里是被叫方 ，然后发送指令
void ClientSocket::receiveSomeVideoCallOrNot(QByteArray msg)
{
    //关的时候要恢复 FREE STATE,  有两处需要设， 一个是我们自己挂的时候， 一个是人家挂的时候
    if(Conversation_MODE != FREE_STATE)
        return;




    //先设定15秒(需求是60秒) 没接电话的情况下， 激活信号，  使机通发送未接来电 假如是地面端就不用发送了
    if (Conversation_MODE == FREE_STATE ){   //未接来电时间， 框子消失时间  ，重播时间都设定一个值
        unAnsweredPhoneTimer->setInterval(PHONE_TIME);
        unAnsweredPhoneTimer->start();
        //把电话赋值
        unansweredPhoneNumber = QString(msg.mid(5,16));
        answeredPhoneNumber = QString(msg.mid(5,16));
    }

    Conversation_MODE = CALLING_STATE;


    reciveMessagebox = new QMessageBox(QMessageBox::NoIcon,QStringLiteral("提示"),QStringLiteral("是否接听电话"));

    //reciveMessagebox->setStandardButtons(QMessageBox::YesToAll|QMessageBox::NoToAll);
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

void ClientSocket::OkToGroundCallInstruct(){

    //Conversation_MODE = PHONING_STATE;
    //CIDC-04
    unAnsweredPhoneTimer->stop();
    //
    QByteArray ReceiveOrNot;
    ReceiveOrNot.append(QByteArray::fromHex(instructHead));
    //帧标识  //改为12了  既18
    ReceiveOrNot.append(QByteArray::fromHex("1a"));
    //帧长度
    short length = 11;
    length = BLEndianShort(length);
    QByteArray lengthByte = ShortToByte(length);
    ReceiveOrNot.append(lengthByte);
    //视频
    ReceiveOrNot.append(QByteArray::fromHex("01"));
    //
    ReceiveOrNot.append(IntToByte(BLEndianInt(statsWaterNumber)));
    //校验和
    char sum = 0;
    for(char i:ReceiveOrNot.mid(2,ReceiveOrNot.size() -2 )) {
        sum += i;
    }
    ReceiveOrNot.append(sum);
    instructSocket->writeDatagram(ReceiveOrNot,QHostAddress(SERVER_ADRESS),INSTRUCT_PORT);
    instructSocket->writeDatagram(ReceiveOrNot,QHostAddress(SERVER_ADRESS),INSTRUCT_PORT);
    instructSocket->writeDatagram(ReceiveOrNot,QHostAddress(SERVER_ADRESS),INSTRUCT_PORT);
    //接电话了调用机通发01H指令
    emit emitAnswerPhoneToJiTong(answeredPhoneNumber);
    //
    incrementWaterNumber(statsWaterNumber);
    openConversation(VIDEOCHAT);

}


void ClientSocket::NotOkToGroundCallInstruct()
{

    Conversation_MODE = FREE_STATE;
    //CIDC-04   改为12了
    unAnsweredPhoneTimer->stop();
    //
    QByteArray ReceiveOrNot;
    ReceiveOrNot.append(QByteArray::fromHex(instructHead));
    //帧标识
    ReceiveOrNot.append(QByteArray::fromHex("1a"));
    //帧长度
    short length = 11;
    length = BLEndianShort(length);
    QByteArray lengthByte = ShortToByte(length);
    ReceiveOrNot.append(lengthByte);
    //视频
    ReceiveOrNot.append(QByteArray::fromHex("02"));
    //流水号
    ReceiveOrNot.append(IntToByte(BLEndianInt(statsWaterNumber)));
    //校验和
    char sum = 0;
    for(char i:ReceiveOrNot.mid(2,ReceiveOrNot.size() -2)) {
        sum += i;
    }
    ReceiveOrNot.append(sum);
    instructSocket->writeDatagram(ReceiveOrNot,QHostAddress(SERVER_ADRESS),INSTRUCT_PORT);
    instructSocket->writeDatagram(ReceiveOrNot,QHostAddress(SERVER_ADRESS),INSTRUCT_PORT);
    instructSocket->writeDatagram(ReceiveOrNot,QHostAddress(SERVER_ADRESS),INSTRUCT_PORT);
    incrementWaterNumber(statsWaterNumber);
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

    sendCloseSystemInstruct();
    //closeSystemCount = 0;
    //closeInstructTimer->setInterval(1000);
    //closeInstructTimer->start();

}

//这个才是发送的关机指令
void ClientSocket::sendCloseSystemInstruct()
{
    //    closeSystemCount ++;
    //    if(closeSystemCount == 4) {
    //        closeInstructTimer->stop();
    //        closeSystemCount = 0 ;
    //        return;
    //        incrementWaterNumber(closeSystemWaterNumber);
    //    }
    //qDebug()<<"close system count ";
    //qDebug() <<closeSystemCount;
    QByteArray closeInstruct;
    closeInstruct.append(QByteArray::fromHex(instructHead));
    //
    closeInstruct.append(QByteArray::fromHex("04"));
    //长度
    short length = 11;
    length = BLEndianShort(length);
    QByteArray lengthByte =ShortToByte(length);
    closeInstruct.append(lengthByte);
    closeInstruct.append(QByteArray::fromHex("01"));
    //流水号
    closeInstruct.append(IntToByte(BLEndianInt(closeSystemWaterNumber)));
    //校验和
    char sum = 0;
    for(char i:closeInstruct.mid(2,closeInstruct.size() - 2)) {
        sum += i;
    }
    closeInstruct.append(sum);
    instructSocket->writeDatagram(closeInstruct,QHostAddress(SERVER_ADRESS),INSTRUCT_PORT);
    instructSocket->writeDatagram(closeInstruct,QHostAddress(SERVER_ADRESS),INSTRUCT_PORT);
    instructSocket->writeDatagram(closeInstruct,QHostAddress(SERVER_ADRESS),INSTRUCT_PORT);
    incrementWaterNumber(closeSystemWaterNumber);

    //下面是记次数的，发送三次，就不发送了，结束了
}

//管理上报自建重发的调控 EN-DCCI-27   //待删除
void ClientSocket::JTongSelfCheckMsgManager()
{
    JtongSelfCheckTimer->setInterval(circleTIME);
    //发送一条，100ms没收到回信就继续发送
    //stopWorkingInstruct();   //先设定一共就发三次
    JtongSelfCheckTimer->start();

}

//播撒中止
void ClientSocket::stopWorkingInstruct(QString cuihua_1,QString cuihua_2)
{


    QByteArray bosaInstruct;
    bosaInstruct.append(QByteArray::fromHex(instructHead));
    bosaInstruct.append(QByteArray::fromHex("1B"));
    short length = 13;
    length = BLEndianShort(length);
    QByteArray lengthByte =ShortToByte(length);
    bosaInstruct.append(lengthByte);

    bosaInstruct.append(QByteArray::fromHex(cuihua_1.toStdString().c_str()));
    bosaInstruct.append(QByteArray::fromHex(cuihua_2.toStdString().c_str()));
    //流水号
    bosaInstruct.append(IntToByte(BLEndianInt(bosaWaterNumber)));
    //校验和
    char sum = 0;
    for(char i:bosaInstruct.mid(2,bosaInstruct.size() - 2)) {
        sum += i;
    }
    bosaInstruct.append(sum);
    instructSocket->writeDatagram(bosaInstruct,QHostAddress(SERVER_ADRESS),INSTRUCT_PORT);
    instructSocket->writeDatagram(bosaInstruct,QHostAddress(SERVER_ADRESS),INSTRUCT_PORT);
    instructSocket->writeDatagram(bosaInstruct,QHostAddress(SERVER_ADRESS),INSTRUCT_PORT);
    incrementWaterNumber(bosaWaterNumber);
}




//电话号码暂时为自我填写的内容
void ClientSocket::hangUpthePhoneInstruct()
{
    Conversation_MODE = FREE_STATE;
    //EN-DCCI-03
    QByteArray tellCIclosed;   //启动视频通话，或者结束视频通话
    tellCIclosed.append(QByteArray::fromHex(instructHead));
    //帧标识
    tellCIclosed.append(QByteArray::fromHex("03"));
    // 帧长度 29  //要改变数字了
    short length = 34;
    length = BLEndianShort(length);
    QByteArray lengthByte = ShortToByte(length);
    tellCIclosed.append(lengthByte);
    //飞机编号，先默认取一个值
    tellCIclosed.append(planeNumber);

    if(dc_side == Which_Client::LEFT) {
        tellCIclosed.append(QByteArray::fromHex("01"));
    } else if(dc_side == Which_Client::RIGHT) {
        tellCIclosed.append(QByteArray::fromHex("02"));
    }
    //0x02为挂断
    tellCIclosed.append(QByteArray::fromHex("02"));
    //前后仓
    if(backOrFrontChamber == FRONT_CHAMBER){
        tellCIclosed.append(QByteArray::fromHex("01"));
    } else if(backOrFrontChamber == BACK_CHAMBER) {
        tellCIclosed.append(QByteArray::fromHex("02"));
    }
    //申请人电话16 //
    if(this->phoneNumber.size() >= 16) {
        //videoRequest.append(this->phoneNumber.mid(0,16));
        QString num = this->phoneNumber.mid(0,16);
        for(int i = 0;i<16;i++) {
            tellCIclosed.append(num.at(i).toLatin1());
        }
    } else {
        while(this->phoneNumber.size() != 16){
            this->phoneNumber = this->phoneNumber + "0";
        }
        //videoRequest.append(this->phoneNumber);
        QString num = this->phoneNumber;
        for(int i = 0;i<16;i++) {
            tellCIclosed.append(num.at(i).toLatin1());
        }
    }
    //流水号
    tellCIclosed.append(IntToByte(BLEndianInt(phoneWaterNumber)));
    //校验和
    char sum= 0;
    for(char i : tellCIclosed.mid(2,tellCIclosed.size() - 2)) {
        sum += i;
    }
    tellCIclosed.append(sum);

    instructSocket->writeDatagram(tellCIclosed,QHostAddress(SERVER_ADRESS),INSTRUCT_PORT);
    instructSocket->writeDatagram(tellCIclosed,QHostAddress(SERVER_ADRESS),INSTRUCT_PORT);
    instructSocket->writeDatagram(tellCIclosed,QHostAddress(SERVER_ADRESS),INSTRUCT_PORT);

    incrementWaterNumber(phoneWaterNumber);
}




//之后再考虑是否  循环的指令 和 单独发送的指令冲突不冲突
void ClientSocket::SendCircleManager()
{
    circle_workSendTimer->setInterval(circleTIME);

    everyCircleWork();

    circle_workSendTimer->start(); //

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

//给出短信那里的时间， 2020-06-12,12:23:03     19个长度的  给出当前的日期和时间
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

    QString entireDateAndTime = entireDate + "," + entireTime;

    return entireDateAndTime;
}



//发送短信息，海事短信
void ClientSocket::sentShortMsgInstruct(QString msg,int beidouOrHaishi)
{
    if(beidouOrHaishi == BEIDOU_MSG){
        sentShortMsgInstructBeiDou(msg);
    } else if(beidouOrHaishi == HAISHI_MSG) {
        sentShortMsgInstructHaiShi(msg);
    }

}




// 海事短信
void ClientSocket::sentShortMsgInstructHaiShi(QString msg) {
    QByteArray shortMsgInstruct;
    shortMsgInstruct.append(QByteArray::fromHex(instructHead));

    shortMsgInstruct.append(QByteArray::fromHex("10"));  //帧标识
    short length = 322;
    length = BLEndianShort(length);
    QByteArray lengthByte = ShortToByte(length);
    shortMsgInstruct.append(lengthByte);  //帧长度

    if(MsgNumber.size() >=20){
        shortMsgInstruct.append(MsgNumber.mid(0,20));
    } else {
        while(MsgNumber.size() != 20){
            MsgNumber.append("0");
        }
        shortMsgInstruct.append(MsgNumber);
    }
    //shortMsgInstruct.append("0001");      //暂时这样保存
    //shortMsgInstruct.append("B-XXXXXX");    //发报地址
    //shortMsgInstruct.append("RYXXXXXX");     //收报地址
    shortMsgInstruct.append(giveCurrentDateAndTime());   //发报时戳
    shortMsgInstruct.append(giveCurrentDateAndTime());   //编制时间？？

    shortMsgInstruct.append(IntToByte(BLEndianInt(msgWaterNumber)));   //流水号，位数向前了一个
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


//发送短信息，北斗短信
void ClientSocket::sentShortMsgInstructBeiDou(QString msg)
{
    QByteArray shortMsgInstruct;
    shortMsgInstruct.append(QByteArray::fromHex(instructHead));

    shortMsgInstruct.append(QByteArray::fromHex("17"));  //帧标识
    short length = 81; //需修改
    length = BLEndianShort(length);
    QByteArray lengthByte = ShortToByte(length);
    shortMsgInstruct.append(lengthByte);  //帧长度

    if(MsgNumber.size() >=3){
        shortMsgInstruct.append(MsgNumber.mid(0,3));
    } else {
        while(MsgNumber.size() != 3){
            MsgNumber.append("0");
        }
        shortMsgInstruct.append(MsgNumber);
    }

    //shortMsgInstruct.append(giveCurrentDateAndTime());   //发报时戳
    //shortMsgInstruct.append(giveCurrentDateAndTime());   //编制时间？？
    //发送信息

    shortMsgInstruct.append(msg.mid(0,68).toUtf8());

    for(int i = msg.toUtf8().size();i<68;i++) {
        shortMsgInstruct.append('\0');
    }

    shortMsgInstruct.append(IntToByte(BLEndianInt(beidouMsgWaterNumber)));   //流水号，位数向前了一个
    //校验和
    char sum= 0;
    for(char i : shortMsgInstruct.mid(2,shortMsgInstruct.size() - 2)) {
        sum += i;
    }

    shortMsgInstruct.append(sum);
    instructSocket->writeDatagram(shortMsgInstruct,QHostAddress(SERVER_ADRESS),INSTRUCT_PORT);
    instructSocket->writeDatagram(shortMsgInstruct,QHostAddress(SERVER_ADRESS),INSTRUCT_PORT);
    instructSocket->writeDatagram(shortMsgInstruct,QHostAddress(SERVER_ADRESS),INSTRUCT_PORT);
    incrementWaterNumber(beidouMsgWaterNumber);
}


//授时帧  DCCI-02   //读到心跳发授时，  发一次就不发了
void ClientSocket::sendSystemTime()
{
    //qDebug()<<QDateTime::currentDateTime().date().year();
    qDebug()<< QStringLiteral("发送授时");
    QByteArray sendTime;
    sendTime.append(QByteArray::fromHex(instructHead));
    //帧表示
    sendTime.append(QByteArray::fromHex("02"));
    //13
    short length = 13;
    length = BLEndianShort(length);
    QByteArray lengthByte = ShortToByte(length);
    sendTime.append(lengthByte);
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


void ClientSocket::receiveShortMsgAndShow(QByteArray replyMsg)
{
    QByteArray msg = replyMsg.mid(105,100);
    QTextCodec *tc = QTextCodec::codecForName("utf-8");
    QString ok  = tc->toUnicode(msg.data());   //转化为能显示的中文信息
    sendRepliedMsgtoMainWindow(ok,HAISHI_MSG);
}

void ClientSocket::receiveShortMsgAndShow_beidou(QByteArray replyMsg){
    QByteArray msg = replyMsg.mid(8,68);//应该是68而不是文档上的一百
    QTextCodec *tc = QTextCodec::codecForName("utf-8");
    QString ok  = tc->toUnicode(msg.data());   //转化为能显示的中文信息
    sendRepliedMsgtoMainWindow(ok,BEIDOU_MSG);
}



//空地状态包 数据帧
void ClientSocket::skyGroundStatusInstruct(int i)
{

    QByteArray statusInstruct;   //EN-DCCI-24
    statusInstruct.append(QByteArray::fromHex(instructHead));
    //帧标识
    statusInstruct.append(QByteArray::fromHex("18"));
    // 帧长度
    short length = 11;
    length = BLEndianShort(length);
    statusInstruct.append(ShortToByte(length));
    statusInstruct.append(IntToByte(BLEndianInt(skyGroundWaterNumber)));
    //起飞
    if(i == 0) {
        statusInstruct.append(QByteArray::fromHex("00"));
    }
    if(i == 1) {
        statusInstruct.append(QByteArray::fromHex("01"));
    }
    if(i == 2) {
        statusInstruct.append(QByteArray::fromHex("02"));
    }
    if(i == 3) {
        statusInstruct.append(QByteArray::fromHex("03"));
    }

    //校验和
    char sum= 0;
    for(char i: statusInstruct.mid(2,statusInstruct.size() -2)) {
        sum += i;
    }
    statusInstruct.append(sum);
    instructSocket->writeDatagram(statusInstruct,QHostAddress(SERVER_ADRESS),INSTRUCT_PORT);
    instructSocket->writeDatagram(statusInstruct,QHostAddress(SERVER_ADRESS),INSTRUCT_PORT);
    instructSocket->writeDatagram(statusInstruct,QHostAddress(SERVER_ADRESS),INSTRUCT_PORT);
    incrementWaterNumber(skyGroundWaterNumber);
}




//EN-DCCI-25
void ClientSocket::mSatelliteControlManager(bool Islinked,char Netstats) {
    this->islink = Islinked;
    this->netstats = Netstats;
    msControlTimer->setInterval(circleTIME);  //假如没收到回复的信息就重新发送，收到了停止发送，显示信息
    skyGroundStatusInstruct(0);
    msControlTimer->start();  //收到回复或者发三次无回应则停止
}






//EN-DCCI-25 飞机编码和弹厂商
void ClientSocket::gunAndFactoryInstrcut(QString plane,QString gun)
{

    QByteArray controlMsg;
    controlMsg.append(QByteArray::fromHex(instructHead));
    //帧标识
    controlMsg.append(QByteArray::fromHex("19"));
    // 帧长度   //需修改
    short length = 17;
    length = BLEndianShort(length);
    controlMsg.append(ShortToByte(length));
    //飞机编号，先默认取一个值
    if(plane.size() ==5)
        controlMsg.append(plane);
    else
        return;
    if(gun.size() == 2)
        controlMsg.append(gun);
    else
        return;
    controlMsg.append(IntToByte(BLEndianInt(gunAndPlaneWaterNumber)));

    //校验和
    char sum= 0;
    for(char i: controlMsg.mid(2,controlMsg.size() - 2)) {
        sum += i;
    }
    controlMsg.append(sum);
    instructSocket->writeDatagram(controlMsg,QHostAddress(SERVER_ADRESS),INSTRUCT_PORT);
    instructSocket->writeDatagram(controlMsg,QHostAddress(SERVER_ADRESS),INSTRUCT_PORT);
    instructSocket->writeDatagram(controlMsg,QHostAddress(SERVER_ADRESS),INSTRUCT_PORT);
    incrementWaterNumber(gunAndPlaneWaterNumber);
}


void ClientSocket::showMsControlReply(QByteArray msg)
{
    QString showText;

    showText.append(QStringLiteral("海事卫星链路连接状态: "));
    if(msg.at(5) == 0x01)
        showText.append(QStringLiteral("建链\n"));
    if(msg.at(5) == 0x02)
        showText.append(QStringLiteral("断开链接\n"));

    QMessageBox* info = new   QMessageBox(QMessageBox::NoIcon,QStringLiteral("提示"),showText);
    info->show();
    //showText.append("带宽状态: ");
    //if(msg.at(6) == 0x00)
    // showText.append("共享宽带\n");
    //if(msg.at(6) == 0x01)
    // showText.append("64kbps独享宽带\n");
    //if(msg.at(6) == 0x02)
    //  showText.append("128kbps独享宽带\n");
    //if(msg.at(6) == 0x03)
    // showText.append("384kbps独享宽带\n");
    //sendMsControlMsgtoMwToShow(showText);
}



short ClientSocket::ByteArrayToShort(QByteArray _2)
{
    short num;
    memcpy(&num,_2.data(),2);
    return num;
}

int ClientSocket::ByteArrayToInt(QByteArray _4)
{
    int num;
    memcpy(&num,_4.data(),sizeof(num));
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
    //homeWorkMsgInstruct();  //此指令废弃，统一用senddataWidget里面的数据，来进行发送
}

//DCCI-14 回音
void ClientSocket::sendAudioReplayInstructToCI(QString fileName)
{
    QByteArray instruct;
    instruct.append(QByteArray::fromHex(instructHead));
    instruct.append(QByteArray::fromHex("14"));
    //帧长度  //一会要改了
    short length = 34;
    length = BLEndianShort(length);
    QByteArray lengthByte = ShortToByte(length);
    instruct.append(lengthByte);
    if(dc_side == Which_Client::LEFT) {
        instruct.append(QByteArray::fromHex("01"));
    } else {
        instruct.append(QByteArray::fromHex("02"));
    }
    instruct.append(fileName.split(' ')[0]);
    QString time = fileName.split(' ')[1];
    QString date = time.split(",")[0];

    QString year = date.split('-')[0];
    QString month = date.split('-')[1];
    QString day = date.split('-')[2];

    time = time.split(",")[1];

    QString hour = time.split(':')[0];
    QString min = time.split(':')[1];
    QString second = time.split(':')[2];

    short y = BLEndianShort(year.toShort());
    char m_onth = month.toInt();
    char d = day.toInt();

    char h = hour.toInt();
    char m = min.toInt();
    char s = second.toInt();
    instruct.append(ShortToByte(y));
    instruct.append(m_onth);
    instruct.append(d);
    instruct.append(h);
    instruct.append(m);
    instruct.append(s);
    //流水号
    instruct.append(IntToByte(BLEndianInt(audioReplyWaterNumber)));
    //校验和
    char sum= 0;
    for(char i: instruct.mid(2,instruct.size() - 2)) {
        sum += i;
    }
    instruct.append(sum);
    instructSocket->writeDatagram(instruct,QHostAddress(SERVER_ADRESS),INSTRUCT_PORT);
    instructSocket->writeDatagram(instruct,QHostAddress(SERVER_ADRESS),INSTRUCT_PORT);
    instructSocket->writeDatagram(instruct,QHostAddress(SERVER_ADRESS),INSTRUCT_PORT);
    incrementWaterNumber(audioReplyWaterNumber);
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

//北斗相关指令
void ClientSocket::BeiDouSatelliteInstruct(bool p1,bool p2)
{
    qDebug() <<"OPEN_BEIDOU  MSG";
    QByteArray instruct;
    instruct.append(QByteArray::fromHex(instructHead));
    //帧标识
    instruct.append(QByteArray::fromHex("15"));
    //帧长度
    short length = 12;   //需修改
    length = BLEndianShort(length);
    instruct.append(ShortToByte(length));

    //北斗控件数据
    if(p1){
        instruct.append(QByteArray::fromHex("01"));
        //在这里开启周期作业信息或者下面关闭信息
        emit openBeidouWorkInfo(true);
    } else {
        instruct.append(QByteArray::fromHex("02"));
        emit openBeidouWorkInfo(false);
    }
    if(p2){
        instruct.append(QByteArray::fromHex("01"));
    } else {
        instruct.append(QByteArray::fromHex("02"));
    }
    //流水号
    instruct.append(IntToByte(BLEndianInt(beidouWaterNumber)));
    //校验和
    char sum= 0;
    for(char i : instruct.mid(2,instruct.size() - 2))
    {
        sum += i;
    }
    instruct.append(sum);

    instructSocket->writeDatagram(instruct,QHostAddress(SERVER_ADRESS),INSTRUCT_PORT);
    instructSocket->writeDatagram(instruct,QHostAddress(SERVER_ADRESS),INSTRUCT_PORT);
    instructSocket->writeDatagram(instruct,QHostAddress(SERVER_ADRESS),INSTRUCT_PORT);

    incrementWaterNumber(beidouWaterNumber);
}


void ClientSocket::receiveMsgFromSendDataClassToGo(QByteArray Instruct)
{
    instructSocket->writeDatagram(Instruct,QHostAddress(SERVER_ADRESS),INSTRUCT_PORT);
}



//为了显示在任务栏那里，所以才改了
QString ClientSocket::giveCurrentTimeToShow()
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
    QString entireTime = hour + QStringLiteral(":") + min  + QStringLiteral(":")  + second;
    return entireTime;
}

QString ClientSocket::changeByteToString(QByteArray bytearray)
{
    QString str;
    for(int i =0 ;i< bytearray.size();i++ ) {
        int  num = bytearray.toInt();
        str.append(QString::number(num));
    }
    return str;
}





//short 转换大小端
short ClientSocket::BLEndianShort(short value)
{
    return ((value & 0x00FF) << 8 ) | ((value & 0xFF00) >> 8);
}



//转换大小端
uint ClientSocket::BLEndianInt(uint x)
{
    return (((uint)(x) & 0xff000000) >> 24) |  (((uint)(x) & 0x00ff0000) >> 8) | \
            (((uint)(x) & 0x0000ff00) << 8) | (((uint)(x) & 0x000000ff) << 24);
}


//DCCI-22  OX16
void ClientSocket::haishiControlInstruct(bool link ,int net){
    qDebug() <<"HAISHI_CONTROL MSG";
    QByteArray instruct;
    instruct.append(QByteArray::fromHex(instructHead));
    //帧标识
    instruct.append(QByteArray::fromHex("16"));
    //帧长度
    short length = 12;   //需修改
    length = BLEndianShort(length);
    instruct.append(ShortToByte(length));
    length = BLEndianShort(length);
    //北斗控件数据
    if(link){
        instruct.append(QByteArray::fromHex("01"));
    } else {
        instruct.append(QByteArray::fromHex("02"));
    }
    if(net == 0){
        instruct.append(QByteArray::fromHex("00"));
    }
    if(net == 1){
        instruct.append(QByteArray::fromHex("01"));
    }
    if(net == 2){
        instruct.append(QByteArray::fromHex("02"));
    }
    if(net == 3){
        instruct.append(QByteArray::fromHex("03"));
    }
    //流水号
    instruct.append(IntToByte(BLEndianInt(haishiControlWaterNumber)));
    //校验和
    char sum= 0;
    for(char i : instruct.mid(2,instruct.size() - 2))
    {
        sum += i;
    }
    instruct.append(sum);

    instructSocket->writeDatagram(instruct,QHostAddress(SERVER_ADRESS),INSTRUCT_PORT);
    instructSocket->writeDatagram(instruct,QHostAddress(SERVER_ADRESS),INSTRUCT_PORT);
    instructSocket->writeDatagram(instruct,QHostAddress(SERVER_ADRESS),INSTRUCT_PORT);
    incrementWaterNumber(haishiControlWaterNumber);
}





























