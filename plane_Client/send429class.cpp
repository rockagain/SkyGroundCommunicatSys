#include "send429class.h"
#include "ui_send429class.h"

#define CARDNO		0


#define TXCHL		1
#define TXCHL_1     0

#define RXCHL		0


send429class::send429class(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::send429class)
{
    ui->setupUi(this);
    init();
}






send429class::~send429class()
{
    delete ui;
}


void send429class::init429board(){

    status = Usb7103_Init(CARDNO,&hUsb7103);

    if(status != 0)
    {
        QMessageBox::information(NULL, "Title", QStringLiteral("429 初始化失败!\n\n请调整设备再次尝试"));
        return;   //可以先注释了进行下面的功能
    }
    QMessageBox::information(NULL, "Title", QStringLiteral("429 初始化已成功！"));
    int resetOk  =  Usb7103_Reset(hUsb7103);
}


void send429class::manipulateGNSS_HIGHT_MSL()
{
    QBitArray gnss; //大气系统状态
    gnss.resize(32);
    gnss.setBit(0,false);
    gnss.setBit(1,false);
    gnss.setBit(2,true);
    gnss.setBit(3,true);
    gnss.setBit(4,true);
    gnss.setBit(5,true);
    gnss.setBit(6,true);
    gnss.setBit(7,false);
    QString gnssNum = ui->gnss_num->text();
    if(gnssNum.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("GNSS高度数据不能为空"));
        return;
    }
    if(!isDigitStr(gnssNum)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("GNSS高度数据不能为非数字"));
        return;
    }
    //这种算法，输入和保存的结果是一致的
    int remain = (gnssNum.toInt())*8;
    for(int i = 8 ;i<= 27;i++){
        if(remain%2 == 1) {
            gnss.setBit(i,true);
        } else
            gnss.setBit(i,false);
        remain = remain/2;
    }

    if(ui->gnss_checkbox->isChecked()){
        //为负
        //取反
        for(int i = 8;i <= 27; i++){
            if(gnss.at(i)){
                gnss.setBit(i,false);
            } else {
                gnss.setBit(i,true);
            }
        }
        //加一  //该方式待验证
        for(int i = 8; i<=27 ;i++) {
            if(gnss.at(i) == false) {
                gnss.setBit(i,true);
                break;
            } else {
                gnss.setBit(i,false);
            }
        }
        gnss.setBit(28,true);
    } else {
        //unchecked  为正
        gnss.setBit(28,false);
    }
    //LSB状态位  MSB状态位
    gnss.setBit(29,true);
    gnss.setBit(30,true);
    //奇校验
    gnss.setBit(31,true);
    QByteArray instruct =  bitsToBytes(gnss);
    //hight_msl_instruct =  instruct.toULong();
    //memcpy(bar.data(),&num,sizeof(num));
    memcpy(&hight_msl_instruct,instruct,4);
    int i = Usb7103_TransFifo(hUsb7103, GPS_SEND_PASSAGE,hight_msl_instruct);
}

void send429class::manipulateHDOP()
{
    QBitArray hdop; //大气系统状态
    hdop.resize(32);
    //if(ui->status_10->isChecked()){
    // airStatus.setBit(0,true);
    // }
    hdop.setBit(0,false);
    hdop.setBit(1,true);
    hdop.setBit(2,false);
    hdop.setBit(3,false);
    hdop.setBit(4,false);
    hdop.setBit(5,false);
    hdop.setBit(6,false);
    hdop.setBit(7,true);
    //SDI
    if(ui->hdop_01->isChecked()) {
        hdop.setBit(8,true);
    } else {
        hdop.setBit(8,false);
    }
    if(ui->hdop_02->isChecked()) {
        hdop.setBit(9,true);
    } else {
        hdop.setBit(9,false);
    }
    hdop.setBit(9,true);
    //
    hdop.setBit(10,false);
    hdop.setBit(11,false);
    hdop.setBit(12,false);
    //15位bit
    QString hdopNum = ui->hdop_num->text();
    if(hdopNum.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("HDOP数据不能为空"));
        return;
    }
    if(!isDigitStr(hdopNum)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("HDOP数据不能为非数字"));
        return;
    }
    //这种算法，输入和保存的结果是一致的
    int remain = (hdopNum.toInt())/0.03125;
    for(int i = 13 ;i<= 27;i++){
        if(remain%2 == 1) {
            hdop.setBit(i,true);
        } else
            hdop.setBit(i,false);
        remain = remain/2;
    }
    //固定为正
    hdop.setBit(28,0);
    //LSB状态位  MSB状态位
    hdop.setBit(29,true);
    hdop.setBit(30,true);
    //奇校验
    hdop.setBit(31,true);
    QByteArray instruct =  bitsToBytes(hdop);
    memcpy(&hdop_instruct,instruct,4);
    Usb7103_TransFifo(hUsb7103, GPS_SEND_PASSAGE,hdop_instruct);
}


void send429class::manipulateVDOP()
{
    QBitArray vdop; //大气系统状态
    vdop.resize(32);
    //if(ui->status_10->isChecked()){
    // airStatus.setBit(0,true);
    // }
    vdop.setBit(0,false);
    vdop.setBit(1,true);
    vdop.setBit(2,false);
    vdop.setBit(3,false);
    vdop.setBit(4,false);
    vdop.setBit(5,false);
    vdop.setBit(6,true);
    vdop.setBit(7,false);
    //SDI
    if(ui->vdop_01->isChecked()) {
        vdop.setBit(8,true);
    } else {
        vdop.setBit(8,false);
    }
    if(ui->vdop_02->isChecked()) {
        vdop.setBit(9,true);
    } else {
        vdop.setBit(9,false);
    }
    //
    vdop.setBit(10,false);
    vdop.setBit(11,false);
    vdop.setBit(12,false);
    //15位bit
    QString hdopNum = ui->hdop_num->text();
    if(hdopNum.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("HDOP数据不能为空"));
        return;
    }
    if(!isDigitStr(hdopNum)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("HDOP数据不能为非数字"));
        return;
    }
    //这种算法，输入和保存的结果是一致的
    int remain = (hdopNum.toInt())/0.03125;
    for(int i = 13 ;i<= 27;i++){
        if(remain%2 == 1) {
            vdop.setBit(i,true);
        } else
            vdop.setBit(i,false);
        remain = remain/2;
    }
    //固定为正
    vdop.setBit(28,0);
    //LSB状态位  MSB状态位
    vdop.setBit(29,true);
    vdop.setBit(30,true);
    //奇校验
    vdop.setBit(31,true);
    QByteArray instruct =  bitsToBytes(vdop);
    memcpy(&vdop_instruct,instruct,4);
    Usb7103_TransFifo(hUsb7103, GPS_SEND_PASSAGE,vdop_instruct);
}


