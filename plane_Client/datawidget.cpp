#include "datawidget.h"
#include "ui_datawidget.h"

dataWidget::dataWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::dataWidget)
{
    ui->setupUi(this);
    ui->haishi_radioButton->setChecked(true);
    this->Haishi_Or_Beidou = HAISHI_MSG;

    filelistWidget = new QListWidget(this);
    filelistWidget->setGeometry(20,360,300,350);
    AudiolistWidget = new QListWidget(this);
    AudiolistWidget->setGeometry(360,360,300,350);
    //saveAudioButton = new QPushButton(this);
    //saveAudioButton->setGeometry(480,350,221,41);
    //saveAudioButton->setText(QStringLiteral("语音留言"));
    MsgNumber = new QLineEdit(this);
    MsgNumber->setGeometry(170,60,171,21);
    sendAudioButton = new QPushButton(this);
    sendAudioButton->setGeometry(740,400,221,41);
    sendAudioButton->setText(QStringLiteral("机通发送音频指令"));
    sendMsgButton = new QPushButton(this);
    sendMsgButton->setGeometry(20,255,321,31);
    sendMsgButton->setText(QStringLiteral("发送短信"));
    //
    ftpButton = new QPushButton(this);
    ftpButton->setGeometry(740,500,221,41);
    ftpButton->setText(QStringLiteral("上传文件"));
    //
    //playAudioButton = new QPushButton(this);
    //playAudioButton->setGeometry(480,450,221,41);
    //playAudioButton->setText(QStringLiteral("播放上条语音留言"));
    sendMsgBox = new QTextEdit(this);
    sendMsgBox->setGeometry(20,120,321,131);
    receiveMsgBox = new QTextEdit(this);
    receiveMsgBox->setGeometry(370,60,321,221);
    sendedMsgBox = new QTextEdit(this);
    sendedMsgBox->setGeometry(740,60,321,221);
    connect(ui->beidou_radioButton,&QRadioButton::pressed,this,&dataWidget::setBeidou_Msg);
    connect(ui->haishi_radioButton,&QRadioButton::pressed,this,&dataWidget::setHaishi_Msg);
}

dataWidget::~dataWidget()
{
    delete ui;
}

int dataWidget::getHaishi_Or_Beidou() const
{
    return Haishi_Or_Beidou;
}

void dataWidget::setHaishi_Msg() {
    this->Haishi_Or_Beidou = HAISHI_MSG;
}


void dataWidget::setBeidou_Msg() {
    this->Haishi_Or_Beidou = BEIDOU_MSG;
}



