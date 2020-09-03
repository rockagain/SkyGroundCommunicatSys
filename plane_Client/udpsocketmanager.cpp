#include "udpsocketmanager.h"

UdpSocketManager::UdpSocketManager(QObject *parent) : QObject(parent)
{

    int dataport;
    int controlport;
    QSettings* settings = new QSettings(QCoreApplication::applicationDirPath()+"/config.ini", QSettings::IniFormat);
    //qDebug()<<QCoreApplication::applicationDirPath();
    //settings->setValue("SIDE/CLIENTSIDE","LEFT");
    QVariant var = settings->value("SIDE/CLIENTSIDE");
    QString side = var.toString();
    //1.之后把所有 调节时间的都当成 配置都进   2. 在 接电话和 打电话的时候，  用信号和槽把  timer里面的内容改过来
    //封装好了函数，把所有的值都设定好，关于时间的

    if(side != "GROUND") {   //飞机端的
        dataport = 50201;
        controlport = 50202;
    } else  {
        dataport = 50140;
        controlport = 50141;
    }

    //记录重新发送的次数，当次数为第二次时候，认为文件传输失败，上报文件传输失败
    reSendTime = 0;
    //文件发送方在发送完“文件描述”数据包后，开始计时，如果4秒内没有接收到文件接收方回复的“文件接收状态”帧，则重新发送“文件描述”数据包，从第1帧开始重新发送文件数据。
    int reSendFileDescript = 4000;
    //设置正常状态下 40秒发送一次 文件数据 //这个地方可以配置，之后通过QSettings里设定
    int nomalTime = 40;
    //按照文档3秒收不到 回复状态  或者  发送数据   则按照 500ms 继续发送状态 //
    int notReceiveInstructTime = 3;

    //这些都是调节发送数据频率的
    dataSenderTimer = new QTimer(this);
    controlSenderTimer = new QTimer(this);
    timeCounterTimer = new QTimer(this);


    // 把udpsocket建立起来
    dataPortSocket = new QUdpSocket(this);
    bool isBinding =   dataPortSocket->bind(QHostAddress::LocalHost,dataport);
    controlPortSocket = new QUdpSocket(this);
    bool isbinging = controlPortSocket->bind(QHostAddress::LocalHost,controlport);

    connect(dataPortSocket, &QUdpSocket::readyRead, this, &UdpSocketManager::dataInstructionReceive);
    //控制端接收信息的
    connect(controlPortSocket, &QUdpSocket::readyRead, this, &UdpSocketManager::controlInstructReceive);
}


void UdpSocketManager::selectFileAndSendFileInfo()
{
    qDebug()<<"uploadFile";
    QString fileName =
            QFileDialog::getOpenFileName(NULL, "选择文件并上传",
                                         QDir::homePath(),
                                         "选择任何文件(*)");
    qDebug()<<fileName;
    if (fileName.isEmpty())
        return;
    //应该是用这个!!!!!QDataStream
    QFile TheFile(fileName);
    TheFile.open(QIODevice::ReadOnly);
    //读进来的东西把它记录下来
    QByteArray wholeFileBar= TheFile.readAll();

    currentUploadFile = new CurrentExchangedFile;

    currentUploadFile->theWholeFile = wholeFileBar;
    //算总帧
    currentUploadFile->wholeFrame = wholeFileBar.size()/1000;

    if(currentUploadFile->wholeFrame<1000)
        currentUploadFile->wholeFrame = 1;

    if((wholeFileBar.size() % 1000) != 0)
        currentUploadFile->wholeFrame ++;

    //算md5
    QByteArray md5ba = QCryptographicHash::hash(wholeFileBar,QCryptographicHash::Md5).toHex();
    qDebug()<<md5ba.length();
    currentUploadFile->md5 = md5ba;
    //读取文件的时候设置currentFrame为0
    currentSendFrame = 0;

    //直接在  intToByte 里面调节大小端
    currentUploadFile->fileSize = intToByte(wholeFileBar.size());
    // currentUploadFile->filename = TheFile.fileName();
    QFileInfo filefullname(TheFile.fileName());

    //currentUploadFile->filename = filefullname.fileName();
    qDebug()<<filefullname.fileName();
    currentUploadFile->filename = filefullname.fileName().toUtf8();
    int nameSize = currentUploadFile->filename.size();
    for(int i = nameSize; i <= 31; i++){
        currentUploadFile->filename += '\0';
    }

    //发送
    sendFileDescriptionInstruct();

    //设置信号和槽，  把电话接通 视频接通  挂断，发送信号
    if(phoneCallMode == FREE_STATE)
        dataSenderTimer->setInterval(40);
    if(phoneCallMode == ANSWERING_STATE)
        dataSenderTimer->stop();
    if(phoneCallMode == PHONING_STATE)
        dataSenderTimer->setInterval(80);

    //
    connect(dataSenderTimer,&QTimer::timeout,this,&UdpSocketManager::sendDataInstruct);
    dataSenderTimer->start();
}