void send429class::manipulateGNSS_TRACK_ANGLE()
{
    QBitArray angle; //大气系统状态
    angle.resize(32);
    //if(ui->status_10->isChecked()){
    // airStatus.setBit(0,true);
    // }
    angle.setBit(0,false);
    angle.setBit(1,true);
    angle.setBit(2,false);
    angle.setBit(3,false);
    angle.setBit(4,false);
    angle.setBit(5,false);
    angle.setBit(6,true);
    angle.setBit(7,true);
    //SDI
    if(ui->angle_01->isChecked()) {
        angle.setBit(8,true);
    } else {
        angle.setBit(8,false);
    }
    if(ui->angle_02->isChecked()) {
        angle.setBit(9,true);
    } else {
        angle.setBit(9,false);
    }
    //
    angle.setBit(10,false);
    angle.setBit(11,false);
    angle.setBit(12,false);
    QString angleNum = ui->angle_num->text();
    if(angleNum.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("真航迹角数据不能为空"));
        return;
    }
    if(!isDigitStr(angleNum)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("真航迹角数据不能为非数字"));
        return;
    }
    //这种算法，输入和保存的结果是一致的
    int remain = (angleNum.toInt())/0.0055;
    for(int i = 13 ;i<= 27;i++){
        if(remain%2 == 1) {
            angle.setBit(i,true);
        } else
            angle.setBit(i,false);
        remain = remain/2;
    }

    if(ui->angle_checkbox->isChecked()){
        //为负
        //取反
        for(int i = 13;i <= 27; i++){
            if(angle.at(i)){
                angle.setBit(i,false);
            } else {
                angle.setBit(i,true);
            }
        }
        //加一  //该方式待验证
        for(int i = 13; i<=27 ;i++) {
            if(angle.at(i) == false) {
                angle.setBit(i,true);
                break;
            } else {
                angle.setBit(i,false);
            }
        }
        angle.setBit(28,true);
    } else {
        //unchecked  为正
        angle.setBit(28,false);
    }
    //LSB状态位  MSB状态位
    angle.setBit(29,true);
    angle.setBit(30,true);
    //奇校验
    angle.setBit(31,true);
    QByteArray instruct =  bitsToBytes(angle);
    memcpy(&track_angle_instruct,instruct,4);
    Usb7103_TransFifo(hUsb7103, GPS_SEND_PASSAGE,track_angle_instruct);
}

// 经度
void send429class::manipulateGNSS_LONGITUDE()
{
    QBitArray longitude; //大气系统状态
    longitude.resize(32);
    //if(ui->status_10->isChecked()){
    // airStatus.setBit(0,true);
    // }
    longitude.setBit(0,false);
    longitude.setBit(1,true);
    longitude.setBit(2,false);
    longitude.setBit(3,false);
    longitude.setBit(4,true);
    longitude.setBit(5,false);
    longitude.setBit(6,false);
    longitude.setBit(7,true);
    //
    QString longitudeNum = ui->longitude_num->text();
    if(longitudeNum.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("经度数据不能为空"));
        return;
    }
    if(!isDigitStr(longitudeNum)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("经度数据不能为非数字"));
        return;
    }
    //这种算法，输入和保存的结果是一致的
    int remain = (longitudeNum.toInt())/0.00000095;
    for(int i = 8 ;i<= 27;i++){
        if(remain%2 == 1) {
            longitude.setBit(i,true);
        } else
            longitude.setBit(i,false);
        remain = remain/2;
    }

    if(ui->longitude_checkbox->isChecked()){
        //为负
        //取反
        for(int i = 8;i <= 27; i++){
            if(longitude.at(i)){
                longitude.setBit(i,false);
            } else {
                longitude.setBit(i,true);
            }
        }
        //加一  //该方式待验证
        for(int i = 8; i<=27 ;i++) {
            if(longitude.at(i) == false) {
                longitude.setBit(i,true);
                break;
            } else {
                longitude.setBit(i,false);
            }
        }
        longitude.setBit(28,true);
    } else {
        //unchecked  为正
        longitude.setBit(28,false);
    }
    //LSB状态位  MSB状态位
    longitude.setBit(29,true);
    longitude.setBit(30,true);
    //奇校验
    longitude.setBit(31,true);
    QByteArray instruct =  bitsToBytes(longitude);
    memcpy(&longitude_instruct,instruct,4);
    Usb7103_TransFifo(hUsb7103, GPS_SEND_PASSAGE,longitude_instruct);
}

//纬度
void send429class::manipulateGNSS_LATITUDE()
{
    QBitArray latitude; //大气系统状态
    latitude.resize(32);
    //if(ui->status_10->isChecked()){
    // airStatus.setBit(0,true);
    // }
    latitude.setBit(0,false);
    latitude.setBit(1,true);
    latitude.setBit(2,false);
    latitude.setBit(3,false);
    latitude.setBit(4,true);
    latitude.setBit(5,false);
    latitude.setBit(6,false);
    latitude.setBit(7,false);
    //
    QString latitudeNum = ui->latitude_num->text();
    if(latitudeNum.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("纬度数据不能为空"));
        return;
    }
    if(!isDigitStr(latitudeNum)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("纬度数据不能为非数字"));
        return;
    }
    //这种算法，输入和保存的结果是一致的
    int remain = (latitudeNum.toInt())/0.00000095;
    for(int i = 8 ;i<= 27;i++){
        if(remain%2 == 1) {
            latitude.setBit(i,true);
        } else
            latitude.setBit(i,false);
        remain = remain/2;
    }

    if(ui->latitude_checkbox->isChecked()){
        //为负
        //取反
        for(int i = 8;i <= 27; i++){
            if(latitude.at(i)){
                latitude.setBit(i,false);
            } else {
                latitude.setBit(i,true);
            }
        }
        //加一  //该方式待验证
        for(int i = 8; i<=27 ;i++) {
            if(latitude.at(i) == false) {
                latitude.setBit(i,true);
                break;
            } else {
                latitude.setBit(i,false);
            }
        }
        latitude.setBit(28,true);
    } else {
        //unchecked  为正
        latitude.setBit(28,false);
    }
    //LSB状态位  MSB状态位
    latitude.setBit(29,true);
    latitude.setBit(30,true);
    //奇校验
    latitude.setBit(31,true);
    QByteArray instruct =  bitsToBytes(latitude);
    memcpy(&latitude_instruct,instruct,4);
    Usb7103_TransFifo(hUsb7103, GPS_SEND_PASSAGE,latitude_instruct);
}




void send429class::manipulate_GROUND_SPEED()
{
    QBitArray speed; //大气系统状态
    speed.resize(32);
    //if(ui->status_10->isChecked()){
    // airStatus.setBit(0,true);
    // }
    speed.setBit(0,false);
    speed.setBit(1,true);
    speed.setBit(2,false);
    speed.setBit(3,false);
    speed.setBit(4,true);
    speed.setBit(5,false);
    speed.setBit(6,true);
    speed.setBit(7,false);
    //
    speed.setBit(8,false);
    speed.setBit(9,false);
    speed.setBit(10,false);
    speed.setBit(11,false);
    speed.setBit(12,false);


    QString speedNum = ui->speed_num->text();
    if(speedNum.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("地速数据不能为空"));
        return;
    }
    if(!isDigitStr(speedNum)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("地速数据不能为非数字"));
        return;
    }
    //这种算法，输入和保存的结果是一致的
    int remain = (speedNum.toInt())/0.125;
    for(int i = 13 ;i<= 27;i++){
        if(remain%2 == 1) {
            speed.setBit(i,true);
        } else
            speed.setBit(i,false);
        remain = remain/2;
    }


    speed.setBit(28,true);

    //LSB状态位  MSB状态位
    speed.setBit(29,true);
    speed.setBit(30,true);
    //奇校验
    speed.setBit(31,true);
    QByteArray instruct =  bitsToBytes(speed);
    memcpy(&speed_instruct,instruct,4);
    Usb7103_TransFifo(hUsb7103, GPS_SEND_PASSAGE,speed_instruct);
}


