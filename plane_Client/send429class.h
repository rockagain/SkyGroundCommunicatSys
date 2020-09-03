#ifndef SEND429CLASS_H
#define SEND429CLASS_H

#include <QWidget>
#include <QHBoxLayout>
#include <QBitArray>
#include <QMessageBox>
#include <QByteArray>
#include <QTime>
#include <QDateTime>
#include <QDebug>
#include <QTimer>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QByteArray>
#include <QString>

#include "windows.h"
#include "Usb7103Drv.h"
#include "stdio.h"
#include "string.h"
#include "conio.h"




namespace Ui {
class send429class;
}

class send429class : public QWidget
{
    Q_OBJECT

public:
    explicit send429class(QWidget *parent = 0);
    ~send429class();

public slots:

    void configGPS_Hardware();

    void init429board();
private slots:

    void sendGpsMsg();
    void Send_PlaneStyle_Msg();
    void sendPlaneMsg();
    void Send_Gps_Msg();
    void Send_422_Msg();
    void init_422_port();
    void configPlaneStyle_Hardware();
private:
    Ui::send429class *ui;
    //
    void init();
    void initParameter();
    //句柄
    unsigned long hUsb7103;
    unsigned long hUsb7103_1;
    //
    QTimer* gpsTimer;
    QTimer* planeStyleTimer;
    //串口类
    QSerialPort *m_serialPort;

    QByteArray bitsToBytes(QBitArray bits);
    bool isDigitStr(QString src);
    QString return_4Bit(int num);
    //指令发送的时间
    int gps_update_time;
    int plane_style_time;
    //下面为 初始化状态 反应成功或失败
    long status;
    //long status_1;
    //用于422选择 播撒数量的
    QByteArray whichNumber(QString number, int type);

    QByteArray setInstructToC0(QByteArray, int inLen);


    ulong hight_msl_instruct;
    ulong hdop_instruct;
    ulong vdop_instruct;
    ulong track_angle_instruct;
    ulong latitude_instruct;
    ulong longitude_instruct;
    ulong speed_instruct;
    ulong fine_latitude_instruct;
    ulong fine_longitude_instruct;
    ulong date_instruct;
    ulong time_instruct;
    ulong sensor_status_instruct;
    ulong gnss_height_instruct;

    ulong flow_style_ulong_0;
    ulong flow_style_ulong_1;
    ulong flow_style_ulong_2;
    ulong flow_style_ulong_3;
    ulong flow_style_ulong_4;

    void manipulateGNSS_HIGHT_MSL();
    void manipulateHDOP();
    void manipulateVDOP();
    void manipulateGNSS_TRACK_ANGLE();
    void manipulateGNSS_LATITUDE();
    void manipulateGNSS_LONGITUDE();
    void manipulate_GROUND_SPEED();
    void manipulateGNSS_FINE_LATITUDE();
    void manipulateGNSS_FINE_LONGITUDE();
    void manipulateUTC_TIME();
    void manipulateUTC_DATE();
    void manipulateGNSS_SENSOR_STATUS();
    void manipulateGNSS_HIGHT();

    void plane_flowstyle_0();
    void plane_flowstyle_1();
    void plane_flowstyle_2();
    void plane_flowstyle_3();
    void plane_flowstyle_4();

    QByteArray broadcastInfo_422();

    int GPS_SEND_PASSAGE;
    int PLANE_STYLE_PORT;
};

#endif // SEND429CLASS_H
