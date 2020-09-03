#include "senddatawidget.h"
#include "ui_senddatawidget.h"



sendDataWidget::sendDataWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::sendDataWidget)
{
    ui->setupUi(this);
    init();

}

sendDataWidget::~sendDataWidget()
{
    delete ui;
}


void sendDataWidget::init(){

    beidouInstructNumber = 0;
    QSettings* settings = new QSettings(QCoreApplication::applicationDirPath()+"/sky_config.ini", QSettings::IniFormat);
    QVariant var = settings->value("SIDE/CLIENTSIDE");
    QString side = var.toString ();
    var = settings->value("INSTRUCT_FREQUENCY/GAP_TIME");
    circleTIME = var.toInt()* 1000;  //设置间隔时间

    //飞机编号
    var = settings->value("PLANE_NUMBER/NUM","00001");
    planeNumber = var.toString();

    QHBoxLayout *Layout = new QHBoxLayout;
    Layout->setContentsMargins(0,0,0,0);

    this->setLayout(Layout);
    Layout->addWidget(ui->tabWidget);

    if(side == "LEFT") {
        dc_side = Which_Client::LEFT;
        //开关
        ui->openOrClose->setChecked(true);
    } else if(side == "RIGHT"){
        dc_side = Which_Client::RIGHT;
    }

    if(dc_side == Which_Client::LEFT) {
        instructHead = "AA87";


    }else if(dc_side == Which_Client::RIGHT) {
        instructHead = "AA88";
    }
    //初始化timer
    circle_timer = new QTimer;
    //初始化QUdpsocket

    //init 参数
    setInitialParameter();
    //放开周期指令
    circleWorkerGo();
    //北斗作业信息周期
    beidouWorkInfo_timer = new QTimer;
    beidouWorkInfo_timer->setInterval(60000);

    //
    connect(ui->beidoupushButton,&QPushButton::clicked,this,&sendDataWidget::EmitBeidouInstruct);
    connect(circle_timer,&QTimer::timeout,this,&sendDataWidget::everyCircleWork);
    connect(ui->haishi_Button,&QPushButton::clicked,this,&sendDataWidget::sendHaishiMsg);
    //空地状态指令
    connect(ui->skyGround_Button,&QPushButton::clicked,this,[=](){
        int i;
        if(ui->skyGround_0->isChecked()) {
            i = 0;
        } else  if (ui->skyGround_1->isChecked()) {
            i = 1;
        } else if (ui->skyGround_2->isChecked()) {
            i = 2;
        } else if (ui->skyGround_3->isChecked()) {
            i = 3;
        }
        emit emitSkyGroundMsg(i);
    });
    //飞机编码和弹厂商
    connect(ui->gunAndPlane_button,&QPushButton::clicked,this,[=](){
        QString planeNum = ui->gunAndPlane_0->text();
        QString gunNum = ui->gunAndPlane_1->text();

        emit emitGunAndPlane(planeNum,gunNum);
    });
    // 播撒中止
    connect(ui->cuihua_button,&QPushButton::clicked,this,[=](){
        QString cuihua_1 = ui->cuihua_01->text();
        QString cuihua_2 = ui->cuihua_02->text();
        emit emitBoSaMsg(cuihua_1,cuihua_2);
    });
}



void sendDataWidget::sendHaishiMsg(){
    bool link;
    int net;
    if(ui->haishi_checkBox->isChecked()) {
        link = true;
    } else {
        link = false;
    }
    if(ui->haishi_0->isChecked()){
        net = 0;
    }
    if(ui->haishi_1->isChecked()){
        net = 1;
    }
    if(ui->haishi_2->isChecked()){
        net = 2;
    }
    if(ui->haishi_3->isChecked()){
        net = 3;
    }
    emit haishiControlMsg(link,net);
}


void sendDataWidget::circleWorkerGo()
{
    circle_timer->setInterval(circleTIME);

    everyCircleWork();

    circle_timer->start(); //
}


void sendDataWidget::everyCircleWork()
{
    //假如没被选就不发送
    if(!ui->openOrClose->isChecked())
        return;
    //周期函数
    aimms_30_weather_Instruct();
    aimms_30_plane_Instruct();
    fcdp_Instruct();
    dxas_Instruct();
    lwc_twc_Instruct();
    pcasp_Instruct();
    lwc_100_Instruct();
    cip_Instruct();
    cdp_Instruct();
    cpi_Instruct();
    twoD_S_Instruct();
    planeStatusInstruct();
    haishiWorkInfoMsgInstruct();
    beidouWorkInfoMsgInstruct();





    //被废除的指令
    //planeLocationInstruct();   //去除该指令了
    //BeiDouSatelliteInstruct();

}




//循环传播 一秒一个 aimms_30_Instruct
void sendDataWidget::aimms_30_weather_Instruct()
{
    qDebug() <<"AIMMS-30 WEATHER MSG";
    QByteArray instruct;
    instruct.append(QByteArray::fromHex(instructHead));
    //帧标识
    instruct.append(QByteArray::fromHex("05"));
    //帧长度 //需修改 24
    short length = 24;
    length = BLEndianShort(length);
    QByteArray lengthByte = ShortToByte(length);
    instruct.append(lengthByte);
    //飞机编号
    instruct.append(planeNumber);
    //时间
    QTime time = QDateTime::currentDateTime().time();
    char hour =  time.hour();
    instruct.append(hour);
    char min = time.minute();
    instruct.append(min);
    char second = time.second();
    instruct.append(second);
    //温度
    QString temper = ui->aimms_temper->text();
    if(temper.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("AIMMS-30天气温度数据不能为空"));
        return;
    }
    if(!isDigitStr(temper)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("AIMMS-30天气温度数据不能为非数字"));
        return;
    }
    short temper_Short = temper.toShort();
    temper_Short = BLEndianShort(temper_Short);
    instruct.append(ShortToByte(temper_Short));
    //相对湿度
    QString humidity = ui->aimms_humidity->text();
    if(humidity.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("AIMMS-30天气湿度数据不能为空"));
        return;
    }
    if(!isDigitStr(humidity)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("AIMMS-30天气湿度数据不能为非数字"));
        return;
    }
    short humidity_Short = humidity.toShort();
    humidity_Short = BLEndianShort(humidity_Short);
    instruct.append(ShortToByte(humidity_Short));
    //大气压
    QString atmos = ui->aimms_atmos->text();
    if(atmos.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("AIMMS-30天气大气压数据不能为空"));
        return;
    }
    if(!isDigitStr(atmos)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("AIMMS-30天气大气压数据不能为非数字"));
        return;
    }
    short atmos_Short = atmos.toShort();
    atmos_Short = BLEndianShort(atmos_Short);
    instruct.append(ShortToByte(atmos_Short));
    //风速
    QString speed = ui->aimms_speed->text();
    if(speed.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("AIMMS-30天气风速不能为空"));
        return;
    }
    if(!isDigitStr(speed)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("AIMMS-30天气风速数据不能为非数字"));
        return;
    }
    short speed_Short = speed.toShort();
    speed_Short = BLEndianShort(speed_Short);
    instruct.append(ShortToByte(speed_Short));
    //风向
    QString direction = ui->aimms_direction->text();
    if(direction.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("AIMMS-30天气风向数据不能为空"));
        return;
    }
    if(!isDigitStr(direction)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("AIMMS-30天气风向数据不能为非数字"));
        return;
    }
    short direction_Short = direction.toShort();
    direction_Short = BLEndianShort(direction_Short);
    instruct.append(ShortToByte(direction_Short));

    char sum= 0;
    for(char i : instruct.mid(2,instruct.size() - 2))
    {
        sum += i;
    }
    instruct.append(sum);
    emit giveMsgToClientSocketToGo(instruct);
}


void sendDataWidget::aimms_30_plane_Instruct()
{
    qDebug() <<"AIMMS-30 PLANE MSG";
    QByteArray instruct;
    instruct.append(QByteArray::fromHex(instructHead));
    //帧标识
    instruct.append(QByteArray::fromHex("06"));
    //帧长度 //需修改
    short length = 18;
    length = BLEndianShort(length);
    QByteArray lengthByte = ShortToByte(length);
    instruct.append(lengthByte);
    //飞机编号
    instruct.append(planeNumber);
    //时间
    QTime time = QDateTime::currentDateTime().time();
    char hour =  time.hour();
    instruct.append(hour);
    char min = time.minute();
    instruct.append(min);
    char second = time.second();
    instruct.append(second);
    //真空速
    QString speed = ui->plane_speed->text();
    if(speed.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("AIMMS-30飞机状体真空速不能为空"));
        return;
    }
    if(!isDigitStr(speed)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("AIMMS-30飞机状态真空速不能为非数字"));
        return;
    }
    short speed_Short = speed.toShort();
    speed_Short = BLEndianShort(speed_Short);
    instruct.append(ShortToByte(speed_Short));
    //垂直风
    QString wind = ui->plane_wind->text();
    if(wind.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("AIMMS-30飞机垂直风不能为空"));
        return;
    }
    if(!isDigitStr(wind)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("AIMMS-30飞机垂直风不能为非数字"));
        return;
    }
    short wind_Short = wind.toShort();
    wind_Short = BLEndianShort(wind_Short);
    instruct.append(ShortToByte(wind_Short));

    char sum= 0;
    for(char i : instruct.mid(2,instruct.size() - 2))
    {
        sum += i;
    }
    instruct.append(sum);
    emit giveMsgToClientSocketToGo(instruct);
}