QByteArray  UdpSocketManager::intToByte(int i)
{
    //可以在这里调节大小端
    QByteArray abyte0;
    abyte0.resize(4);
    abyte0[0] = (uchar)  (0x000000ff & i);
    abyte0[1] = (uchar) ((0x0000ff00 & i) >> 8);
    abyte0[2] = (uchar) ((0x00ff0000 & i) >> 16);
    abyte0[3] = (uchar) ((0xff000000 & i) >> 24);
    return abyte0;
}


void  UdpSocketManager::sendFileDescriptionInstruct()
{

    QByteArray instructC;
    //instructBar.resize(78);
    instructC.append(QByteArray::fromHex("1001"));

    QByteArray numBar;
    ushort num =78;
    instructC.append(numBar.setNum(num,16));

    instructC.append(currentUploadFile->fileSize);

    //飞机编号
    instructC.append(QByteArray::fromHex("0000000000"));

    instructC.append(currentUploadFile->md5);
    instructC.append(currentUploadFile->filename);
    char sum = 0;
    for(char oneByte:instructC) {
        sum += oneByte;
        qDebug()<< sum;
    }
    qDebug()<<instructC.size();
    //之后端口在进行更改
    dataPortSocket->writeDatagram(instructC,QHostAddress(Config_L::MANAGER_SERVER_ADDRESS),Config_L::SERVER_PORT);
}


//DATA指令， 这条指令有可能不定长， 在他是最后一条的时候
void UdpSocketManager::sendDataInstruct()
{
    QByteArray instructC;
    //instructBar.resize(78);
    instructC.append(QByteArray::fromHex("1002"));

    QByteArray numBar;
    ushort num =1008;
    instructC.append(numBar.setNum(num,16));
    //发 currentFrame + 1
    instructC.append(numBar.setNum(currentSendFrame + 1,16));

    char transferStats;
    if(currentUploadFile->wholeFrame != currentSendFrame + 1)
        transferStats = 1;
    if(currentUploadFile->wholeFrame == currentSendFrame + 1) //为最后一帧
        transferStats = 2;

    instructC.append(transferStats);

    QByteArray fileBar;
    if(currentSendFrame == 0 )
    {
        fileBar = currentUploadFile->theWholeFile.mid(0,1000);
    } else
    {
        fileBar = currentUploadFile->theWholeFile.mid(((currentSendFrame - 1)*1000 + 999),1000);
    }
    instructC.append(fileBar);

    char sum = 0;
    for(char oneByte:instructC) {
        sum += oneByte;
        qDebug()<< sum;
    }
    sum = sum & 0xFF;
    instructC.append(sum);
    qDebug()<<instructC.size();
    //之后端口在进行更改
    dataPortSocket->writeDatagram(instructC,QHostAddress(Config_L::MANAGER_SERVER_ADDRESS),Config_L::SERVER_PORT);

}


//接收control 指令
void UdpSocketManager::controlInstructReceive()  //接收反馈处理信息
{

    //反馈回来的 指令
    QByteArray ba;
    while(controlPortSocket->hasPendingDatagrams())
    {
        ba.resize(controlPortSocket->pendingDatagramSize());
        controlPortSocket->readDatagram(ba.data(), ba.size());
        checkMsg(ba);   //  分析来到的信息
    }

}



