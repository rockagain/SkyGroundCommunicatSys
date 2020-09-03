#include "statustoshow.h"
#include "ui_statustoshow.h"

statusToShow::statusToShow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::statusToShow)
{
    ui->setupUi(this);
    init();
}

statusToShow::~statusToShow()
{
    delete ui;
}


void statusToShow::init(){
    QHBoxLayout *Layout = new QHBoxLayout;
    Layout->setContentsMargins(0,0,0,0);

    this->setLayout(Layout);
    Layout->addWidget(ui->tabWidget);
}


void statusToShow::showMsattilateLinkStatusReport(QByteArray msg)
{
    //下面为显示内容
    QString showText = QStringLiteral("设备状态: ");
    if(msg.at(5) == 0x1)
        showText.append(QStringLiteral("正常"));
    if(msg.at(5) == 0x2)
        showText.append(QStringLiteral("故障\n"));
    showText.append(QStringLiteral("建链状态: "));
    if(msg.at(6) == 0x1)
        showText.append(QStringLiteral("建链"));
    if(msg.at(6) == 0x2)
        showText.append(QStringLiteral("断开\n"));
    showText.append(QStringLiteral("信号质量: "));
    int charToint = msg.at(7);  //此方式待验证
    showText.append(QString::number(charToint));
    showText.append(".");
    charToint = msg.at(8);
    showText.append(QString::number(charToint));
    showText.append(QStringLiteral("\n数据连接时长: "));
    charToint = msg.at(9);
    showText.append(QString::number(charToint));
    showText.append(":");
    charToint = msg.at(10);
    showText.append(QString::number(charToint));
    showText.append(":");
    charToint = msg.at(11);
    showText.append(QStringLiteral("上传速度 : "));
    showText.append(QString::number(charToint));
    showText.append("\n");
    short speed = ByteArrayToShort(msg.mid(12,2)); //可能需要转换  
    speed = BLEndianShort(speed);
    showText.append(QString::number(speed));
    showText.append("\n");
    showText.append(QStringLiteral("下载速度: "));
    speed = ByteArrayToShort(msg.mid(14,2)); //可能需要转换
    speed = BLEndianShort(speed);
    showText.append(QString::number(speed));
    showText.append("\n");
    showText.append(QStringLiteral("总发送数据: "));
    int data = ByteArrayToInt(msg.mid(16,4)); //可能需要转换
    data = BLEndianInt(data);
    showText.append(QString::number(data));
    showText.append(QStringLiteral("\n总接收数据: "));
    data = ByteArrayToInt(msg.mid(20,4));
    data = BLEndianInt(data);
    showText.append(QString::number(data));
    ui->textEdit->clear();
    ui->textEdit->setPlainText(showText);

}


short statusToShow::ByteArrayToShort(QByteArray length_2)
{
    short num;
    memcpy(&num,length_2.data(),2);
    return num;
}

int statusToShow::ByteArrayToInt(QByteArray length_4)
{
    int num;
    memcpy(&num,length_4.data(),sizeof(num));
    return num;
}



//short 转换大小端
short statusToShow::BLEndianShort(short value)
{
    return ((value & 0x00FF) << 8 ) | ((value & 0xFF00) >> 8);
}



//转换大小端
uint statusToShow::BLEndianInt(uint x)
{
    return (((uint)(x) & 0xff000000) >> 24) |  (((uint)(x) & 0x00ff0000) >> 8) | \
            (((uint)(x) & 0x0000ff00) << 8) | (((uint)(x) & 0x000000ff) << 24);
}


