void send429class::initParameter()
{
    ui->gnss_num->setText("131072");
    ui->hdop_num->setText("1024");
    ui->vdop_num->setText("1024");
    ui->angle_num->setText("180");
    ui->latitude_num->setText("90");
    ui->longitude_num->setText("180");
    ui->speed_num->setText("4096");
    ui->fine_latitude_num->setText("90");
    ui->fine_longitude_num->setText("180");
    ui->sensor_8->setText("15");
    ui->sensor_9->setText("15");
    ui->sensor_10->setText("00000");
    ui->gnss_height_num->setText("131072");
    ui->flowstyle_0->setText("360");
    ui->flowstyle_1->setText("180");
    ui->flowstyle_2->setText("180");
    ui->flowstyle_3->setText("128");
    ui->flowstyle_4->setText("128");
    ui->cuihua_max->setText("03");


    QStringList baudList;//波特率
    baudList<<"1200"<<"1800"<<"2400"<<"4800"<<"9600"
           <<"14400"<<"19200"<<"38400"<<"56000"<<"57600"
          <<"76800"<<"115200";
    QStringList dataBitsList;
    dataBitsList<<"5"<<"6"<<"7"<<"8";
    ui->cmbBaudRate->addItems(baudList);
    ui->cmbBaudRate->setCurrentIndex(11);
    ui->cmbDataBits->addItems(dataBitsList);
    ui->cmbDataBits->setCurrentIndex(3);
    QStringList gpsList;
    gpsList<<"12K5"<<"100K"<<"48K";
    ui->gpsbitrate->addItems(gpsList);
    ui->stylebitrate->addItems(gpsList);
    QStringList oddOrEven;
    oddOrEven<<QStringLiteral("奇")<<QStringLiteral("偶")<<QStringLiteral("无");
    ui->stylecheckBit->addItems(oddOrEven);
    ui->gpscheckBit->addItems(oddOrEven);
    ui->GPS_port->setText("0");
    ui->PLANE_port->setText("0");
    ui->sendPort_1->setText("0");
    ui->sendPort_2->setText("1");
    //
    QStringList boshaWhatList;
    boshaWhatList << QStringLiteral("焰条(冷云") <<QStringLiteral("焰弹(冷)") << QStringLiteral("液氮")
                  << QStringLiteral("粉剂") << QStringLiteral("焰条(暖云)") << QStringLiteral("焰弹(暖云)");
    ui->comboBox_422_type->addItems(boshaWhatList);
    ui->open422orNot->setChecked(true);

}

//精纬度
void send429class::manipulateGNSS_FINE_LATITUDE()
{
    QBitArray latitude; //大气系统状态
    latitude.resize(32);
    //if(ui->status_10->isChecked()){
    // airStatus.setBit(0,true);
    // }
    latitude.setBit(0,false);
    latitude.setBit(1,true);
    latitude.setBit(2,false);
    latitude.setBit(3,true);
    latitude.setBit(4,false);
    latitude.setBit(5,false);
    latitude.setBit(6,false);
    latitude.setBit(7,false);
    //
    latitude.setBit(8,false);
    latitude.setBit(9,false);
    latitude.setBit(10,false);
    latitude.setBit(11,false);
    latitude.setBit(12,false);
    latitude.setBit(13,false);
    latitude.setBit(14,false);
    latitude.setBit(15,false);
    latitude.setBit(16,false);



    QString latitudeNum = ui->fine_latitude_num->text();
    if(latitudeNum.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("精纬度数据不能为空"));
        return;
    }
    if(!isDigitStr(latitudeNum)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("精纬度数据不能为非数字"));
        return;
    }
    //这种算法，输入和保存的结果是一致的
    int remain =((latitudeNum.toInt())/0.0000000001)/4.7;

    for(int i = 17 ;i<= 27;i++){
        if(remain%2 == 1) {
            latitude.setBit(i,true);
        } else
            latitude.setBit(i,false);
        remain = remain/2;
    }
    //固定为零
    latitude.setBit(28,false);
    //LSB状态位  MSB状态位
    latitude.setBit(29,true);
    latitude.setBit(30,true);
    //奇校验
    latitude.setBit(31,true);
    QByteArray instruct =  bitsToBytes(latitude);

    memcpy(&fine_latitude_instruct,instruct,4);
    Usb7103_TransFifo(hUsb7103, GPS_SEND_PASSAGE ,fine_latitude_instruct);
}

void send429class::init()
{
    //ui->close_gps_pushButton->setHidden(true);
    //ui->close_plane_pushButton->setHidden(true);
    //把端口相关的隐藏了
    ui->GPS_port->setHidden(true);
    ui->PLANE_port->setHidden(true);
    ui->label_23->setHidden(true);
    ui->label_22->setHidden(true);


    status = -1;  //-1
    //status_1 = -1;  //-1
    ui->gps_pushButton->setCheckable(true);
    ui->plane_pushButton->setCheckable(true);
    ui->start_pushButton->setStyleSheet(
                "QPushButton:pressed{background-color:#FFAA33}"
                "QPushButton:checked{background-color:#FFAA33}"
                );
    ui->init_planeStyle_Button->setStyleSheet(
                "QPushButton:pressed{background-color:#FFAA33}"
                "QPushButton:checked{background-color:#FFAA33}"
                );
    ui->plane_pushButton->setStyleSheet(
                "QPushButton:pressed{background-color:#FFAA33}"
                "QPushButton:checked{background-color:#FFAA33}"
                );
    ui->gps_pushButton->setStyleSheet(
                "QPushButton:pressed{background-color:#FFAA33}"
                "QPushButton:checked{background-color:#FFAA33}"
                );
    ui->work_PushButton->setStyleSheet(
                "QPushButton:pressed{background-color:#FFAA33}"
                "QPushButton:checked{background-color:#FFAA33}"
                );
    //设置间隔都为零 ，待测试是否可以成功
    gps_update_time = 200;
    plane_style_time = 19;
    QHBoxLayout *Layout = new QHBoxLayout;
    Layout->setContentsMargins(0,0,0,0);
    this->setLayout(Layout);
    Layout->addWidget(ui->tabWidget);
    //控件内的参数
    initParameter();
    gpsTimer = new QTimer;
    planeStyleTimer = new QTimer;
    gpsTimer->setInterval(gps_update_time);   //5Hz
    planeStyleTimer->setInterval(plane_style_time);
    //
    m_serialPort = new QSerialPort();


    connect(ui->start_pushButton,&QPushButton::clicked,this,&send429class::configGPS_Hardware);
    connect(gpsTimer,&QTimer::timeout,this,&send429class::sendGpsMsg);
    connect(ui->plane_pushButton,&QPushButton::clicked,this,&send429class::Send_PlaneStyle_Msg);
    connect(planeStyleTimer,&QTimer::timeout,this,&send429class::sendPlaneMsg);
    connect(ui->gps_pushButton,&QPushButton::clicked,this,&send429class::Send_Gps_Msg);
    connect(ui->work_PushButton,&QPushButton::clicked,this,&send429class::Send_422_Msg);
    connect(ui->close_gps_pushButton,&QPushButton::clicked,ui->gps_pushButton,&QPushButton::click);
    connect(ui->close_plane_pushButton,&QPushButton::clicked,ui->plane_pushButton,&QPushButton::click);
    connect(ui->init_422_Button,&QPushButton::clicked,this,&send429class::init_422_port);
    connect(ui->init_planeStyle_Button,&QPushButton::clicked,this,&send429class::configPlaneStyle_Hardware);
}

void send429class::configPlaneStyle_Hardware()
{

    int style_bitrate,stylecheck;

    //    if(status != 0){
    //        status = Usb7103_Init(CARDNO,&hUsb7103);
    //        int resetOk  =  Usb7103_Reset(hUsb7103);
    //    }

    int port = ui->PLANE_port->text().toInt();
    if(ui->stylebitrate->currentText() == "12K5") {
        style_bitrate = USB7103_BITRATE_12K5;
    } else if (ui->stylebitrate->currentText() == "100K") {
        style_bitrate = USB7103_BITRATE_100K;
    } if (ui->stylebitrate->currentText() == "48K") {
        style_bitrate = USB7103_BITRATE_48K;
    }

    if(ui->gpscheckBit->currentText() == QStringLiteral("奇")) {
        stylecheck = USB7103_PARITY_ODD;
    } else if (ui->gpscheckBit->currentText() == QStringLiteral("偶")) {
        stylecheck = USB7103_PARITY_EVEN;
    } if (ui->gpscheckBit->currentText() == QStringLiteral("无")) {
        stylecheck = USB7103_PARITY_NONE;
    }


    if(status != 0)
    {
        QMessageBox::information(NULL, "Title", QStringLiteral("航姿端口初始化失败!\n\n请调整设备再次尝试"));
        return;   //可以先注释了进行下面的功能
        //status = 0;   //为了调试把status设为0
    }
    QMessageBox::information(NULL, "Title", QStringLiteral("航姿配置成功 !\n\n可以发送GPS以及航姿信息"));
    //
    Usb7103_TxConfig (hUsb7103, PLANE_STYLE_PORT, style_bitrate, stylecheck);

}


