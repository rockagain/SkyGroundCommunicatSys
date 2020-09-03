#include "datawidget.h"
#include "ui_datawidget.h"

dataWidget::dataWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::dataWidget)
{
    ui->setupUi(this);
    filelistWidget = new QListWidget(this);
    filelistWidget->setGeometry(20,340,321,211);
    saveAudioButton = new QPushButton(this);
    saveAudioButton->setGeometry(480,390,221,41);
    saveAudioButton->setText(QStringLiteral("语音留言"));
    MsgNumber = new QLineEdit(this);
    MsgNumber->setGeometry(170,60,171,21);
    //    sendAudioButton = new QPushButton(this);
    //    sendAudioButton->setGeometry(480,400,221,41);
    //    sendAudioButton->setText(QStringLiteral("发送音频"));
    sendMsgButton = new QPushButton(this);
    sendMsgButton->setGeometry(20,255,321,31);
    sendMsgButton->setText(QStringLiteral("发送短信"));
    //
    playAudioButton = new QPushButton(this);
    playAudioButton->setGeometry(480,450,221,41);
    playAudioButton->setText(QStringLiteral("播放本条语音留言"));
    //
    ftpButton = new QPushButton(this);
    ftpButton->setGeometry(480,510,221,41);
    ftpButton->setText(QStringLiteral("文件上传"));
    //
    changeRateButton = new QPushButton(this);
    changeRateButton->setGeometry(480,600,221,41);
    changeRateButton->setText(QStringLiteral("改变周期"));

    sendMsgBox = new QTextEdit(this);
    sendMsgBox->setGeometry(20,120,321,131);
    receiveMsgBox = new QTextEdit(this);
    receiveMsgBox->setGeometry(370,60,321,221);
    sendedMsgBox = new QTextEdit(this);
    sendedMsgBox->setGeometry(740,60,321,221);
    ui->radioButton->setChecked(true);

    connect(changeRateButton,&QPushButton::clicked,this,&dataWidget::emitChangeRate);

}

dataWidget::~dataWidget()
{
    delete ui;
}

void dataWidget::emitChangeRate() {

    if(ui->radioButton->isChecked()) {
        emit changeRate(1);
    } else if (ui->radioButton_2->isChecked()) {
        emit changeRate(2);
    } else if (ui->radioButton_3->isChecked()) {
        emit changeRate(3);
    }
}