void sendDataWidget::fcdp_Instruct()
{
    qDebug() <<"FCDP MSG";
    QByteArray instruct;
    instruct.append(QByteArray::fromHex(instructHead));
    //帧标识
    instruct.append(QByteArray::fromHex("07"));
    //帧长度 //需修改
    short length = 58;
    length = BLEndianShort(length);
    QByteArray lengthByte = ShortToByte(length);
    instruct.append(lengthByte);
    //飞机编号
    instruct.append(planeNumber);
    //时间
    QTime time = QDateTime::currentDateTime().time();
    char hour =  time.hour();
    instruct.append(hour);
    char min = time.minute();
    instruct.append(min);
    char second = time.second();
    instruct.append(second);
    //浓度
    QString thickness = ui->fcdp_thickness->text();
    if(thickness.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("FCDP浓度应为三位数字"));
        return;
    }
    if(!isDigitStr(thickness)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("FCDP浓度不能为非数字"));
        return;
    }

    int thicknessInt = thickness.toInt();
    thicknessInt = BLEndianInt(thicknessInt);
    QByteArray ba = IntToByte(thicknessInt);
    instruct.append(ba);

    // 0通道
    QString passage = ui->fcdp_num->text();
    if(passage.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("FCDP粒子数量不能为空"));
        return;
    }
    if(!isDigitStr(passage)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("FCDP粒子数量不能为非数字"));
        return;
    }
    short passage_Short = passage.toShort();
    passage_Short = BLEndianShort(passage_Short);
    instruct.append(ShortToByte(passage_Short));
    //1通道
    passage = ui->fcdp_num_1->text();
    if(passage.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("FCDP粒子数量不能为空"));
        return;
    }
    if(!isDigitStr(passage)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("FCDP粒子数量不能为非数字"));
        return;
    }
    passage_Short = passage.toShort();
    passage_Short = BLEndianShort(passage_Short);
    instruct.append(ShortToByte(passage_Short));
    //2通道
    passage = ui->fcdp_num_2->text();
    if(passage.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("FCDP粒子数量不能为空"));
        return;
    }
    if(!isDigitStr(passage)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("FCDP粒子数量不能为非数字"));
        return;
    }
    passage_Short = passage.toShort();
    passage_Short = BLEndianShort(passage_Short);
    instruct.append(ShortToByte(passage_Short));
    //3通道
    passage = ui->fcdp_num_3->text();
    if(passage.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("FCDP粒子数量不能为空"));
        return;
    }
    if(!isDigitStr(passage)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("FCDP粒子数量不能为非数字"));
        return;
    }
    passage_Short = passage.toShort();
    passage_Short = BLEndianShort(passage_Short);
    instruct.append(ShortToByte(passage_Short));
    //4通道
    passage = ui->fcdp_num_4->text();
    if(passage.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("FCDP粒子数量不能为空"));
        return;
    }
    if(!isDigitStr(passage)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("FCDP粒子数量不能为非数字"));
        return;
    }
    passage_Short = passage.toShort();
    passage_Short = BLEndianShort(passage_Short);
    instruct.append(ShortToByte(passage_Short));
    //5通道
    passage = ui->fcdp_num_5->text();
    if(passage.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("FCDP粒子数量不能为空"));
        return;
    }
    if(!isDigitStr(passage)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("FCDP粒子数量不能为非数字"));
        return;
    }
    passage_Short = passage.toShort();
    passage_Short = BLEndianShort(passage_Short);
    instruct.append(ShortToByte(passage_Short));
    //6通道
    passage = ui->fcdp_num_6->text();
    if(passage.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("FCDP粒子数量不能为空"));
        return;
    }
    if(!isDigitStr(passage)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("FCDP粒子数量不能为非数字"));
        return;
    }
    passage_Short = passage.toShort();
    passage_Short = BLEndianShort(passage_Short);
    instruct.append(ShortToByte(passage_Short));
    //7通道
    passage = ui->fcdp_num_7->text();
    if(passage.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("FCDP粒子数量不能为空"));
        return;
    }
    if(!isDigitStr(passage)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("FCDP粒子数量不能为非数字"));
        return;
    }
    passage_Short = passage.toShort();
    passage_Short = BLEndianShort(passage_Short);
    instruct.append(ShortToByte(passage_Short));
    //8通道
    passage = ui->fcdp_num_8->text();
    if(passage.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("FCDP粒子数量不能为空"));
        return;
    }
    if(!isDigitStr(passage)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("FCDP粒子数量不能为非数字"));
        return;
    }
    passage_Short = passage.toShort();
    passage_Short = BLEndianShort(passage_Short);
    instruct.append(ShortToByte(passage_Short));
    //9通道
    passage = ui->fcdp_num_9->text();
    if(passage.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("FCDP粒子数量不能为空"));
        return;
    }
    if(!isDigitStr(passage)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("FCDP粒子数量不能为非数字"));
        return;
    }
    passage_Short = passage.toShort();
    passage_Short = BLEndianShort(passage_Short);
    instruct.append(ShortToByte(passage_Short));
    //10通道
    passage = ui->fcdp_num_10->text();
    if(passage.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("FCDP粒子数量不能为空"));
        return;
    }
    if(!isDigitStr(passage)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("FCDP粒子数量不能为非数字"));
        return;
    }
    passage_Short = passage.toShort();
    passage_Short = BLEndianShort(passage_Short);
    instruct.append(ShortToByte(passage_Short));
    //11通道
    passage = ui->fcdp_num_11->text();
    if(passage.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("FCDP粒子数量不能为空"));
        return;
    }
    if(!isDigitStr(passage)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("FCDP粒子数量不能为非数字"));
        return;
    }
    passage_Short = passage.toShort();
    passage_Short = BLEndianShort(passage_Short);
    instruct.append(ShortToByte(passage_Short));
    //12通道
    passage = ui->fcdp_num_12->text();
    if(passage.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("FCDP粒子数量不能为空"));
        return;
    }
    if(!isDigitStr(passage)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("FCDP粒子数量不能为非数字"));
        return;
    }
    passage_Short = passage.toShort();
    passage_Short = BLEndianShort(passage_Short);
    instruct.append(ShortToByte(passage_Short));
    //13通道
    passage = ui->fcdp_num_13->text();
    if(passage.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("FCDP粒子数量不能为空"));
        return;
    }
    if(!isDigitStr(passage)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("FCDP粒子数量不能为非数字"));
        return;
    }
    passage_Short = passage.toShort();
    passage_Short = BLEndianShort(passage_Short);
    instruct.append(ShortToByte(passage_Short));
    //14通道
    passage = ui->fcdp_num_14->text();
    if(passage.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("FCDP粒子数量不能为空"));
        return;
    }
    if(!isDigitStr(passage)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("FCDP粒子数量不能为非数字"));
        return;
    }
    passage_Short = passage.toShort();
    passage_Short = BLEndianShort(passage_Short);
    instruct.append(ShortToByte(passage_Short));
    //15通道
    passage = ui->fcdp_num_15->text();
    if(passage.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("FCDP粒子数量不能为空"));
        return;
    }
    if(!isDigitStr(passage)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("FCDP粒子数量不能为非数字"));
        return;
    }
    passage_Short = passage.toShort();
    passage_Short = BLEndianShort(passage_Short);
    instruct.append(ShortToByte(passage_Short));
    //16通道
    passage = ui->fcdp_num_16->text();
    if(passage.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("FCDP粒子数量不能为空"));
        return;
    }
    if(!isDigitStr(passage)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("FCDP粒子数量不能为非数字"));
        return;
    }
    passage_Short = passage.toShort();
    passage_Short = BLEndianShort(passage_Short);
    instruct.append(ShortToByte(passage_Short));
    //17通道
    passage = ui->fcdp_num_17->text();
    if(passage.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("FCDP粒子数量不能为空"));
        return;
    }
    if(!isDigitStr(passage)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("FCDP粒子数量不能为非数字"));
        return;
    }
    passage_Short = passage.toShort();
    passage_Short = BLEndianShort(passage_Short);
    instruct.append(ShortToByte(passage_Short));
    //18通道
    passage = ui->fcdp_num_18->text();
    if(passage.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("FCDP粒子数量不能为空"));
        return;
    }
    if(!isDigitStr(passage)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("FCDP粒子数量不能为非数字"));
        return;
    }
    passage_Short = passage.toShort();
    passage_Short = BLEndianShort(passage_Short);
    instruct.append(ShortToByte(passage_Short));
    //19通道
    passage = ui->fcdp_num_19->text();
    if(passage.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("FCDP粒子数量不能为空"));
        return;
    }
    if(!isDigitStr(passage)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("FCDP粒子数量不能为非数字"));
        return;
    }
    passage_Short = passage.toShort();
    passage_Short = BLEndianShort(passage_Short);
    instruct.append(ShortToByte(passage_Short));

    char sum= 0;
    for(char i : instruct.mid(2,instruct.size() - 2))
    {
        sum += i;
    }

    instruct.append(sum);
    emit giveMsgToClientSocketToGo(instruct);
}