//分析 control指令
void UdpSocketManager::checkMsg(QByteArray byteArray)
{
    //默认得到信息为15个byte
    if(byteArray.mid(0,2).toUShort() != 0x1003) {
        qDebug()<< "指令有问题";  //之后这里改为弹个窗之类的行为
    }
    if(byteArray.mid(2,2).toUShort() != 11) {
        qDebug()<< "指令对不上";  //之后看看是不是其余的都要检查。没协商好怎么处理的先不写
        //比方说文件状态 和  失败原因等  飞机端号啥的 和在哪个端 也有关系， 和校验和啥的
    }
    currentSendFrame = byteArray.mid(10,2).toUShort();

}


//接收data port 指令
void UdpSocketManager::dataInstructionReceive()
{
    //反馈回来的 指令
    QByteArray ba;
    while(dataPortSocket->hasPendingDatagrams())
    {
        ba.resize(dataPortSocket->pendingDatagramSize());
        dataPortSocket->readDatagram(ba.data(), ba.size());
        checkDataMsg(ba);   //  分析来到的信息
    }
}


void UdpSocketManager::checkDataMsg(QByteArray byteArray)
{

    if(byteArray.mid(0,2).toUShort() == 0x1001){   //其实在这里就建立文件也可以，在下面建立文件也可以，因为在下面建立，是传好了再建
        currentRecivedFile = new CurrentExchangedFile;
        currentRecivedFile->fileSize = byteArray.mid(9,4);
        currentRecivedFile->md5 = byteArray.mid(13,32);
        currentRecivedFile->filename = byteArray.mid(45,32);
        currentRecivedFile->wholeFrame = byteArray.size()/1000;

        currentRecivedFrame = 0;  //第一个文件包就是第一帧  //这里之后还要做一个判断，选判断所选的内容是否已经有内容，在根据内容多少设定帧
        //检测完了是否为第一帧之后还要回复一个包 就是回复包！
        //
        if(byteArray.size()%1000 != 0)
            currentRecivedFile->wholeFrame += 1;

        receivedFileName = QDir::currentPath() + "/" + currentRecivedFile->filename;    //貌似自己把\0去除了

        //建立文件
        savedfile= new QFile(receivedFileName);
        savedfile->open(QFile::WriteOnly|QFile::Append);  //结束的时候close

        controlSenderTimer->setInterval(40);
        connect(controlSenderTimer,&QTimer::timeout,this,&UdpSocketManager::controlInstructSender);
        controlSenderTimer->start();

    } else if(byteArray.mid(0,2).toUShort() == 0x1002) {
        //保存文件   currentRecivedFile->receivedFile 这个在这里保存
        if(currentRecivedFrame == (byteArray.mid(4,2).toUShort() - 1)) {
            //如果相等
            QByteArray dataBar = byteArray.mid(7,1000);
            savedfile->write(dataBar);  //传完了之后closed
            //设定在这里，假如上面的 帧 对不上，  就不增加继续发
            currentRecivedFrame ++ ;
        }

    }
}

//发送 control 指令
void UdpSocketManager::controlInstructSender()
{
    QByteArray instructC;
    //instructBar.resize(78);
    instructC.append(QByteArray::fromHex("1003"));

    QByteArray numBar;
    ushort num =15;
    instructC.append(numBar.setNum(num,16));
    //飞机编号
    instructC.append(QByteArray::fromHex("0000000000"));
    num = 1;
    instructC.append(numBar.setNum(num,16));
    num = currentRecivedFrame;
    instructC.append(numBar.setNum(num,16));
    num = 1;  //最后做一个判断，当为最后一帧的时候，检查md5值，发送是否正确
    instructC.append(numBar.setNum(num,16));
    uchar sum = 0;
    for(char oneByte:instructC) {
        sum += oneByte;
        qDebug()<< sum;
    }
    qDebug()<<instructC.size();
    //之后端口在进行更改
    dataPortSocket->writeDatagram(instructC,QHostAddress(Config_L::MANAGER_SERVER_ADDRESS),Config_L::SERVER_PORT);
}







