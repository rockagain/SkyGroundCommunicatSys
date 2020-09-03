#ifndef SENDDATAWIDGET_H
#define SENDDATAWIDGET_H

#include <QWidget>
#include <QDebug>
#include <QButtonGroup>
#include <QSettings>
#include "config.h"
#include <QTimer>
#include <QTime>
#include <QMessageBox>
#include <QHBoxLayout>
#include <QBitArray>


namespace Ui {
class sendDataWidget;
}

class sendDataWidget : public QWidget
{
    Q_OBJECT

public:
    explicit sendDataWidget(QWidget *parent = 0);
    ~sendDataWidget();

private:
    Ui::sendDataWidget *ui;
    //记录是那一端，全局用
    int dc_side;
    //帧头
    const char* instructHead;
    //周期  周期性指令的zhou
    QTimer* circle_timer;
    //北斗作业信息为60秒
    QTimer* beidouWorkInfo_timer;
    //初始化函数
    void init();
    //机载的周期 变量， 改变周期就是改变这个量
    int circleTIME;
    //所有周期性的函数都包装在这里
    void circleWorkerGo();
    //下面三个是转换函数
    QByteArray ShortToByte(short num);
    QByteArray IntToByte(int num);
    QByteArray bitsToBytes(QBitArray bits);
    //判断是数字
    bool isDigitStr(QString src);
    //初始化参数， 否则会报错， 填入非法数字也会报错
    void setInitialParameter();
    //标准气象报
    void aimms_30_weather_Instruct();
    //飞机状态包
    void aimms_30_plane_Instruct();
    //FCDP
    void fcdp_Instruct();
    //DXAS
    void dxas_Instruct();
    //LWC_TWC
    void lwc_twc_Instruct();
    //PCASP
    void pcasp_Instruct();
    //LWC_100
    void lwc_100_Instruct();
    //CIP
    void cip_Instruct();
    //CDP
    void cdp_Instruct();
    //CPI
    void cpi_Instruct();
    //2D-S
    void twoD_S_Instruct();
    //飞机系统状态
    void planeStatusInstruct();
    //飞机位置
    void planeLocationInstruct();
    //海事作业信息
    void haishiWorkInfoMsgInstruct();
    //给你日期
    QString giveCurrentDate();
    //给你时间
    QString giveCurrentTime();
    //大小端转换 short int bytearray 版本
    short BLEndianShort(short value);
    uint BLEndianInt(uint x);
    QByteArray BLEndiansByteVersion(QByteArray value);
    //北斗信息数
    ushort beidouInstructNumber;
    //飞机编号
    QString planeNumber;
private slots:

    void everyCircleWork();
    void EmitBeidouInstruct();
    void sendHaishiMsg();
    //北斗作业信息
    void beidouWorkInfoMsgInstruct();
signals:
    //下面这些都是事件指令， 通过发送信号， 把 槽 传给 clientsocket
    //把信息给  clientsocket 去做
    void giveMsgToClientSocketToGo(QByteArray);
    //发送北斗信号
    void emitBeiDouStatus(bool,bool);
    //海事控制信号
    void haishiControlMsg(bool,int);
    //空地指令
    void emitSkyGroundMsg(int);
    //弹药信息，武器库信息
    void emitGunAndPlane(QString,QString);
    //播撒信息
    void emitBoSaMsg(QString,QString);

public slots:
    //改变频率
    void changeRate(char);
    //控制北斗作业信息
    void controlBeidouWorkInfo(bool open);
};

#endif // SENDDATAWIDGET_H