void sendDataWidget::dxas_Instruct()
{
    qDebug() <<"D/XAS  MSG";
    QByteArray instruct;
    instruct.append(QByteArray::fromHex(instructHead));
    //帧标识
    instruct.append(QByteArray::fromHex("08"));
    //帧长度 //需修改
    short length = 34;
    length = BLEndianShort(length);
    QByteArray lengthByte = ShortToByte(length);
    instruct.append(lengthByte);
    //飞机编号
    instruct.append(planeNumber);
    //时间
    QTime time = QDateTime::currentDateTime().time();
    char hour =  time.hour();
    instruct.append(hour);
    char min = time.minute();
    instruct.append(min);
    char second = time.second();
    instruct.append(second);
    //数据1
    QString msg = ui->dxas_1->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("DXAS数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("DXAS数据不能为非数字"));
        return;
    }
    short msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据2
    msg = ui->dxas_2->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("DXAS数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("DXAS数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据3
    msg = ui->dxas_3->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("DXAS数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("DXAS数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据4
    msg = ui->dxas_4->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("DXAS数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("DXAS数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据5
    msg = ui->dxas_5->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("DXAS数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("DXAS数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据6
    msg = ui->dxas_6->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("DXAS数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("DXAS数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据7
    msg = ui->dxas_7->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("DXAS数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("DXAS数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据8
    msg = ui->dxas_8->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("DXAS数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("DXAS数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据9
    msg = ui->dxas_9->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("DXAS数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("DXAS数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据10
    msg = ui->dxas_10->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("DXAS数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("DXAS数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    char sum= 0;
    for(char i : instruct.mid(2,instruct.size() - 2))
    {
        sum += i;
    }
    instruct.append(sum);
    emit giveMsgToClientSocketToGo(instruct);
}


QByteArray sendDataWidget::ShortToByte(short num){
    QByteArray bar;
    bar.resize(2);
    memcpy(bar.data(),&num,sizeof(num));
    return bar;
}

bool sendDataWidget::isDigitStr(QString src)
{

    QByteArray ba = src.toLatin1();//QString 转换为 char*
    const char *s = ba.data();

    while(*s && *s>='0' && *s<='9') s++;

    if (*s)
    { //不是纯数字
        if(src.contains('-'))
            return true;
        if(src.contains('.'))
            return true;
        return false;
    }
    else
    { //纯数字
        return true;
    }
}

void sendDataWidget::lwc_twc_Instruct()
{
    qDebug() <<"LWC/TWC  MSG";
    QByteArray instruct;
    instruct.append(QByteArray::fromHex(instructHead));
    //帧标识
    instruct.append(QByteArray::fromHex("09"));
    //帧长度 //需修改
    short length = 18;
    length = BLEndianShort(length);
    QByteArray lengthByte = ShortToByte(length);
    instruct.append(lengthByte);
    //飞机编号
    instruct.append(planeNumber);
    //时间
    QTime time = QDateTime::currentDateTime().time();
    char hour =  time.hour();
    instruct.append(hour);
    char min = time.minute();
    instruct.append(min);
    char second = time.second();
    instruct.append(second);
    //数据1
    QString msg = ui->lwctwc_1->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("LWC/TWC数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("LWC/TWC数据不能为非数字"));
        return;
    }
    short msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据2
    msg = ui->lwctwc_2->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("LWC/TWC数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("LWC/TWC数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));

    char sum= 0;
    for(char i : instruct.mid(2,instruct.size() - 2))
    {
        sum += i;
    }
    instruct.append(sum);
    emit giveMsgToClientSocketToGo(instruct);
}

void sendDataWidget::pcasp_Instruct()
{
    qDebug() <<"PCASP  MSG";
    QByteArray instruct;
    instruct.append(QByteArray::fromHex(instructHead));
    //帧标识
    instruct.append(QByteArray::fromHex("0A"));
    //帧长度
    short length = 110;
    length = BLEndianShort(length);
    QByteArray lengthByte = ShortToByte(length);
    instruct.append(lengthByte);
    //飞机编号
    instruct.append(planeNumber);
    //时间
    QTime time = QDateTime::currentDateTime().time();
    char hour =  time.hour();
    instruct.append(hour);
    char min = time.minute();
    instruct.append(min);
    char second = time.second();
    instruct.append(second);
    //数据1
    QString msg = ui->pcasp_1->text();
    if(msg.isEmpty() && (msg.size() !=3)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("PCASP1数据长度不正确"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("PCASP数据不能为非数字"));
        return;
    }
    instruct.append(msg);
    //数据2
    msg = ui->pcasp_2->text();
    if(msg.isEmpty() && (msg.size() != 3)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("PCASP1数据长度不正确"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("PCASP数据不能为非数字"));
        return;
    }
    instruct.append(msg);
    //数据3
    msg = ui->pcasp_3->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("PCASP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("PCASP数据不能为非数字"));
        return;
    }

    short msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据4
    msg = ui->pcasp_4->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("PCASP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("PCASP数据不能为非数字"));
        return;
    }

    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据5
    msg = ui->pcasp_5->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("PCASP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("PCASP数据不能为非数字"));
        return;
    }

    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据6
    msg = ui->pcasp_6->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("PCASP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("PCASP数据不能为非数字"));
        return;
    }

    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据7
    msg = ui->pcasp_7->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("PCASP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("PCASP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //粒子1
    msg = ui->pcasp_01->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("PCASP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("PCASP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //粒子2
    msg = ui->pcasp_02->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("PCASP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("PCASP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //粒子3
    msg = ui->pcasp_03->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("PCASP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("PCASP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //粒子4
    msg = ui->pcasp_04->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("PCASP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("PCASP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //粒子5
    msg = ui->pcasp_05->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("PCASP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("PCASP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //粒子6
    msg = ui->pcasp_06->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("PCASP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("PCASP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //粒子7
    msg = ui->pcasp_07->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("PCASP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("PCASP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //粒子8
    msg = ui->pcasp_08->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("PCASP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("PCASP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //粒子9
    msg = ui->pcasp_09->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("PCASP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("PCASP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //粒子10
    msg = ui->pcasp_010->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("PCASP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("PCASP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //粒子11
    msg = ui->pcasp_011->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("PCASP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("PCASP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //粒子12
    msg = ui->pcasp_012->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("PCASP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("PCASP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //粒子13
    msg = ui->pcasp_013->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("PCASP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("PCASP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //粒子14
    msg = ui->pcasp_014->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("PCASP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("PCASP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //粒子15
    msg = ui->pcasp_015->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("PCASP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("PCASP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //粒子16
    msg = ui->pcasp_016->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("PCASP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("PCASP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //粒子17
    msg = ui->pcasp_017->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("PCASP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("PCASP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //粒子18
    msg = ui->pcasp_018->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("PCASP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("PCASP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //粒子19
    msg = ui->pcasp_019->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("PCASP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("PCASP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //粒子20
    msg = ui->pcasp_020->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("PCASP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("PCASP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //粒子21  //从这个开始后面都是int的了
    msg = ui->pcasp_021->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("PCASP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("PCASP数据不能为非数字"));
        return;
    }
    int msg_Int = msg.toInt();
    instruct.append(IntToByte(BLEndianInt(msg_Int)));
    //粒子22
    msg = ui->pcasp_022->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("PCASP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("PCASP数据不能为非数字"));
        return;
    }
    msg_Int = msg.toInt();
    instruct.append(IntToByte(BLEndianInt(msg_Int)));
    //粒子23
    msg = ui->pcasp_023->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("PCASP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("PCASP数据不能为非数字"));
        return;
    }
    msg_Int = msg.toInt();
    instruct.append(IntToByte(BLEndianInt(msg_Int)));
    //粒子24
    msg = ui->pcasp_024->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("PCASP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("PCASP数据不能为非数字"));
        return;
    }
    msg_Int = msg.toInt();
    instruct.append(IntToByte(BLEndianInt(msg_Int)));
    //粒子25
    msg = ui->pcasp_025->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("PCASP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("PCASP数据不能为非数字"));
        return;
    }
    msg_Int = msg.toInt();
    instruct.append(IntToByte(BLEndianInt(msg_Int)));
    //粒子26
    msg = ui->pcasp_026->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("PCASP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("PCASP数据不能为非数字"));
        return;
    }
    msg_Int = msg.toInt();
    instruct.append(IntToByte(BLEndianInt(msg_Int)));
    //粒子27
    msg = ui->pcasp_027->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("PCASP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("PCASP数据不能为非数字"));
        return;
    }
    msg_Int = msg.toInt();
    instruct.append(IntToByte(BLEndianInt(msg_Int)));
    //粒子28
    msg = ui->pcasp_028->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("PCASP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("PCASP数据不能为非数字"));
        return;
    }
    msg_Int = msg.toInt();
    instruct.append(IntToByte(BLEndianInt(msg_Int)));
    //粒子29
    msg = ui->pcasp_029->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("PCASP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("PCASP数据不能为非数字"));
        return;
    }
    msg_Int = msg.toInt();
    instruct.append(IntToByte(BLEndianInt(msg_Int)));
    //粒子30
    msg = ui->pcasp_030->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("PCASP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("PCASP数据不能为非数字"));
        return;
    }
    msg_Int = msg.toInt();
    instruct.append(IntToByte(BLEndianInt(msg_Int)));
    //
    char sum= 0;
    for(char i : instruct.mid(2,instruct.size() - 2))
    {
        sum += i;
    }
    instruct.append(sum);
    emit giveMsgToClientSocketToGo(instruct);
}


void sendDataWidget::lwc_100_Instruct()
{
    qDebug() <<"LWC/100  MSG";
    QByteArray instruct;
    instruct.append(QByteArray::fromHex(instructHead));
    //帧标识
    instruct.append(QByteArray::fromHex("0B"));
    //帧长度 //需修改
    short length = 16;
    length = BLEndianShort(length);
    QByteArray lengthByte = ShortToByte(length);
    instruct.append(lengthByte);
    //飞机编号
    instruct.append(planeNumber);
    //时间
    QTime time = QDateTime::currentDateTime().time();
    char hour =  time.hour();
    instruct.append(hour);
    char min = time.minute();
    instruct.append(min);
    char second = time.second();
    instruct.append(second);
    //数据1
    QString msg = ui->lwc_100->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("LWC-100数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("LWC-100数据不能为非数字"));
        return;
    }
    short msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));

    char sum= 0;
    for(char i : instruct.mid(2,instruct.size() - 2))
    {
        sum += i;
    }
    instruct.append(sum);
    emit giveMsgToClientSocketToGo(instruct);
}


void sendDataWidget::cip_Instruct()
{
    qDebug() <<"CIP  MSG";
    QByteArray instruct;
    instruct.append(QByteArray::fromHex(instructHead));
    //帧标识
    instruct.append(QByteArray::fromHex("0C"));
    //帧长度 //需修改
    short length = 150;
    length = BLEndianShort(length);
    QByteArray lengthByte = ShortToByte(length);
    instruct.append(lengthByte);
    //飞机编号
    instruct.append(planeNumber);
    //时间
    QTime time = QDateTime::currentDateTime().time();
    char hour =  time.hour();
    instruct.append(hour);
    char min = time.minute();
    instruct.append(min);
    char second = time.second();
    instruct.append(second);
    //数据1
    QString msg = ui->cip_1->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为非数字"));
        return;
    }
    short msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据2
    msg = ui->cip_2->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据3
    msg = ui->cip_3->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据4
    msg = ui->cip_4->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据5
    msg = ui->cip_5->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据6
    msg = ui->cip_6->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据07
    msg = ui->cip_07->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据08
    msg = ui->cip_08->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据09
    msg = ui->cip_09->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据010
    msg = ui->cip_010->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据011
    msg = ui->cip_011->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据012
    msg = ui->cip_012->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据013
    msg = ui->cip_013->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据014
    msg = ui->cip_014->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据015
    msg = ui->cip_015->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据016
    msg = ui->cip_016->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据017
    msg = ui->cip_017->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据018
    msg = ui->cip_018->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据019
    msg = ui->cip_019->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据020
    msg = ui->cip_020->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据021
    msg = ui->cip_021->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据022
    msg = ui->cip_022->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据023
    msg = ui->cip_023->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据024
    msg = ui->cip_024->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据025
    msg = ui->cip_025->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据026
    msg = ui->cip_026->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据027
    msg = ui->cip_027->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据028
    msg = ui->cip_028->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据029
    msg = ui->cip_029->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据030
    msg = ui->cip_030->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据031
    msg = ui->cip_031->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据032
    msg = ui->cip_032->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据033
    msg = ui->cip_033->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据034
    msg = ui->cip_034->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据035
    msg = ui->cip_035->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据036
    msg = ui->cip_036->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据037
    msg = ui->cip_037->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据038
    msg = ui->cip_038->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据039
    msg = ui->cip_039->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据040
    msg = ui->cip_040->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据041
    msg = ui->cip_041->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据042
    msg = ui->cip_042->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据043
    msg = ui->cip_043->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据044
    msg = ui->cip_044->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据045
    msg = ui->cip_045->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据046
    msg = ui->cip_046->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据047
    msg = ui->cip_047->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据048
    msg = ui->cip_048->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据049
    msg = ui->cip_049->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据050
    msg = ui->cip_050->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据051
    msg = ui->cip_051->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据052
    msg = ui->cip_052->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据053
    msg = ui->cip_053->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据054
    msg = ui->cip_054->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据055
    msg = ui->cip_055->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据056
    msg = ui->cip_056->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据057
    msg = ui->cip_057->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据058
    msg = ui->cip_058->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据059
    msg = ui->cip_059->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据060
    msg = ui->cip_060->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据061
    msg = ui->cip_061->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据062
    msg = ui->cip_062->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据063
    msg = ui->cip_063->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据064
    msg = ui->cip_064->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据065
    msg = ui->cip_065->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据066
    msg = ui->cip_066->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据067
    msg = ui->cip_067->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据068
    msg = ui->cip_068->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CIP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //*******//
    char sum= 0;
    for(char i : instruct.mid(2,instruct.size() - 2))
    {
        sum += i;
    }
    instruct.append(sum);
    emit giveMsgToClientSocketToGo(instruct);
}


void sendDataWidget::setInitialParameter()
{
    //初始化数据
    QStringList WeaponDealerList;//武器厂商

    WeaponDealerList<<QStringLiteral("其他")<<QStringLiteral("陕西中天火箭股份有限公司")<<QStringLiteral("吉林三三零五机械厂")
                   <<QStringLiteral("山西北方晋东化工有限公司")<<QStringLiteral("北方华安工业集团有限公司")
                  <<QStringLiteral("重庆长安工业（集团）有限责任公司")<<QStringLiteral("内蒙古北方保安民爆器材有限公司")
                 <<QStringLiteral("江西新余国科科技股份有限公司")<<QStringLiteral("国营云南包装厂")<<QStringLiteral("晋西工业集团有限责任公司")
                <<QStringLiteral("西安北方秦川集团有限公司")<<QStringLiteral("中国华云");
    // <<QStringLiteral("其他")<<QStringLiteral("其他")<<QStringLiteral("其他")<<QStringLiteral("其他");


    QStringList cuihuaWhatList_1;
    cuihuaWhatList_1<<QStringLiteral("焰条(冷云)") << QStringLiteral("焰条(暖云)");
    QStringList cuihuaWhatList_2;
    cuihuaWhatList_2<<QStringLiteral("焰弹(冷云)") << QStringLiteral("焰弹(暖云)");
    QStringList cuihuaWhatList_3;
    cuihuaWhatList_3<<QStringLiteral("粉剂");
    QStringList cuihuaWhatList_4;
    cuihuaWhatList_4<<QStringLiteral("液氮") ;

    //初始化 aimms 天气
    ui->aimms_atmos->setText("70");
    ui->aimms_direction->setText("70");
    ui->aimms_humidity->setText("70");
    ui->aimms_speed->setText("70");
    ui->aimms_temper->setText("70");
    //初始化 aimms 飞机状态
    ui->plane_speed->setText("67");
    ui->plane_wind->setText("67");
    //fcdp 状态
    ui->fcdp_thickness->setText("132");
    ui->fcdp_num->setText("32");
    ui->fcdp_num_1->setText("32");
    ui->fcdp_num_2->setText("32");
    ui->fcdp_num_3->setText("32");
    ui->fcdp_num_4->setText("32");
    ui->fcdp_num_5->setText("32");
    ui->fcdp_num_6->setText("32");
    ui->fcdp_num_7->setText("32");
    ui->fcdp_num_8->setText("32");
    ui->fcdp_num_9->setText("32");
    ui->fcdp_num_10->setText("32");
    ui->fcdp_num_11->setText("32");
    ui->fcdp_num_12->setText("32");
    ui->fcdp_num_13->setText("32");
    ui->fcdp_num_14->setText("32");
    ui->fcdp_num_15->setText("32");
    ui->fcdp_num_16->setText("32");
    ui->fcdp_num_17->setText("32");
    ui->fcdp_num_18->setText("32");
    ui->fcdp_num_19->setText("32");
    //dxas数据
    ui->dxas_1->setText("34");
    ui->dxas_2->setText("34");
    ui->dxas_3->setText("34");
    ui->dxas_4->setText("34");
    ui->dxas_5->setText("34");
    ui->dxas_6->setText("34");
    ui->dxas_7->setText("34");
    ui->dxas_8->setText("34");
    ui->dxas_9->setText("34");
    ui->dxas_10->setText("34");
    //LWC_TWC数据
    ui->lwctwc_1->setText("43");
    ui->lwctwc_2->setText("43");
    //
    ui->pcasp_1->setText("132");
    ui->pcasp_2->setText("132");
    ui->pcasp_3->setText("78");
    ui->pcasp_4->setText("78");
    ui->pcasp_5->setText("78");
    ui->pcasp_6->setText("78");
    ui->pcasp_7->setText("78");
    ui->pcasp_01->setText("70");
    ui->pcasp_02->setText("70");
    ui->pcasp_03->setText("70");
    ui->pcasp_04->setText("70");
    ui->pcasp_05->setText("70");
    ui->pcasp_06->setText("70");
    ui->pcasp_07->setText("70");
    ui->pcasp_08->setText("70");
    ui->pcasp_09->setText("70");
    ui->pcasp_010->setText("70");
    ui->pcasp_011->setText("70");
    ui->pcasp_012->setText("70");
    ui->pcasp_013->setText("70");
    ui->pcasp_014->setText("70");
    ui->pcasp_015->setText("70");
    ui->pcasp_016->setText("70");
    ui->pcasp_017->setText("70");
    ui->pcasp_018->setText("70");
    ui->pcasp_019->setText("70");
    ui->pcasp_020->setText("70");
    ui->pcasp_021->setText("70");
    ui->pcasp_022->setText("70");
    ui->pcasp_023->setText("70");
    ui->pcasp_024->setText("70");
    ui->pcasp_025->setText("70");
    ui->pcasp_026->setText("70");
    ui->pcasp_027->setText("70");
    ui->pcasp_028->setText("70");
    ui->pcasp_029->setText("70");
    ui->pcasp_030->setText("70");
    //lwc-100 数据
    ui->lwc_100->setText("100");
    //cip
    ui->cip_1->setText("103");
    ui->cip_2->setText("103");
    ui->cip_3->setText("103");
    ui->cip_4->setText("103");
    ui->cip_5->setText("103");
    ui->cip_6->setText("103");
    ui->cip_07->setText("67");
    ui->cip_08->setText("67");
    ui->cip_09->setText("67");
    ui->cip_010->setText("67");
    ui->cip_011->setText("67");
    ui->cip_012->setText("67");
    ui->cip_013->setText("67");
    ui->cip_014->setText("67");
    ui->cip_015->setText("67");
    ui->cip_016->setText("67");
    ui->cip_017->setText("67");
    ui->cip_018->setText("67");
    ui->cip_019->setText("67");
    ui->cip_020->setText("67");
    ui->cip_021->setText("67");
    ui->cip_022->setText("67");
    ui->cip_023->setText("67");
    ui->cip_024->setText("67");
    ui->cip_025->setText("67");
    ui->cip_026->setText("67");
    ui->cip_027->setText("67");
    ui->cip_028->setText("67");
    ui->cip_029->setText("67");
    ui->cip_030->setText("67");
    ui->cip_031->setText("67");
    ui->cip_032->setText("67");
    ui->cip_033->setText("67");
    ui->cip_034->setText("67");
    ui->cip_035->setText("67");
    ui->cip_036->setText("67");
    ui->cip_037->setText("67");
    ui->cip_038->setText("67");
    ui->cip_039->setText("67");
    ui->cip_040->setText("67");
    ui->cip_041->setText("67");
    ui->cip_042->setText("67");
    ui->cip_043->setText("67");
    ui->cip_044->setText("67");
    ui->cip_045->setText("67");
    ui->cip_046->setText("67");
    ui->cip_047->setText("67");
    ui->cip_048->setText("67");
    ui->cip_049->setText("67");
    ui->cip_050->setText("67");
    ui->cip_051->setText("67");
    ui->cip_052->setText("67");
    ui->cip_053->setText("67");
    ui->cip_054->setText("67");
    ui->cip_055->setText("67");
    ui->cip_056->setText("67");
    ui->cip_057->setText("67");
    ui->cip_058->setText("67");
    ui->cip_059->setText("67");
    ui->cip_060->setText("67");
    ui->cip_061->setText("67");
    ui->cip_062->setText("67");
    ui->cip_063->setText("67");
    ui->cip_064->setText("67");
    ui->cip_065->setText("67");
    ui->cip_066->setText("67");
    ui->cip_067->setText("67");
    ui->cip_068->setText("67");
    //cdp
    ui->cdp_01->setText("77");
    ui->cdp_02->setText("77");
    ui->cdp_03->setText("77");
    ui->cdp_04->setText("77");
    ui->cdp_05->setText("77");
    ui->cdp_06->setText("77");
    ui->cdp_07->setText("77");
    ui->cdp_08->setText("77");
    ui->cdp_09->setText("77");
    ui->cdp_010->setText("77");
    ui->cdp_011->setText("77");
    ui->cdp_012->setText("77");
    ui->cdp_013->setText("77");
    ui->cdp_014->setText("77");
    ui->cdp_015->setText("77");
    ui->cdp_016->setText("77");
    ui->cdp_017->setText("77");
    ui->cdp_018->setText("77");
    ui->cdp_019->setText("77");
    ui->cdp_020->setText("77");
    ui->cdp_021->setText("77");
    ui->cdp_022->setText("77");
    ui->cdp_023->setText("77");
    ui->cdp_024->setText("77");
    ui->cdp_025->setText("77");
    ui->cdp_026->setText("77");
    ui->cdp_027->setText("77");
    ui->cdp_028->setText("77");
    ui->cdp_029->setText("77");
    ui->cdp_030->setText("77");
    ui->cdp_031->setText("77");
    ui->cdp_032->setText("77");
    ui->cdp_033->setText("77");
    ui->cdp_034->setText("77");
    //2D-S
    ui->d_s->setText("67");
    //    //北斗
    //    ui->beidou_1->setChecked(true);
    //    ui->beidou_2->setChecked(true);
    //飞机状态
    //ui->status_28->setText("28");
    //海事作业信息
    ui->haishi_workinfo_0->setText("113.82490");
    ui->haishi_workinfo_1->setText("34.54030");
    ui->haishi_workinfo_2->setText("03403");
    ui->haishi_workinfo_3->setText("170");
    ui->haishi_workinfo_4->setText("045");
    ui->haishi_workinfo_5->setText("-10.5");
    ui->haishi_workinfo_6->setText("089");

    ui->bullet_comboBox_0->addItems(WeaponDealerList);
    ui->bullet_comboBox_1->addItems(WeaponDealerList);
    ui->bullet_comboBox_2->addItems(WeaponDealerList);
    ui->bullet_comboBox_3->addItems(WeaponDealerList);
    ui->bullet_comboBox_0->setCurrentIndex(2);
    ui->bullet_comboBox_1->setCurrentIndex(2);
    ui->bullet_comboBox_2->setCurrentIndex(2);
    ui->bullet_comboBox_3->setCurrentIndex(2);

    ui->bulletTypeBox_0->addItems(cuihuaWhatList_1);
    ui->bulletTypeBox_1->addItems(cuihuaWhatList_2);
    ui->bulletTypeBox_2->addItems(cuihuaWhatList_4);
    ui->bulletTypeBox_3->addItems(cuihuaWhatList_3);

    ui->workinfo_1->setText("2");
    ui->workinfo_2->setText("4");
    ui->workinfo_3->setText("6");
    ui->workinfo_4->setText("8");
    //飞机位置
    ui->plane_location->setText("35");
    ui->plane_location_2->setText("35");
    ui->plane_location_3->setText("35");
    ui->plane_location_4->setText("-35");
    ui->plane_location_5->setText("-35");
    //海事卫星控制
    ui->haishi_0->setChecked(true);
    //空地状态数据帧
    ui->skyGround_0->setChecked(true);
    //飞机编号和 弹厂商
    ui->gunAndPlane_0->setText(planeNumber);
    ui->gunAndPlane_1->setText("A4");
    //
    ui->cuihua_01->setText("01");
    ui->cuihua_02->setText("04");
    //北斗作业信息
    ui->beidouWorkInfo_0->setText("45.12345");
    ui->beidouWorkInfo_1->setText("46.12345");
    ui->beidouWorkInfo_2->setText("999");
    ui->beidouWorkInfo_3->setText("699");  //ushort
    ui->beidouWorkInfo_4->setText("300");
    ui->beidouWorkInfo_5->setText("900");
    ui->beidouWorkInfo_6->setText("50");
    ui->beidouWorkInfo_7->setText("5000");


    ui->weaponComboBox_1->addItems(WeaponDealerList);
    ui->weaponComboBox_1->setCurrentIndex(2);
    ui->weaponComboBox_2->addItems(WeaponDealerList);
    ui->weaponComboBox_2->setCurrentIndex(2);
    ui->weaponComboBox_3->addItems(WeaponDealerList);
    ui->weaponComboBox_3->setCurrentIndex(2);
    ui->weaponComboBox_4->addItems(WeaponDealerList);
    ui->weaponComboBox_4->setCurrentIndex(2);



    ui->bulletTypeCombobox_1->addItems(cuihuaWhatList_1);
    ui->bulletTypeCombobox_2->addItems(cuihuaWhatList_2);
    ui->bulletTypeCombobox_3->addItems(cuihuaWhatList_3);
    ui->bulletTypeCombobox_4->addItems(cuihuaWhatList_4);

    ui->howManyBullet_1->setText("20");
    ui->howManyBullet_2->setText("20");
    ui->howManyBullet_3->setText("20");
    ui->howManyBullet_4->setText("20");

}


QByteArray sendDataWidget::IntToByte(int num){
    QByteArray bar;
    bar.resize(4);
    memcpy(bar.data(),&num,sizeof(num));
    return bar;
}


void sendDataWidget::cdp_Instruct()
{
    qDebug() <<"CDP PLANE MSG";
    QByteArray instruct;
    instruct.append(QByteArray::fromHex(instructHead));
    //帧标识
    instruct.append(QByteArray::fromHex("0D"));
    //帧长度 //需修改
    short length = 92;
    length = BLEndianShort(length);
    QByteArray lengthByte = ShortToByte(length);
    instruct.append(lengthByte);
    //飞机编号
    instruct.append(planeNumber);
    //时间
    QTime time = QDateTime::currentDateTime().time();
    char hour =  time.hour();
    instruct.append(hour);
    char min = time.minute();
    instruct.append(min);
    char second = time.second();
    instruct.append(second);
    //数据1
    QString msg = ui->cdp_01->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CDP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CDP数据不能为非数字"));
        return;
    }
    short msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据2
    msg = ui->cdp_02->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CDP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CDP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据3
    msg = ui->cdp_03->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CDP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CDP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据4
    msg = ui->cdp_04->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CDP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CDP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据5
    msg = ui->cdp_05->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CDP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CDP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据6
    msg = ui->cdp_06->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CDP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CDP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据7
    msg = ui->cdp_07->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CDP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CDP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据8
    msg = ui->cdp_08->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CDP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CDP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据9
    msg = ui->cdp_09->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CDP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CDP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据10
    msg = ui->cdp_010->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CDP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CDP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据11
    msg = ui->cdp_011->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CDP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CDP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据12
    msg = ui->cdp_012->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CDP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CDP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据13
    msg = ui->cdp_013->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CDP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CDP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据14
    msg = ui->cdp_014->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CDP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CDP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据15
    msg = ui->cdp_015->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CDP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CDP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据16
    msg = ui->cdp_016->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CDP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CDP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据17
    msg = ui->cdp_017->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CDP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CDP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据18
    msg = ui->cdp_018->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CDP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CDP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据19
    msg = ui->cdp_019->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CDP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CDP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据20
    msg = ui->cdp_020->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CDP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CDP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据21
    msg = ui->cdp_021->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CDP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CDP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据22
    msg = ui->cdp_022->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CDP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CDP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据23
    msg = ui->cdp_023->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CDP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CDP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据24
    msg = ui->cdp_024->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CDP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CDP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据25
    msg = ui->cdp_025->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CDP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CDP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据26
    msg = ui->cdp_026->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CDP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CDP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据27
    msg = ui->cdp_027->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CDP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CDP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据28
    msg = ui->cdp_028->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CDP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CDP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据29
    msg = ui->cdp_029->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CDP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CDP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据30
    msg = ui->cdp_030->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CDP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CDP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据31
    msg = ui->cdp_031->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CDP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CDP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据32
    msg = ui->cdp_032->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CDP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CDP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据33
    msg = ui->cdp_033->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CDP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CDP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据34
    msg = ui->cdp_034->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CDP数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("CDP数据不能为非数字"));
        return;
    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    instruct.append("0000000000");
    /**********************/
    char sum= 0;
    for(char i : instruct.mid(2,instruct.size() - 2))
    {
        sum += i;
    }
    instruct.append(sum);
    emit giveMsgToClientSocketToGo(instruct);
}


void sendDataWidget::cpi_Instruct()
{
    qDebug() <<"CPI  MSG";
    QByteArray instruct;
    instruct.append(QByteArray::fromHex(instructHead));
    //帧标识
    instruct.append(QByteArray::fromHex("0E"));
    //帧长度 //需修改
    short length = 20;
    length = BLEndianShort(length);
    QByteArray lengthByte = ShortToByte(length);
    instruct.append(lengthByte);
    //飞机编号
    instruct.append(planeNumber);
    //时间
    QTime time = QDateTime::currentDateTime().time();
    char hour =  time.hour();
    instruct.append(hour);
    char min = time.minute();
    instruct.append(min);
    char second = time.second();
    instruct.append(second);

    QDate date = QDate::currentDate();
    int Int_year = date.year() - 2000;
    char year = (char)Int_year;
    char month = date.month();
    char day = date.day();

    instruct.append(year);
    instruct.append(month);
    instruct.append(day);
    instruct.append(hour);
    instruct.append(min);
    instruct.append(second);

    char sum= 0;
    for(char i : instruct.mid(2,instruct.size() - 2))
    {
        sum += i;
    }
    instruct.append(sum);
    emit giveMsgToClientSocketToGo(instruct);
}


void sendDataWidget::twoD_S_Instruct()
{
    qDebug() <<"2D_S  MSG";
    QByteArray instruct;
    instruct.append(QByteArray::fromHex(instructHead));
    //帧标识
    instruct.append(QByteArray::fromHex("0f"));
    //帧长度 //需修改
    short length = 16;
    length = BLEndianShort(length);
    QByteArray lengthByte = ShortToByte(length);
    instruct.append(lengthByte);
    //飞机编号
    instruct.append(planeNumber);
    //时间
    QTime time = QDateTime::currentDateTime().time();
    char hour =  time.hour();
    instruct.append(hour);
    char min = time.minute();
    instruct.append(min);
    char second = time.second();
    instruct.append(second);
    //数据1
    QString msg = ui->d_s->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("2D-S数据不能为空"));
        return;
    }
    if(!isDigitStr(msg)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("2D-S数据不能为非数字"));
        return;
    }
    short msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));

    char sum= 0;
    for(char i : instruct.mid(2,instruct.size() - 2))
    {
        sum += i;
    }
    instruct.append(sum);
    emit giveMsgToClientSocketToGo(instruct);
}




//这个可能不需要重等位，需要和控制端联合调一下
void sendDataWidget::planeStatusInstruct()
{
    qDebug() <<"PLANE_STATUS  MSG";
    QByteArray instruct;
    instruct.append(QByteArray::fromHex(instructHead));
    //帧标识
    instruct.append(QByteArray::fromHex("13"));
    //帧长度 //需修改
    short length = 19;
    length = BLEndianShort(length);
    QByteArray lengthByte = ShortToByte(length);
    instruct.append(lengthByte);
    //飞机编号
    instruct.append(planeNumber);
    //时间
    QTime time = QDateTime::currentDateTime().time();
    char hour =  time.hour();
    instruct.append(hour);
    char min = time.minute();
    instruct.append(min);
    char second = time.second();
    instruct.append(second);

    QBitArray systemStatus;   //子系统状态
    systemStatus.resize(16);
    if(ui->status_27->isChecked()){
        systemStatus.setBit(0,true);
    }
    if(ui->status_1->isChecked()){
        systemStatus.setBit(1,true);
    }
    if(ui->status_2->isChecked()){
        systemStatus.setBit(2,true);
    }
    if(ui->status_3->isChecked()){
        systemStatus.setBit(3,true);
    }
    if(ui->status_4->isChecked()){
        systemStatus.setBit(4,true);
    }
    if(ui->status_5->isChecked()){
        systemStatus.setBit(5,true);
    }
    if(ui->status_6->isChecked()){
        systemStatus.setBit(6,true);
    }
    if(ui->status_7->isChecked()){
        systemStatus.setBit(7,true);
    }
    if(ui->status_8->isChecked()){
        systemStatus.setBit(8,true);
    }
    if(ui->status_9->isChecked()){
        systemStatus.setBit(9,true);
    }
    instruct.append(BLEndiansByteVersion(bitsToBytes(systemStatus)));

    QBitArray airStatus; //大气系统状态
    airStatus.resize(8);
    if(ui->status_28->isChecked()){
        airStatus.setBit(0,true);
    }
    if(ui->status_10->isChecked()){
        airStatus.setBit(1,true);
    }
    if(ui->status_11->isChecked()){
        airStatus.setBit(2,true);
    }
    if(ui->status_12->isChecked()){
        airStatus.setBit(3,true);
    }
    if(ui->status_13->isChecked()){
        airStatus.setBit(4,true);
    }
    if(ui->status_14->isChecked()){
        airStatus.setBit(5,true);
    }
    if(ui->status_15->isChecked()){
        airStatus.setBit(6,true);
    }
    instruct.append(bitsToBytes(airStatus));

    QBitArray catalyzeStatus;  //催化子系统
    catalyzeStatus.resize(8);
    if(ui->status_29->isChecked()){
        catalyzeStatus.setBit(0,true);
    }
    if(ui->status_16->isChecked()){
        catalyzeStatus.setBit(1,true);
    }
    if(ui->status_17->isChecked()){
        catalyzeStatus.setBit(2,true);
    }
    if(ui->status_18->isChecked()){
        catalyzeStatus.setBit(3,true);
    }
    if(ui->status_19->isChecked()){
        catalyzeStatus.setBit(4,true);
    }
    if(ui->status_20->isChecked()){
        catalyzeStatus.setBit(5,true);
    }
    if(ui->status_21->isChecked()){
        catalyzeStatus.setBit(6,true);
    }
    if(ui->status_22->isChecked()){
        catalyzeStatus.setBit(7,true);
    }
    instruct.append(bitsToBytes(catalyzeStatus));
    //空地通信系统状态
    QBitArray skyGroundstatus;
    skyGroundstatus.resize(8);
    if(ui->status_30->isChecked()){
        skyGroundstatus.setBit(0,true);
    }
    if(ui->status_23->isChecked()){
        skyGroundstatus.setBit(1,true);
    }
    if(ui->status_24->isChecked()){
        skyGroundstatus.setBit(2,true);
    }
    if(ui->status_25->isChecked()){
        skyGroundstatus.setBit(3,true);
    }
    if(ui->status_26->isChecked()){
        skyGroundstatus.setBit(4,true);
    }
    instruct.append(bitsToBytes(skyGroundstatus));

    char sum= 0;
    for(char i : instruct.mid(2,instruct.size() - 2))
    {
        sum += i;
    }
    instruct.append(sum);
    emit giveMsgToClientSocketToGo(instruct);

}



//稍后再去做，解决了位的问题之后再去弄
void sendDataWidget::planeLocationInstruct()
{
    qDebug() <<"PLANE LOCATION MSG";
    QByteArray instruct;
    instruct.append(QByteArray::fromHex(instructHead));
    //帧标识
    instruct.append(QByteArray::fromHex("12"));
    //帧长度  33
    short length = 33;
    length = BLEndianShort(length);
    QByteArray lengthByte = ShortToByte(length);
    instruct.append(lengthByte);
    //飞机编号
    instruct.append(planeNumber);
    //时间
    QTime time = QDateTime::currentDateTime().time();
    char hour =  time.hour();
    instruct.append(hour);
    char min = time.minute();
    instruct.append(min);
    char second = time.second();
    instruct.append(second);

    QBitArray latitude1;   //纬度
    latitude1.resize(24);
    //latitude1.clear();
    for(int i = 0 ;i<=20; i++) {
        latitude1.setBit(i,true);
    }
    QByteArray ba = bitsToBytes(latitude1);
    instruct.append(ba);

    QBitArray longitude;
    longitude.resize(24);
    //longitude.clear();
    for(int i = 0 ;i<=20; i++) {
        longitude.setBit(i,true);
    }
    instruct.append(bitsToBytes(longitude));

    QBitArray height;
    height.resize(24);
    for(int i = 0;i<=20; i++){
        height.setBit(i,true);
    }
    instruct.append(bitsToBytes(height));
    //数据1
    QString msg = ui->plane_location->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("飞机位置数据不能为空"));
        return;
    }
    //    if(!isDigitStr(msg)) {
    //        QMessageBox::information(NULL, "Title", QStringLiteral("2D-S数据不能为非数字"));
    //        return;
    //    }
    short msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据2
    msg = ui->plane_location_2->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("飞机位置数据不能为空"));
        return;
    }
    //    if(!isDigitStr(msg)) {
    //        QMessageBox::information(NULL, "Title", QStringLiteral("2D-S数据不能为非数字"));
    //        return;
    //    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据3
    msg = ui->plane_location_3->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("飞机位置数据不能为空"));
        return;
    }
    //    if(!isDigitStr(msg)) {
    //        QMessageBox::information(NULL, "Title", QStringLiteral("2D-S数据不能为非数字"));
    //        return;
    //    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据4
    msg = ui->plane_location_4->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("飞机位置数据不能为空"));
        return;
    }
    //    if(!isDigitStr(msg)) {
    //        QMessageBox::information(NULL, "Title", QStringLiteral("2D-S数据不能为非数字"));
    //        return;
    //    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));
    //数据5
    msg = ui->plane_location_5->text();
    if(msg.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("飞机位置数据不能为空"));
        return;
    }
    //    if(!isDigitStr(msg)) {
    //        QMessageBox::information(NULL, "Title", QStringLiteral("2D-S数据不能为非数字"));
    //        return;
    //    }
    msg_Short = msg.toShort();
    msg_Short = BLEndianShort(msg_Short);
    instruct.append(ShortToByte(msg_Short));


    /*******************************/
    char sum= 0;
    for(char i : instruct.mid(2,instruct.size() - 2))
    {
        sum += i;
    }
    instruct.append(sum);
    emit giveMsgToClientSocketToGo(instruct);
}

QByteArray sendDataWidget::bitsToBytes(QBitArray bits) {     
    QByteArray bytes;
    bytes.resize(bits.count()/8);
    bytes.fill(0);
    // Convert from QBitArray to QByteArray
    for(int b=0; b<bits.count(); ++b)
        bytes[b/8] = ( bytes.at(b/8) | ((bits[b]?1:0)<<(b%8)));
    return bytes;
}



//EN-DCCI-28
void sendDataWidget::beidouWorkInfoMsgInstruct(){
    qDebug() <<"BEIDOU WORKINFO MSG";
    QByteArray instruct;
    instruct.append(QByteArray::fromHex(instructHead));
    //帧标识
    instruct.append(QByteArray::fromHex("1c"));
    //帧长度  需改动
    short length = 45;
    length = BLEndianShort(length);
    QByteArray lenghByte = ShortToByte(length);
    instruct.append(lenghByte);
    //序号
    instruct.append(ShortToByte(BLEndianShort(beidouInstructNumber)));
    beidouInstructNumber ++;
    //飞机编号   //之后需要用统一配置那里的
    instruct.append(planeNumber.mid(1,4));


    QBitArray timeBits;   //时间
    timeBits.resize(16);
    QTime time =QTime::currentTime();
    int hour = time.hour();
    int min = time.minute();
    int second = time.second();
    if(hour > 12)
        hour = hour - 12;
    for(int i = 12 ;i <=15 ;i++){
        if(hour%2 == 1) {
            timeBits.setBit(i,true);
        } else
            timeBits.setBit(i,false);
        hour = hour/2;
    }
    for( int i = 6;i <=11; i++) {
        if(min%2 == 1) {
            timeBits.setBit(i,true);
        } else
            timeBits.setBit(i,false);
        min = min/2;
    }
    for( int i = 0;i <=6; i++) {
        if(second%2 == 1) {
            timeBits.setBit(i,true);
        } else
            timeBits.setBit(i,false);
        second = second/2;
    }
    QByteArray timeByte = bitsToBytes(timeBits);
    //填了以后大小端换一下
    timeByte = BLEndiansByteVersion(timeByte);
    instruct.append(timeByte);

    //经度
    QBitArray longitudeBits;
    longitudeBits.resize(32);

    if(ui->beidouWorkInfo_9->isChecked()) {
        longitudeBits.setBit(31,true);
    }
    QString lgtdStr = ui->beidouWorkInfo_0->text();
    if(lgtdStr.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("北斗作业信息数据不能为空"));
        return;
    }
    if(!isDigitStr(lgtdStr)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("北斗作业信息数据不能为非数字"));
        return;
    }
    float lgtdMum = lgtdStr.toFloat() * 100000;
    int  lgtInt = (int)lgtdMum;

    for(int i = 0 ;i <=30 ;i++){
        if(lgtInt%2 == 1) {
            longitudeBits.setBit(i,true);
        } else
            longitudeBits.setBit(i,false);
        lgtInt = lgtInt/2;
    }
    QByteArray lgByte = bitsToBytes(longitudeBits);
    //填了以后大小端换一下
    lgByte = BLEndiansByteVersion(lgByte);
    instruct.append(lgByte);

    //纬度
    QBitArray latitudeBits;
    latitudeBits.resize(32);

    if(ui->beidouWorkInfo_10->isChecked()) {
        latitudeBits.setBit(31,true);
    }
    QString latdStr = ui->beidouWorkInfo_1->text();
    if(latdStr.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("北斗作业信息数据不能为空"));
        return;
    }
    if(!isDigitStr(latdStr)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("北斗作业信息数据不能为非数字"));
        return;
    }
    float latdMum = latdStr.toFloat() * 100000;
    int  latdInt = (int)latdMum;

    for(int i = 0 ;i <=30 ;i++){
        if(latdInt%2 == 1) {
            latitudeBits.setBit(i,true);
        } else
            latitudeBits.setBit(i,false);
        latdInt = latdInt/2;
    }
    QByteArray laByte = bitsToBytes(latitudeBits);
    //填了以后大小端换一下
    laByte = BLEndiansByteVersion(laByte);
    instruct.append(laByte);

    //海拔高度
    QString workStr = ui->beidouWorkInfo_2->text();
    if(workStr.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("北斗作业信息数据不能为空"));
        return;
    }
    if(!isDigitStr(workStr)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("北斗作业信息数据不能为非数字"));
        return;
    }
    short heightShort =  workStr.toShort();
    heightShort = BLEndianShort(heightShort);
    instruct.append(ShortToByte(heightShort));

    //航速
    workStr = ui->beidouWorkInfo_3->text();
    if(workStr.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("北斗作业信息数据不能为空"));
        return;
    }
    if(!isDigitStr(workStr)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("北斗作业信息数据不能为非数字"));
        return;
    }
    ushort speed_flow =  workStr.toUShort();
    speed_flow = BLEndianShort(speed_flow);
    instruct.append(ShortToByte(speed_flow));

    //磁航向
    workStr = ui->beidouWorkInfo_4->text();
    if(workStr.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("北斗作业信息数据不能为空"));
        return;
    }
    if(!isDigitStr(workStr)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("北斗作业信息数据不能为非数字"));
        return;
    }
    ushort magnetism_flow =  workStr.toUShort();
    magnetism_flow = BLEndianShort(magnetism_flow);
    instruct.append(ShortToByte(magnetism_flow));

    //temperature  温度
    workStr = ui->beidouWorkInfo_5->text();
    if(workStr.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("北斗作业信息数据不能为空"));
        return;
    }
    if(!isDigitStr(workStr)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("北斗作业信息数据不能为非数字"));
        return;
    }
    ushort temperature =  workStr.toUShort();
    temperature = BLEndianShort(temperature);
    instruct.append(ShortToByte(temperature));


    //湿度
    workStr = ui->beidouWorkInfo_6->text();
    if(workStr.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("北斗作业信息数据不能为空"));
        return;
    }
    if(!isDigitStr(workStr)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("北斗作业信息数据不能为非数字"));
        return;
    }
    char humidity = (char)workStr.toShort();
    instruct.append(humidity);

    //焰条
    QBitArray fireSlip;
    fireSlip.resize(24);
    int  factoryNum = ui->weaponComboBox_1->currentIndex();
    for(int i = 19;i <=23 ;i++) {
        if(factoryNum%2 == 1) {
            fireSlip.setBit(i,true);
        } else
            fireSlip.setBit(i,false);
        factoryNum = factoryNum/2;
    }
    int bulletType = ui->bulletTypeCombobox_1->currentIndex();
    for(int i = 14;i <=18 ;i++) {
        if(bulletType%2 == 1) {
            fireSlip.setBit(i,true);
        } else
            fireSlip.setBit(i,false);
        bulletType = bulletType/2;
    }
    QString howManyNum = ui->howManyBullet_1->text();
    if(howManyNum.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("北斗作业信息数据不能为空"));
        return;
    }
    if(!isDigitStr(howManyNum)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("北斗作业信息数据不能为非数字"));
        return;
    }
    int howManyInt = howManyNum.toInt();
    for(int i = 0;i <= 13 ;i++) {
        if(howManyInt%2 == 1) {
            fireSlip.setBit(i,true);
        } else
            fireSlip.setBit(i,false);
        howManyInt = howManyInt/2;
    }
    QByteArray bulletByte = bitsToBytes(fireSlip);
    bulletByte = BLEndiansByteVersion(bulletByte);
    instruct.append(bulletByte);

    //焰弹
    QBitArray fireBullet;
    fireBullet.resize(24);
    int  factoryNum_2 = ui->weaponComboBox_2->currentIndex();
    for(int i = 19;i <=23 ;i++) {
        if(factoryNum_2%2 == 1) {
            fireBullet.setBit(i,true);
        } else
            fireBullet.setBit(i,false);
        factoryNum_2 = factoryNum_2/2;
    }
    int bulletType_2 = ui->bulletTypeCombobox_2->currentIndex();
    for(int i = 14;i <=18 ;i++) {
        if(bulletType_2%2 == 1) {
            fireBullet.setBit(i,true);
        } else
            fireBullet.setBit(i,false);
        bulletType_2 = bulletType_2/2;
    }
    QString howManyNum_2 = ui->howManyBullet_2->text();
    if(howManyNum_2.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("北斗作业信息数据不能为空"));
        return;
    }
    if(!isDigitStr(howManyNum_2)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("北斗作业信息数据不能为非数字"));
        return;
    }
    int howManyInt_2 = howManyNum_2.toInt();
    for(int i = 0;i <= 13 ;i++) {
        if(howManyInt_2%2 == 1) {
            fireBullet.setBit(i,true);
        } else
            fireBullet.setBit(i,false);
        howManyInt_2 = howManyInt_2/2;
    }
    QByteArray bulletByte_2 = bitsToBytes(fireBullet);
    bulletByte_2 = BLEndiansByteVersion(bulletByte_2);
    instruct.append(bulletByte_2);

    //液氮
    QBitArray veryColdBits;
    veryColdBits.resize(24);
    int  factoryNum_3 = ui->weaponComboBox_3->currentIndex();
    for(int i = 19;i <=23 ;i++) {
        if(factoryNum_3%2 == 1) {
            veryColdBits.setBit(i,true);
        } else
            veryColdBits.setBit(i,false);
        factoryNum_3 = factoryNum_3/2;
    }
    int bulletType_3 = ui->bulletTypeCombobox_3->currentIndex();
    for(int i = 14;i <=18 ;i++) {
        if(bulletType_3%2 == 1) {
            veryColdBits.setBit(i,true);
        } else
            veryColdBits.setBit(i,false);
        bulletType_3 = bulletType_3/2;
    }
    QString howManyNum_3 = ui->howManyBullet_3->text();
    if(howManyNum_3.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("北斗作业信息数据不能为空"));
        return;
    }
    if(!isDigitStr(howManyNum_3)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("北斗作业信息数据不能为非数字"));
        return;
    }
    int howManyInt_3 = howManyNum_3.toInt();
    for(int i = 0;i <= 13 ;i++) {
        if(howManyInt_3%2 == 1) {
            veryColdBits.setBit(i,true);
        } else
            veryColdBits.setBit(i,false);
        howManyInt_3 = howManyInt_3/2;
    }
    QByteArray bulletByte_3 = bitsToBytes(veryColdBits);
    bulletByte_3 = BLEndiansByteVersion(bulletByte_3);
    instruct.append(bulletByte_3);


    //粉剂
    QBitArray powderBits;
    powderBits.resize(24);
    int  factoryNum_4 = ui->weaponComboBox_4->currentIndex();
    for(int i = 19;i <=23 ;i++) {
        if(factoryNum_4%2 == 1) {
            powderBits.setBit(i,true);
        } else
            powderBits.setBit(i,false);
        factoryNum_4 = factoryNum_4/2;
    }
    int bulletType_4 = ui->bulletTypeCombobox_4->currentIndex();
    for(int i = 14;i <=18 ;i++) {
        if(bulletType_4%2 == 1) {
            powderBits.setBit(i,true);
        } else
            powderBits.setBit(i,false);
        bulletType_4 = bulletType_4/2;
    }
    QString howManyNum_4 = ui->howManyBullet_4->text();
    if(howManyNum_4.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("北斗作业信息数据不能为空"));
        return;
    }
    if(!isDigitStr(howManyNum_4)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("北斗作业信息数据不能为非数字"));
        return;
    }
    int howManyInt_4 = howManyNum_4.toInt();
    for(int i = 0;i <= 13 ;i++) {
        if(howManyInt_4%2 == 1) {
            powderBits.setBit(i,true);
        } else
            powderBits.setBit(i,false);
        howManyInt_4 = howManyInt_4/2;
    }
    QByteArray bulletByte_4 = bitsToBytes(powderBits);
    bulletByte_3 = BLEndiansByteVersion(bulletByte_4);
    instruct.append(bulletByte_4);

    //LWC
    QString LWC = ui->beidouWorkInfo_7->text();
    if(LWC.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("北斗作业信息数据不能为空"));
        return;
    }
    if(!isDigitStr(LWC)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("北斗作业信息数据不能为非数字"));
        return;
    }
    short lwcShort =  LWC.toShort();
    lwcShort = BLEndianShort(lwcShort);
    instruct.append(ShortToByte(lwcShort));

    char sum= 0;
    for(char i : instruct.mid(2,instruct.size() - 2))
    {
        sum += i;
    }
    instruct.append(sum);
    emit giveMsgToClientSocketToGo(instruct);
}