void send429class::init_422_port(){
    //在串口中选择你要用的422串口在下面使用
    QStringList m_serialPortName;
    foreach(const QSerialPortInfo &info,QSerialPortInfo::availablePorts())
    {
        m_serialPortName << info.portName();
        qDebug()<<"serialPortName:"<<info.portName();
    }
    if(m_serialPortName.size() == 0) {
        QMessageBox::information(NULL, "Title", QStringLiteral("没有可用的422串口"));
        ui->combox_422_portName->addItem("无可用串口，请调试设备");
        return;
    }
    ui->combox_422_portName->addItems(m_serialPortName);
    ui->combox_422_portName->setCurrentIndex(0);

}


void send429class::Send_422_Msg(){




    //实例化串口类一个对象
    //m_serialPort->setPortName(m_serialPortName[0]);
    m_serialPort->setPortName(ui->combox_422_portName->currentText());

    if(m_serialPort->isOpen())//如果串口已经打开了 先给他关闭了
    {
        m_serialPort->clear();
        m_serialPort->close();
    }


    if(!m_serialPort->open(QIODevice::ReadWrite))//用ReadWrite 的模式尝试打开串口
    {
        QMessageBox::information(NULL, "Title", QStringLiteral("422串口没有打开"));
        return;
    }
    //QMessageBox::information(NULL, "Title", QStringLiteral("串口打开"));
    int baudnum = ui->cmbBaudRate->currentText().toInt();
    int databits = ui->cmbDataBits->currentText().toInt();
    m_serialPort->setBaudRate(baudnum,QSerialPort::AllDirections);//设置波特率和读写方向
    if(databits == 5)
        m_serialPort->setDataBits(QSerialPort::Data5);		//数据位为5位
    if(databits == 6)
        m_serialPort->setDataBits(QSerialPort::Data6);		//数据位为5位
    if(databits == 7)
        m_serialPort->setDataBits(QSerialPort::Data7);		//数据位为5位
    if(databits == 8)
        m_serialPort->setDataBits(QSerialPort::Data8);		//数据位为5位

    m_serialPort->setFlowControl(QSerialPort::NoFlowControl);//无流控制
    m_serialPort->setParity(QSerialPort::NoParity);	//无校验位
    m_serialPort->setStopBits(QSerialPort::OneStop); //一位停止位
    QByteArray sendArray = broadcastInfo_422();
    m_serialPort->write(sendArray);
}


void send429class::Send_Gps_Msg()
{
    if(status != 0) {
        ui->gps_pushButton->setChecked(false);
        return;
    }
    //假如开始就关闭，假如关闭就开启
    if(gpsTimer->isActive()) {
        gpsTimer->stop();
        qDebug()<<"CLOSE GPS MSG";
        Usb7103_TxStop (hUsb7103, GPS_SEND_PASSAGE);
        //Usb7103_Reset (hUsb7103);
        //Usb7103_Close (hUsb7103);
    } else {
        Usb7103_TxStart (hUsb7103, GPS_SEND_PASSAGE);
        gpsTimer->start();
        qDebug()<<"SEND GPS MSG";
    }
}


void send429class::configGPS_Hardware()
{

    //初始化USB7103板卡
    qDebug()<<"config  GPS MSG";

    //设置发送通道模式
    int gps_bitrate;
    if(ui->gpsbitrate->currentText() == "12K5") {
        gps_bitrate = USB7103_BITRATE_12K5;
    } else if (ui->gpsbitrate->currentText() == "100K") {
        gps_bitrate = USB7103_BITRATE_100K;
    } if (ui->gpsbitrate->currentText() == "48K") {
        gps_bitrate = USB7103_BITRATE_48K;
    }

    int gpscheck;
    if(ui->gpscheckBit->currentText() == QStringLiteral("奇")) {
        gpscheck = USB7103_PARITY_ODD;
    } else if (ui->gpscheckBit->currentText() == QStringLiteral("偶")) {
        gpscheck = USB7103_PARITY_EVEN;
    } if (ui->gpscheckBit->currentText() == QStringLiteral("无")) {
        gpscheck = USB7103_PARITY_NONE;
    }

    this->GPS_SEND_PASSAGE = ui->sendPort_1->text().toInt();

    //    if(status != 0)
    //    {
    //        status = Usb7103_Init(CARDNO,&hUsb7103);
    //        int resetOk  =  Usb7103_Reset(hUsb7103);
    //    }

    if(status != 0)
    {
        QMessageBox::information(NULL, "Title", QStringLiteral("端口初始化失败!\n\n请调整设备再次尝试"));
        return;   //可以先注释了进行下面的功能
        //status = 0;   //为了调试把status设为0
    }


    //
    int configOk =  Usb7103_TxConfig (hUsb7103, GPS_SEND_PASSAGE, gps_bitrate, gpscheck);
    //Usb7103_ClearMsg(hUsb7103, GPS_SEND_PASSAGE);


}

//发送 GPS 信息
void send429class::sendGpsMsg()
{

    manipulateGNSS_HIGHT_MSL();
    manipulateHDOP();
    manipulateVDOP();
    manipulateGNSS_TRACK_ANGLE();
    manipulateGNSS_LATITUDE();
    manipulateGNSS_LONGITUDE();
    manipulate_GROUND_SPEED();
    manipulateGNSS_FINE_LATITUDE();
    manipulateGNSS_FINE_LONGITUDE();
    manipulateUTC_DATE();
    manipulateUTC_TIME();
    manipulateGNSS_SENSOR_STATUS();
    manipulateGNSS_HIGHT();
}

void send429class::Send_PlaneStyle_Msg()
{
        if(status != 0) {
            ui->plane_pushButton->setChecked(false);
            return;
        }
    //假如开始就关闭，假如关闭就开启
    if(planeStyleTimer->isActive()) {
        planeStyleTimer->stop();
        qDebug()<<"CLOSE PLANE MSG";
        //Usb7103_TxStop (hUsb7103, TXCHL_1);
        //Usb7103_Reset (hUsb7103);
        //Usb7103_Close (hUsb7103);
        Usb7103_TxStop(hUsb7103, PLANE_STYLE_PORT);
    } else {

        PLANE_STYLE_PORT = ui->sendPort_2->text().toInt();
        Usb7103_TxStart(hUsb7103, PLANE_STYLE_PORT);
        planeStyleTimer->start();
        qDebug()<<"SEND PLANE MSG";
    }

}

void send429class::sendPlaneMsg()
{
    plane_flowstyle_0();
    plane_flowstyle_1();
    plane_flowstyle_2();
    plane_flowstyle_3();
    plane_flowstyle_4();

}




//精经度
void send429class::manipulateGNSS_FINE_LONGITUDE()
{
    QBitArray longitude; //大气系统状态
    longitude.resize(32);
    longitude.setBit(0,false);
    longitude.setBit(1,true);
    longitude.setBit(2,false);
    longitude.setBit(3,true);
    longitude.setBit(4,false);
    longitude.setBit(5,false);
    longitude.setBit(6,false);
    longitude.setBit(7,true);
    //
    longitude.setBit(8,false);
    longitude.setBit(9,false);
    longitude.setBit(10,false);
    longitude.setBit(11,false);
    longitude.setBit(12,false);
    longitude.setBit(13,false);
    longitude.setBit(14,false);
    longitude.setBit(15,false);
    longitude.setBit(16,false);

    QString longitudeNum = ui->fine_longitude_num->text();
    if(longitudeNum.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("精经度数据不能为空"));
        return;
    }
    if(!isDigitStr(longitudeNum)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("精经度数据不能为非数字"));
        return;
    }
    //这种算法，输入和保存的结果是一致的
    int remain =((longitudeNum.toInt())/0.0000000001)/4.7;

    for(int i = 17 ;i<= 27;i++){
        if(remain%2 == 1) {
            longitude.setBit(i,true);
        } else
            longitude.setBit(i,false);
        remain = remain/2;
    }
    //固定为零
    longitude.setBit(28,false);
    //LSB状态位  MSB状态位
    longitude.setBit(29,true);
    longitude.setBit(30,true);
    //奇校验
    longitude.setBit(31,true);
    QByteArray instruct =  bitsToBytes(longitude);
    memcpy(&fine_longitude_instruct,instruct,4);
    Usb7103_TransFifo(hUsb7103, GPS_SEND_PASSAGE,fine_longitude_instruct);
}