//海事 作业信息
void sendDataWidget::haishiWorkInfoMsgInstruct()
{
    qDebug() <<"HAISHI WORKINFO MSG";
    QByteArray homeworkinstruct;
    homeworkinstruct.append(QByteArray::fromHex(instructHead));
    //帧标识
    homeworkinstruct.append(QByteArray::fromHex("11"));
    //序号 1- 99999循环， 转换一下
    homeworkinstruct.append("00001");
    //符号
    homeworkinstruct.append(",");
    // 飞机编号
    homeworkinstruct.append(planeNumber);
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
    QString str = ui->haishi_workinfo_0->text();
    if(str.size() >= 9) {
        str = str.mid(0,9);
    } else {
        while(str.size() != 9)
            str = str + "0";
    }
    homeworkinstruct.append(str);
    homeworkinstruct.append(",");
    str = ui->haishi_workinfo_1->text();
    if(str.size() >= 8) {
        str = str.mid(0,8);
    } else {
        while(str.size() != 8)
            str = str + "0";
    }
    homeworkinstruct.append(str);
    //高度  5位前面不够 0 来凑
    homeworkinstruct.append(",");
    str = ui->haishi_workinfo_2->text();
    if(str.size() >= 5) {
        str = str.mid(0,5);
    } else {
        while(str.size() != 5)
            str = str.prepend('0');
    }
    homeworkinstruct.append(str);
    //航速 3位  不够0来凑
    homeworkinstruct.append(",");
    str = ui->haishi_workinfo_3->text();
    if(str.size() >= 3) {
        str = str.mid(0,3);
    } else {
        while(str.size() != 3)
            str = str.prepend('0');
    }
    homeworkinstruct.append(str);
    homeworkinstruct.append(",");
    //航向 3位
    str = ui->haishi_workinfo_4->text();
    if(str.size() >= 3) {
        str = str.mid(0,3);
    } else {
        while(str.size() != 3)
            str = str.prepend('0');
    }
    homeworkinstruct.append(str);
    homeworkinstruct.append(",");
    //温度 5 位  不够0来凑
    str = ui->haishi_workinfo_5->text();
    if(str.size() >= 5) {
        str = str.mid(0,5);
    } else {
        while(str.size() != 5)
            str = str.append('0');
    }
    homeworkinstruct.append(str);
    //湿度 3位  不够0凑
    homeworkinstruct.append(",");
    str = ui->haishi_workinfo_6->text();
    if(str.size() >= 3) {
        str = str.mid(0,3);
    } else {
        while(str.size() != 3)
            str = str.prepend('0');
    }
    homeworkinstruct.append(str);
    homeworkinstruct.append(",");
    //判断勾选是否正确
    if(!ui->bullet_checkBox_0->isChecked()) {
        ui->bullet_checkBox_1->setChecked(false);
        //ui->bullet_checkBox_1->setCheckable(false);
    } /*else {
        ui->bullet_checkBox_1->setCheckable(true);
    }*/
    if(!ui->bullet_checkBox_1->isChecked()) {
        ui->bullet_checkBox_2->setChecked(false);
    } /*else {
        ui->bullet_checkBox_2->setCheckable(true);
    }*/

    if(!ui->bullet_checkBox_2->isChecked()) {
        ui->bullet_checkBox_3->setChecked(false);
    }  /*else {
        ui->bullet_checkBox_3->setCheckable(true);
    }*/

    //焰条 11位
    if(ui->bullet_checkBox_0->isChecked()) {
        short factory = ui->bullet_comboBox_0->currentIndex();
        QString factoryNumber = QString::number(factory);
        if(factory < 10)
            factoryNumber.prepend("0");
        homeworkinstruct.append(factoryNumber);
        homeworkinstruct.append("2");
        if(ui->bulletTypeBox_0->currentIndex() == 0){
            //冷云
            homeworkinstruct.append("1");
            //short bulletType = 5;  //弹药样式
            //homeworkinstruct.append(ShortToByte(BLEndianShort(bulletType)));
            homeworkinstruct.append("05");
            //bulletType = 1;        //弹药型号
            //homeworkinstruct.append(ShortToByte(BLEndianShort(bulletType)));
            homeworkinstruct.append("01");

        } else {
            //暖云
            homeworkinstruct.append("3");
            //short bulletType = 5;   //弹药样式
            //homeworkinstruct.append(ShortToByte(BLEndianShort(bulletType)));
            homeworkinstruct.append("05");
            //bulletType = 5;         //弹药型号
            //homeworkinstruct.append(ShortToByte(BLEndianShort(bulletType)));
            homeworkinstruct.append("05");
        }
        // 不用这种方法，用字符串的方法
        //        int volume = ui->workinfo_1->text().toInt();
        //        QBitArray howManyBullet;
        //        howManyBullet.resize(24);
        //        for(int i = 0 ;i <=23 ;i++){
        //            if(volume%2 == 1) {
        //                howManyBullet.setBit(i,true);
        //            } else
        //                howManyBullet.setBit(i,false);
        //            volume = volume/2;
        //        }
        //        homeworkinstruct.append(bitsToBytes(howManyBullet));
        QString volume = ui->workinfo_1->text();
        if(volume.size()>=3) {
            homeworkinstruct.append(volume.mid(0,3));
        } else {
            while(volume.size() != 3){
                volume.prepend("0");
            }
            homeworkinstruct.append(volume);
        }

        if(ui->bullet_checkBox_1->isChecked() || ui->bullet_checkBox_2->isChecked() || ui->bullet_checkBox_3->isChecked())
            homeworkinstruct.append(",");
    }

    //焰弹 11位
    if(ui->bullet_checkBox_1->isChecked()) {
        short factory = ui->bullet_comboBox_1->currentIndex();
        QString factoryNumber = QString::number(factory);
        if(factory < 10)
            factoryNumber.prepend("0");
        homeworkinstruct.append(factoryNumber);
        homeworkinstruct.append("2");
        if(ui->bulletTypeBox_1->currentIndex() == 0){
            //冷云
            //催化剂种类
            homeworkinstruct.append("1");
            //short bulletType = 6;  //弹药样式
            //homeworkinstruct.append(ShortToByte(BLEndianShort(bulletType)));
            homeworkinstruct.append("06");
            //bulletType = 2;        //弹药型号
            //homeworkinstruct.append(ShortToByte(BLEndianShort(bulletType)));
            homeworkinstruct.append("02");

        } else {
            //暖云
            //催化剂种类
            homeworkinstruct.append("3");
            //short bulletType = 6;   //弹药样式
            // homeworkinstruct.append(ShortToByte(BLEndianShort(bulletType)));
            homeworkinstruct.append("06");
            //bulletType = 6;         //弹药型号
            //homeworkinstruct.append(ShortToByte(BLEndianShort(bulletType)));
            homeworkinstruct.append("06");
        }
        //        int volume = ui->workinfo_2->text().toInt();
        //        QBitArray howManyBullet;
        //        howManyBullet.resize(24);
        //        for(int i = 0 ;i <=23 ;i++){
        //            if(volume%2 == 1) {
        //                howManyBullet.setBit(i,true);
        //            } else
        //                howManyBullet.setBit(i,false);
        //            volume = volume/2;
        //        }
        //        homeworkinstruct.append(bitsToBytes(howManyBullet));

        QString volume = ui->workinfo_2->text();
        if(volume.size()>=3) {
            homeworkinstruct.append(volume.mid(0,3));
        } else {
            while(volume.size() != 3){
                volume.prepend("0");
            }
            homeworkinstruct.append(volume);
        }

        if(ui->bullet_checkBox_2->isChecked() || ui->bullet_checkBox_3->isChecked())
            homeworkinstruct.append(",");
    }

    //液氮 11位
    if(ui->bullet_checkBox_2->isChecked()) {
        short factory = ui->bullet_comboBox_2->currentIndex();
        QString factoryNumber = QString::number(factory);
        if(factory < 10)
            factoryNumber.prepend("0");
        homeworkinstruct.append(factoryNumber);
        homeworkinstruct.append("2");

        //液氮
        //催化剂种类
        homeworkinstruct.append("2");
        //short bulletType = 0;  //弹药样式
        //homeworkinstruct.append(ShortToByte(BLEndianShort(bulletType)));
        homeworkinstruct.append("00");
        //bulletType = 3;        //弹药型号
        //homeworkinstruct.append(ShortToByte(BLEndianShort(bulletType)));
        homeworkinstruct.append("03");

        //        int volume = ui->workinfo_3->text().toInt();
        //        QBitArray howManyBullet;
        //        howManyBullet.resize(24);
        //        for(int i = 0 ;i <=23 ;i++){
        //            if(volume%2 == 1) {
        //                howManyBullet.setBit(i,true);
        //            } else
        //                howManyBullet.setBit(i,false);
        //            volume = volume/2;
        //        }
        //        homeworkinstruct.append(bitsToBytes(howManyBullet));
        QString volume = ui->workinfo_3->text();
        if(volume.size()>=3) {
            homeworkinstruct.append(volume.mid(0,3));
        } else {
            while(volume.size() != 3){
                volume.prepend("0");
            }
            homeworkinstruct.append(volume);
        }

        if(ui->bullet_checkBox_3->isChecked())
            homeworkinstruct.append(",");
    }


    //粉剂 11位
    if(ui->bullet_checkBox_3->isChecked()) {
        short factory = ui->bullet_comboBox_3->currentIndex();
        QString factoryNumber = QString::number(factory);
        if(factory < 10)
            factoryNumber.prepend("0");
        homeworkinstruct.append(factoryNumber);
        homeworkinstruct.append("2");

        //粉剂
        //催化剂种类
        homeworkinstruct.append("3");
        //short bulletType = 0;  //弹药样式
        //homeworkinstruct.append(ShortToByte(BLEndianShort(bulletType)));
        homeworkinstruct.append("00");
        //bulletType = 4;        //弹药型号
        //homeworkinstruct.append(ShortToByte(BLEndianShort(bulletType)));
        homeworkinstruct.append("04");

        //        int volume = ui->workinfo_4->text().toInt();
        //        QBitArray howManyBullet;
        //        howManyBullet.resize(24);
        //        for(int i = 0 ;i <=23 ;i++){
        //            if(volume%2 == 1) {
        //                howManyBullet.setBit(i,true);
        //            } else
        //                howManyBullet.setBit(i,false);
        //            volume = volume/2;
        //        }
        //        homeworkinstruct.append(bitsToBytes(howManyBullet));

        QString volume = ui->workinfo_4->text();
        if(volume.size()>=3) {
            homeworkinstruct.append(volume.mid(0,3));
        } else {
            while(volume.size() != 3){
                volume.prepend("0");
            }
            homeworkinstruct.append(volume);
        }
    }

    //帧长度
    short length = homeworkinstruct.size() + 3;
    length = BLEndianShort(length);
    QByteArray lenghByte = ShortToByte(length);
    homeworkinstruct.insert(3,lenghByte);

    char sum= 0;
    for(char i : homeworkinstruct.mid(2,homeworkinstruct.size() - 2))
    {
        sum += i;
    }
    homeworkinstruct.append(sum);
    emit giveMsgToClientSocketToGo(homeworkinstruct);
}

QString sendDataWidget::giveCurrentDate()
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

QString sendDataWidget::giveCurrentTime()
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

void sendDataWidget::EmitBeidouInstruct()
{
    bool p1,p2;
    p1 = ui->beidouOpenOrNot->isChecked();
    p2 = ui->lujingOpenOrNot->isChecked();
    emit emitBeiDouStatus(p1,p2);
}


void sendDataWidget::changeRate(char num)
{

    switch (num) {
    case 0x01:
        circle_timer->setInterval(1000);
        break;
    case 0x0A:
        circle_timer->setInterval(10000);
        break;
    case 0x3C:
        circle_timer->setInterval(60000);
        break;
    }
    qDebug()<< QStringLiteral("已更改周期指令速率");
    circle_timer->start();
}



//short 转换大小端
short sendDataWidget::BLEndianShort(short value)
{
    return ((value & 0x00FF) << 8 ) | ((value & 0xFF00) >> 8);
}

//转换大小端
QByteArray sendDataWidget::BLEndiansByteVersion(QByteArray value)
{
    QByteArray ba;
    for(auto i :value) {
        ba.prepend(i);
    }
    return ba;
}

//转换大小端
uint sendDataWidget::BLEndianInt(uint x)
{
    return (((uint)(x) & 0xff000000) >> 24) |  (((uint)(x) & 0x00ff0000) >> 8) | \
            (((uint)(x) & 0x0000ff00) << 8) | (((uint)(x) & 0x000000ff) << 24);
}


void sendDataWidget::controlBeidouWorkInfo(bool open){

    if(open == true) {
        if(!beidouWorkInfo_timer->isActive()) {
            beidouWorkInfo_timer->start();
        }
    } else {
        if(beidouWorkInfo_timer->isActive()) {
            beidouWorkInfo_timer->stop();
        }
    }
}