void send429class::manipulateUTC_TIME()
{
    QBitArray time;
    time.resize(32);
    time.setBit(0,false);
    time.setBit(1,true);
    time.setBit(2,true);
    time.setBit(3,false);
    time.setBit(4,true);
    time.setBit(5,false);
    time.setBit(6,false);
    time.setBit(7,false);
    //
    if(ui->time_01->isChecked()){
        time.setBit(8,true);
    } else {
        time.setBit(8,false);
    }

    if(ui->time_02->isChecked()) {
        time.setBit(9,true);
    } else {
        time.setBit(9,false);
    }
    QTime system_time = QDateTime::currentDateTime().time();
    int second = system_time.second();
    second = second/10;
    QString analysis = return_4Bit(second);
    if(analysis.at(0) == '0') {
        time.setBit(10,0);
    } else {
        time.setBit(10,1);
    }
    if(analysis.at(1) == '0') {
        time.setBit(11,0);
    } else {
        time.setBit(11,1);
    }
    if(analysis.at(2) == '0') {
        time.setBit(12,0);
    } else {
        time.setBit(12,1);
    }
    if(analysis.at(3) == '0') {
        time.setBit(13,0);
    } else {
        time.setBit(13,1);
    }

    int min = system_time.minute();
    int one = min%10;
    int ten = min/10;
    analysis = return_4Bit(one);
    if(analysis.at(0) == '0') {
        time.setBit(14,0);
    } else {
        time.setBit(14,1);
    }
    if(analysis.at(1) == '0') {
        time.setBit(15,0);
    } else {
        time.setBit(15,1);
    }
    if(analysis.at(2) == '0') {
        time.setBit(16,0);
    } else {
        time.setBit(16,1);
    }
    if(analysis.at(3) == '0') {
        time.setBit(17,0);
    } else {
        time.setBit(17,1);
    }
    analysis = return_4Bit(ten);
    if(analysis.at(0) == '0') {
        time.setBit(18,0);
    } else {
        time.setBit(18,1);
    }
    if(analysis.at(1) == '0') {
        time.setBit(19,0);
    } else {
        time.setBit(19,1);
    }
    if(analysis.at(2) == '0') {
        time.setBit(20,0);
    } else {
        time.setBit(20,1);
    }
    if(analysis.at(3) == '0') {
        time.setBit(21,0);
    } else {
        time.setBit(21,1);
    }
    int hour = system_time.hour();
    one = hour%10;
    ten = hour/10;
    analysis = return_4Bit(one);
    if(analysis.at(0) == '0') {
        time.setBit(22,0);
    } else {
        time.setBit(22,1);
    }
    if(analysis.at(1) == '0') {
        time.setBit(23,0);
    } else {
        time.setBit(23,1);
    }
    if(analysis.at(2) == '0') {
        time.setBit(24,0);
    } else {
        time.setBit(24,1);
    }
    if(analysis.at(3) == '0') {
        time.setBit(25,0);
    } else {
        time.setBit(25,1);
    }
    analysis = return_4Bit(ten);
    if(analysis.at(0) == '0') {
        time.setBit(26,0);
    } else {
        time.setBit(26,1);
    }
    if(analysis.at(1) == '0') {
        time.setBit(27,0);
    } else {
        time.setBit(27,1);
    }
    if(analysis.at(2) == '0') {
        time.setBit(28,0);
    } else {
        time.setBit(28,1);
    }
    //后三位
    time.setBit(29,1);
    time.setBit(30,1);
    time.setBit(31,1);
    QByteArray instruct =  bitsToBytes(time);
    memcpy(&time_instruct,instruct,4);
    Usb7103_TransFifo(hUsb7103, GPS_SEND_PASSAGE,time_instruct);
}


void send429class::manipulateUTC_DATE()
{
    QBitArray date;
    date.resize(32);
    date.setBit(0,true);
    date.setBit(1,false);
    date.setBit(2,true);
    date.setBit(3,true);
    date.setBit(4,false);
    date.setBit(5,false);
    date.setBit(6,false);
    date.setBit(7,false);
    //
    if(ui->date_01->isChecked()){
        date.setBit(8,true);
    } else {
        date.setBit(8,false);
    }

    if(ui->date_02->isChecked()) {
        date.setBit(9,true);
    } else {
        date.setBit(9,false);
    }
    QDate system_date = QDate::currentDate();

    int year = system_date.year();
    year = year%2000;
    int one = year%10;
    int ten = year/10;
    QString analysis = return_4Bit(one);
    if(analysis.at(0) == '0') {
        date.setBit(10,0);
    } else {
        date.setBit(10,1);
    }
    if(analysis.at(1) == '0') {
        date.setBit(11,0);
    } else {
        date.setBit(11,1);
    }
    if(analysis.at(2) == '0') {
        date.setBit(12,0);
    } else {
        date.setBit(12,1);
    }
    if(analysis.at(3) == '0') {
        date.setBit(13,0);
    } else {
        date.setBit(13,1);
    }
    analysis = return_4Bit(ten);
    if(analysis.at(0) == '0') {
        date.setBit(14,0);
    } else {
        date.setBit(14,1);
    }
    if(analysis.at(1) == '0') {
        date.setBit(15,0);
    } else {
        date.setBit(15,1);
    }
    if(analysis.at(2) == '0') {
        date.setBit(16,0);
    } else {
        date.setBit(16,1);
    }
    if(analysis.at(3) == '0') {
        date.setBit(17,0);
    } else {
        date.setBit(17,1);
    }

    int month = system_date.month();
    one = month%10;
    ten = month/10;
    analysis = return_4Bit(one);
    if(analysis.at(0) == '0') {
        date.setBit(18,0);
    } else {
        date.setBit(18,1);
    }
    if(analysis.at(1) == '0') {
        date.setBit(19,0);
    } else {
        date.setBit(19,1);
    }
    if(analysis.at(2) == '0') {
        date.setBit(20,0);
    } else {
        date.setBit(20,1);
    }
    if(analysis.at(3) == '0') {
        date.setBit(21,0);
    } else {
        date.setBit(21,1);
    }
    analysis = return_4Bit(ten);
    if(analysis.at(0) == '0') {
        date.setBit(22,0);
    } else {
        date.setBit(22,1);
    }

    int day = system_date.day();
    one = day%10;
    ten = day/10;
    analysis = return_4Bit(one);
    if(analysis.at(0) == '0') {
        date.setBit(23,0);
    } else {
        date.setBit(23,1);
    }
    if(analysis.at(1) == '0') {
        date.setBit(24,0);
    } else {
        date.setBit(24,1);
    }
    if(analysis.at(2) == '0') {
        date.setBit(25,0);
    } else {
        date.setBit(25,1);
    }
    if(analysis.at(3) == '0') {
        date.setBit(26,0);
    } else {
        date.setBit(26,1);
    }
    analysis = return_4Bit(ten);
    if(analysis.at(0) == '0') {
        date.setBit(27,0);
    } else {
        date.setBit(27,1);
    }
    if(analysis.at(1) == '0') {
        date.setBit(28,0);
    } else {
        date.setBit(28,1);
    }
    //后三位
    date.setBit(29,1);
    date.setBit(30,1);
    date.setBit(31,1);
    QByteArray instruct =  bitsToBytes(date);
    memcpy(&date_instruct,instruct,4);
    Usb7103_TransFifo(hUsb7103, GPS_SEND_PASSAGE,date_instruct);
}

void send429class::manipulateGNSS_SENSOR_STATUS()
{
    QBitArray status;
    status.resize(32);
    status.setBit(0,true);
    status.setBit(1,false);
    status.setBit(2,true);
    status.setBit(3,true);
    status.setBit(4,true);
    status.setBit(5,false);
    status.setBit(6,true);
    status.setBit(7,true);
    //
    if(ui->sensor_01->isChecked()){
        status.setBit(8,true);
    } else {
        status.setBit(8,false);
    }
    if(ui->sensor_02->isChecked()) {
        status.setBit(9,true);
    } else {
        status.setBit(9,false);
    }
    if(ui->sensor_3->isChecked()) {
        status.setBit(10,true);
    } else {
        status.setBit(10,false);
    }
    if(ui->sensor_4->isChecked()) {
        status.setBit(11,true);
    } else {
        status.setBit(11,false);
    }
    if(ui->sensor_5->isChecked()) {
        status.setBit(12,true);
    } else {
        status.setBit(12,false);
    }
    if(ui->sensor_6->isChecked()) {
        status.setBit(13,true);
    } else {
        status.setBit(13,false);
    }
    if(ui->sensor_7->isChecked()) {
        status.setBit(14,true);
    } else {
        status.setBit(14,false);
    }
    QString sensorNum = ui->sensor_8->text();
    if(sensorNum.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("传感器数据不能为空"));
        return;
    }
    if(!isDigitStr(sensorNum)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("传感器数据不能为非数字"));
        return;
    }
    //这种算法，输入和保存的结果是一致的
    int remain = sensorNum.toInt();
    for(int i = 15 ;i<= 18;i++){
        if(remain%2 == 1) {
            status.setBit(i,true);
        } else
            status.setBit(i,false);
        remain = remain/2;
    }
    //
    sensorNum = ui->sensor_9->text();
    if(sensorNum.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("传感器数据不能为空"));
        return;
    }
    if(!isDigitStr(sensorNum)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("传感器数据不能为非数字"));
        return;
    }
    //这种算法，输入和保存的结果是一致的
    remain = sensorNum.toInt();
    for(int i = 19 ;i<= 22;i++){
        if(remain%2 == 1) {
            status.setBit(i,true);
        } else
            status.setBit(i,false);
        remain = remain/2;
    }
    //
    sensorNum = ui->sensor_10->text();
    if(sensorNum.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("传感器数据不能为空"));
        return;
    }
    if(!isDigitStr(sensorNum)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("传感器数据不能为非数字"));
        return;
    }
    //这种算法，输入和保存的结果是一致的
    remain = sensorNum.toInt();
    QString analysis = sensorNum;
    if(analysis.at(0) == '0') {
        status.setBit(23,false);
    } else {
        status.setBit(23,true);
    }
    if(analysis.at(1) == '0') {
        status.setBit(24,false);
    } else {
        status.setBit(24,true);
    }

    if(analysis.at(2) == '0') {
        status.setBit(25,false);
    } else {
        status.setBit(25,true);
    }

    if(analysis.at(3) == '0') {
        status.setBit(26,false);
    } else {
        status.setBit(26,true);
    }

    if(analysis.at(4) == '0') {
        status.setBit(27,false);
    } else {
        status.setBit(27,true);
    }
    if(ui->sensor_11->isChecked()) {
        status.setBit(28,true);
    } else {
        status.setBit(28,false);
    }
    //后三位
    status.setBit(29,1);
    status.setBit(30,1);
    status.setBit(31,1);
    QByteArray instruct =  bitsToBytes(status);
    memcpy(&sensor_status_instruct,instruct,4);
    Usb7103_TransFifo(hUsb7103, GPS_SEND_PASSAGE,sensor_status_instruct);
}

void send429class::manipulateGNSS_HIGHT()
{
    QBitArray gnss; //大气系统状态
    gnss.resize(32);

    gnss.setBit(0,true);
    gnss.setBit(1,true);
    gnss.setBit(2,true);
    gnss.setBit(3,true);
    gnss.setBit(4,true);
    gnss.setBit(5,false);
    gnss.setBit(6,false);
    gnss.setBit(7,false);
    QString gnssNum = ui->gnss_height_num->text();
    if(gnssNum.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("HIGHT数据不能为空"));
        return;
    }
    if(!isDigitStr(gnssNum)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("HIGHT数据不能为非数字"));
        return;
    }
    //这种算法，输入和保存的结果是一致的
    int remain = (gnssNum.toInt())*8;
    for(int i = 8 ;i<= 27;i++){
        if(remain%2 == 1) {
            gnss.setBit(i,true);
        } else
            gnss.setBit(i,false);
        remain = remain/2;
    }

    if(ui->gnss_height_checkbox->isChecked()){
        //为负
        //取反
        for(int i = 8;i <= 27; i++){
            if(gnss.at(i)){
                gnss.setBit(i,false);
            } else {
                gnss.setBit(i,true);
            }
        }
        //加一  //该方式待验证
        for(int i = 8; i<=27 ;i++) {
            if(gnss.at(i) == false) {
                gnss.setBit(i,true);
                break;
            } else {
                gnss.setBit(i,false);
            }
        }
        gnss.setBit(28,true);
    } else {
        //unchecked  为正
        gnss.setBit(28,false);
    }
    //LSB状态位  MSB状态位
    gnss.setBit(29,true);
    gnss.setBit(30,true);
    //奇校验
    gnss.setBit(31,true);
    QByteArray instruct =  bitsToBytes(gnss);
    memcpy(&gnss_height_instruct,instruct,4);
    Usb7103_TransFifo(hUsb7103, GPS_SEND_PASSAGE ,gnss_height_instruct);
}


QByteArray send429class::bitsToBytes(QBitArray bits) {
    QByteArray bytes;
    bytes.resize(bits.count()/8);
    bytes.fill(0);
    // Convert from QBitArray to QByteArray
    for(int b=0; b<bits.count(); ++b)
        bytes[b/8] = ( bytes.at(b/8) | ((bits[b]?1:0)<<(b%8)));
    return bytes;
}

bool send429class::isDigitStr(QString src)
{

    QByteArray ba = src.toLatin1();//QString 转换为 char*
    const char *s = ba.data();

    while(*s && *s>='0' && *s<='9') s++;

    if (*s)
    { //不是纯数字
        if(src.contains('.'))
            return true;
        return false;
    }
    else
    { //纯数字
        return true;
    }
}

QString send429class::return_4Bit(int num) {
    QString four_Bit;
    switch (num) {
    case 0:
        four_Bit = "0000";
        break;

    case 1:
        four_Bit = "1000";
        break;
    case 2:
        four_Bit = "0100";
        break;

    case 3:
        four_Bit = "1100";
        break;
    case 4:
        four_Bit = "0010";
        break;

    case 5:
        four_Bit = "1010";
        break;
    case 6:
        four_Bit = "0110";
        break;

    case 7:
        four_Bit = "1110";
        break;
    case 8:
        four_Bit = "0001";
        break;

    case 9:
        four_Bit = "1001";
        break;
    default:
        break;
    }
    return four_Bit;
}

void send429class::plane_flowstyle_0()
{
    QBitArray flow_style; //大气系统状态
    flow_style.resize(32);
    flow_style.setBit(0,true);
    flow_style.setBit(1,true);
    flow_style.setBit(2,false);
    flow_style.setBit(3,true);
    flow_style.setBit(4,false);
    flow_style.setBit(5,false);
    flow_style.setBit(6,false);
    flow_style.setBit(7,false);
    //
    flow_style.setBit(8,false);
    flow_style.setBit(9,false);
    flow_style.setBit(10,false);
    flow_style.setBit(11,false);
    flow_style.setBit(12,false);
    QString flow_styleNum = ui->flowstyle_0->text();
    if(flow_styleNum.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("flow_style高度数据不能为空"));
        return;
    }
    if(!isDigitStr(flow_styleNum)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("flow_style高度数据不能为非数字"));
        return;
    }
    //这种算法，输入和保存的结果是一致的
    int remain = (flow_styleNum.toInt())/0.0054931640625;
    for(int i = 14 ;i<= 27;i++){
        if(remain%2 == 1) {
            flow_style.setBit(i,true);
        } else
            flow_style.setBit(i,false);
        remain = remain/2;
    }

    if(ui->flowstyle_01->isChecked()){
        //为负
        //取反
        for(int i = 14;i <= 27; i++){
            if(flow_style.at(i)){
                flow_style.setBit(i,false);
            } else {
                flow_style.setBit(i,true);
            }
        }
        //加一  //该方式待验证
        for(int i = 14; i<=27 ;i++) {
            if(flow_style.at(i) == false) {
                flow_style.setBit(i,true);
                break;
            } else {
                flow_style.setBit(i,false);
            }
        }
        flow_style.setBit(28,true);
    } else {
        //unchecked  为正
        flow_style.setBit(28,false);
    }
    //LSB状态位  MSB状态位
    flow_style.setBit(29,true);
    flow_style.setBit(30,true);
    //奇校验
    flow_style.setBit(31,true);
    QByteArray instruct =  bitsToBytes(flow_style);
    //hight_msl_instruct =  instruct.toULong();
    //memcpy(bar.data(),&num,sizeof(num));
    memcpy(&flow_style_ulong_0,instruct,4);
    int i = Usb7103_TransFifo(hUsb7103, PLANE_STYLE_PORT,flow_style_ulong_0);

}


void send429class::plane_flowstyle_1()
{
    QBitArray flow_style; //大气系统状态
    flow_style.resize(32);
    flow_style.setBit(0,true);
    flow_style.setBit(1,true);
    flow_style.setBit(2,false);
    flow_style.setBit(3,true);
    flow_style.setBit(4,false);
    flow_style.setBit(5,true);
    flow_style.setBit(6,false);
    flow_style.setBit(7,false);
    //
    flow_style.setBit(8,false);
    flow_style.setBit(9,false);
    flow_style.setBit(10,false);
    flow_style.setBit(11,false);
    flow_style.setBit(12,false);
    QString flow_styleNum = ui->flowstyle_1->text();
    if(flow_styleNum.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("flow_style高度数据不能为空"));
        return;
    }
    if(!isDigitStr(flow_styleNum)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("flow_style高度数据不能为非数字"));
        return;
    }
    //这种算法，输入和保存的结果是一致的
    int remain = (flow_styleNum.toInt())/0.0054931640625;
    for(int i = 14 ;i<= 27;i++){
        if(remain%2 == 1) {
            flow_style.setBit(i,true);
        } else
            flow_style.setBit(i,false);
        remain = remain/2;
    }

    if(ui->flowstyle_11->isChecked()){
        //为负
        //取反
        for(int i = 14;i <= 27; i++){
            if(flow_style.at(i)){
                flow_style.setBit(i,false);
            } else {
                flow_style.setBit(i,true);
            }
        }
        //加一  //该方式待验证
        for(int i = 14; i<=27 ;i++) {
            if(flow_style.at(i) == false) {
                flow_style.setBit(i,true);
                break;
            } else {
                flow_style.setBit(i,false);
            }
        }
        flow_style.setBit(28,true);
    } else {
        //unchecked  为正
        flow_style.setBit(28,false);
    }
    //LSB状态位  MSB状态位
    flow_style.setBit(29,true);
    flow_style.setBit(30,true);
    //奇校验
    flow_style.setBit(31,true);
    QByteArray instruct =  bitsToBytes(flow_style);
    //hight_msl_instruct =  instruct.toULong();
    //memcpy(bar.data(),&num,sizeof(num));
    memcpy(&flow_style_ulong_1,instruct,4);

    int i = Usb7103_TransFifo(hUsb7103, PLANE_STYLE_PORT,flow_style_ulong_1);

}

void send429class::plane_flowstyle_2()
{
    QBitArray flow_style; //大气系统状态
    flow_style.resize(32);
    flow_style.setBit(0,true);
    flow_style.setBit(1,true);
    flow_style.setBit(2,false);
    flow_style.setBit(3,true);
    flow_style.setBit(4,false);
    flow_style.setBit(5,true);
    flow_style.setBit(6,false);
    flow_style.setBit(7,true);
    //
    flow_style.setBit(8,false);
    flow_style.setBit(9,false);
    flow_style.setBit(10,false);
    flow_style.setBit(11,false);
    flow_style.setBit(12,false);
    QString flow_styleNum = ui->flowstyle_2->text();
    if(flow_styleNum.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("flow_style高度数据不能为空"));
        return;
    }
    if(!isDigitStr(flow_styleNum)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("flow_style高度数据不能为非数字"));
        return;
    }
    //这种算法，输入和保存的结果是一致的
    int remain = (flow_styleNum.toInt())/0.0054931640625;
    for(int i = 14 ;i<= 27;i++){
        if(remain%2 == 1) {
            flow_style.setBit(i,true);
        } else
            flow_style.setBit(i,false);
        remain = remain/2;
    }

    if(ui->flowstyle_21->isChecked()){
        //为负
        //取反
        for(int i = 14;i <= 27; i++){
            if(flow_style.at(i)){
                flow_style.setBit(i,false);
            } else {
                flow_style.setBit(i,true);
            }
        }
        //加一  //该方式待验证
        for(int i = 14; i<=27 ;i++) {
            if(flow_style.at(i) == false) {
                flow_style.setBit(i,true);
                break;
            } else {
                flow_style.setBit(i,false);
            }
        }
        flow_style.setBit(28,true);
    } else {
        //unchecked  为正
        flow_style.setBit(28,false);
    }
    //LSB状态位  MSB状态位
    flow_style.setBit(29,true);
    flow_style.setBit(30,true);
    //奇校验
    flow_style.setBit(31,true);
    QByteArray instruct =  bitsToBytes(flow_style);
    //hight_msl_instruct =  instruct.toULong();
    //memcpy(bar.data(),&num,sizeof(num));
    memcpy(&flow_style_ulong_2,instruct,4);

    int i = Usb7103_TransFifo(hUsb7103, PLANE_STYLE_PORT,flow_style_ulong_2);

}



void send429class::plane_flowstyle_3()
{
    QBitArray flow_style; //大气系统状态
    flow_style.resize(32);
    flow_style.setBit(0,true);
    flow_style.setBit(1,true);
    flow_style.setBit(2,false);
    flow_style.setBit(3,true);
    flow_style.setBit(4,true);
    flow_style.setBit(5,true);
    flow_style.setBit(6,true);
    flow_style.setBit(7,false);
    //
    flow_style.setBit(8,false);
    flow_style.setBit(9,false);
    flow_style.setBit(10,false);
    flow_style.setBit(11,false);
    flow_style.setBit(12,false);
    QString flow_styleNum = ui->flowstyle_3->text();
    if(flow_styleNum.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("flow_style高度数据不能为空"));
        return;
    }
    if(!isDigitStr(flow_styleNum)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("flow_style高度数据不能为非数字"));
        return;
    }
    //这种算法，输入和保存的结果是一致的
    int remain = (flow_styleNum.toInt())/0.00390625;
    for(int i = 14 ;i<= 27;i++){
        if(remain%2 == 1) {
            flow_style.setBit(i,true);
        } else
            flow_style.setBit(i,false);
        remain = remain/2;
    }

    if(ui->flowstyle_31->isChecked()){
        //为负
        //取反
        for(int i = 14;i <= 27; i++){
            if(flow_style.at(i)){
                flow_style.setBit(i,false);
            } else {
                flow_style.setBit(i,true);
            }
        }
        //加一  //该方式待验证
        for(int i = 14; i<=27 ;i++) {
            if(flow_style.at(i) == false) {
                flow_style.setBit(i,true);
                break;
            } else {
                flow_style.setBit(i,false);
            }
        }
        flow_style.setBit(28,true);
    } else {
        //unchecked  为正
        flow_style.setBit(28,false);
    }
    //LSB状态位  MSB状态位
    flow_style.setBit(29,true);
    flow_style.setBit(30,true);
    //奇校验
    flow_style.setBit(31,true);
    QByteArray instruct =  bitsToBytes(flow_style);
    //hight_msl_instruct =  instruct.toULong();
    //memcpy(bar.data(),&num,sizeof(num));
    memcpy(&flow_style_ulong_3,instruct,4);

    int i = Usb7103_TransFifo(hUsb7103, PLANE_STYLE_PORT,flow_style_ulong_3);

}


void send429class::plane_flowstyle_4()
{
    QBitArray flow_style; //大气系统状态
    flow_style.resize(32);
    flow_style.setBit(0,true);
    flow_style.setBit(1,true);
    flow_style.setBit(2,false);
    flow_style.setBit(3,true);
    flow_style.setBit(4,true);
    flow_style.setBit(5,true);
    flow_style.setBit(6,true);
    flow_style.setBit(7,true);
    //
    flow_style.setBit(8,false);
    flow_style.setBit(9,false);
    flow_style.setBit(10,false);
    flow_style.setBit(11,false);
    flow_style.setBit(12,false);
    QString flow_styleNum = ui->flowstyle_4->text();
    if(flow_styleNum.isEmpty()) {
        QMessageBox::information(NULL, "Title", QStringLiteral("flow_style高度数据不能为空"));
        return;
    }
    if(!isDigitStr(flow_styleNum)) {
        QMessageBox::information(NULL, "Title", QStringLiteral("flow_style高度数据不能为非数字"));
        return;
    }
    //这种算法，输入和保存的结果是一致的
    int remain = (flow_styleNum.toInt())/0.00390625;
    for(int i = 14 ;i<= 27;i++){
        if(remain%2 == 1) {
            flow_style.setBit(i,true);
        } else
            flow_style.setBit(i,false);
        remain = remain/2;
    }

    if(ui->flowstyle_41->isChecked()){
        //为负
        //取反
        for(int i = 14;i <= 27; i++){
            if(flow_style.at(i)){
                flow_style.setBit(i,false);
            } else {
                flow_style.setBit(i,true);
            }
        }
        //加一  //该方式待验证
        for(int i = 14; i<=27 ;i++) {
            if(flow_style.at(i) == false) {
                flow_style.setBit(i,true);
                break;
            } else {
                flow_style.setBit(i,false);
            }
        }
        flow_style.setBit(28,true);
    } else {
        //unchecked  为正
        flow_style.setBit(28,false);
    }
    //LSB状态位  MSB状态位
    flow_style.setBit(29,true);
    flow_style.setBit(30,true);
    //奇校验
    flow_style.setBit(31,true);
    QByteArray instruct =  bitsToBytes(flow_style);
    //hight_msl_instruct =  instruct.toULong();
    //memcpy(bar.data(),&num,sizeof(num));
    memcpy(&flow_style_ulong_4,instruct,4);
    int i = Usb7103_TransFifo(hUsb7103, PLANE_STYLE_PORT,flow_style_ulong_4);
}


//播撒信息 422发送
QByteArray send429class::broadcastInfo_422(){
    //播撒信息  422
    QByteArray info; //帧头
    info.append(QByteArray::fromHex("C0"));
    //帧标识
    info.append(QByteArray::fromHex("04"));
    //长度
    info.append(QByteArray::fromHex("06"));
    //信息标识
    info.append(QByteArray::fromHex("A1"));
    //催化剂种类
    if(ui->open422orNot->isChecked()) {   // 选中为开始

        if(ui->comboBox_422_type->currentIndex() == 0){
            info.append(QByteArray::fromHex("01"));
        } else if (ui->comboBox_422_type->currentIndex() == 1) {
            info.append(QByteArray::fromHex("02"));
        } else if (ui->comboBox_422_type->currentIndex() == 2) {
            info.append(QByteArray::fromHex("03"));
        } else if (ui->comboBox_422_type->currentIndex() == 3) {
            info.append(QByteArray::fromHex("04"));
        } else if (ui->comboBox_422_type->currentIndex() == 4) {
            info.append(QByteArray::fromHex("05"));
        } else if (ui->comboBox_422_type->currentIndex() == 5) {
            info.append(QByteArray::fromHex("06"));
        }

    } else {    //反之为结束，这里是结束

        if(ui->comboBox_422_type->currentIndex() == 0){
            info.append(QByteArray::fromHex("81"));
        } else if (ui->comboBox_422_type->currentIndex() == 1) {
            info.append(QByteArray::fromHex("82"));
        } else if (ui->comboBox_422_type->currentIndex() == 2) {
            info.append(QByteArray::fromHex("83"));
        } else if (ui->comboBox_422_type->currentIndex() == 3) {
            info.append(QByteArray::fromHex("84"));
        } else if (ui->comboBox_422_type->currentIndex() == 1) {
            info.append(QByteArray::fromHex("85"));
        } else if (ui->comboBox_422_type->currentIndex() == 2) {
            info.append(QByteArray::fromHex("86"));
        }

    }

    QString cuihua_num  = ui->cuihua_max->text();
    //催化剂的量
    info.append(QByteArray::fromHex(cuihua_num.toStdString().c_str()));

    //校验和
    char sum = 0;
    for(char i:info.mid(2,info.size() -2 )) {
        sum += i;
    }
    info.append(sum);
    info.append(QByteArray::fromHex("C0"));

    info = setInstructToC0(info,info.size());
    return info;
}


QByteArray send429class::whichNumber(QString number, int type)
{
    int i,j;
    QString zero = "0";
    switch (type) {
    case 1:
    case 2:
        if(number.toInt()<= 15) {
            number = zero + number;
            return QByteArray::fromHex(number.toStdString().c_str());
        }
        if(number.toInt() > 15) {
            const char* i= QString::number(number.toInt(),16).toStdString().c_str();
            return QByteArray::fromHex(i);
        }
    case 3:
        i = number.toInt()/25;
        if((number.toInt())<25)
            i = 1;

        if(i<= 15) {
            number = zero + number;
            return QByteArray::fromHex(number.toStdString().c_str());
        }
        if(i > 15) {
            const char* ii= QString::number(number.toInt(),16).toStdString().c_str();
            return QByteArray::fromHex(ii);
        }
    case 4:
        j = number.toInt()/2;
        if((number.toInt())<2)
            j = 1;
        if(j<= 15) {
            number =   zero + number;
            return QByteArray::fromHex(number.toStdString().c_str());
        }
        if(j > 15) {
            const char* jj= QString::number(number.toInt(),16).toStdString().c_str();
            return QByteArray::fromHex(jj);
        }
    }
}


QByteArray send429class::setInstructToC0(QByteArray input,int inLen)
{
    int ii = 0;

    QByteArray  output;

    output.append(0xc0);
    ii ++;

    for(int jj = 0; jj < inLen; jj ++)
    {
        if(input.at(jj) == 0xc0)
        {
            output.append(0xdb);
            ii ++;
            output.append(0xde);
        }else if(input.at(jj) == 0xdb)
        {
            output.append(0xdb);
            ii ++;
            output.append(0xdd);
        }else
        {
            output.append(input.at(jj));
            ii ++;
        }

    }

    return output;

}



