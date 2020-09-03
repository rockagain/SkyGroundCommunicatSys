#include "mediawidget.h"
#include "ui_mediawidget.h"

MediaWidget::MediaWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MediaWidget)
{
    ui->setupUi(this);
    //this->setAttribute(Qt::WA_StyledBackground,true);
    //this->setStyleSheet("background-color: rgb(0,255, 255)");
    //this->autoFillBackground();

    VedioPhoneCall = new QPushButton(this);
    VedioPhoneCall->setGeometry(420,70,81,31);
    VedioPhoneCall->setText(QStringLiteral("视频呼叫"));
    stopVideo = new QPushButton(this);
    stopVideo->setGeometry(640,70,81,31);
    stopVideo->setText(QStringLiteral("挂断"));
    phoneNumber = new QLineEdit(this);
    phoneNumber->setGeometry(185,75,150,26);

    phaneLabel = new QLabel(this);
    phaneLabel->setGeometry(50,20,200,51);
    //phaneLabel->setText(QStringLiteral("卫通电话状态: 通话中"));

    phoneCallCount = new QLabel(this);
    phoneCallCount->setGeometry(520,75,71,20);

    showConnectedOrNot = new QLabel(this);
    showConnectedOrNot->setGeometry(740,70,81,31);
    showConnectedOrNot->setText(QStringLiteral("未链接"));
    QFont size;
    size.setPointSize(12);
    showConnectedOrNot->setFont(size);

    //    //设置颜色
    //    QPalette pa;
    //    pa.setColor(QPalette::WindowText,Qt::red);
    //    ui.label_4->setPalette(pa);
    //showConnectedOrNot->setf
}

MediaWidget::~MediaWidget()
{
    delete ui;
}

void MediaWidget::showUnAnsweredPhone(QByteArray phoneNum){
    QString oldMsg = ui->unAnsweredPhone->toPlainText();
    ui->unAnsweredPhone->clear();
    oldMsg = bytearrayToQString(phoneNum) + "\n" + oldMsg;
    ui->unAnsweredPhone->setPlainText(oldMsg);

}

QString MediaWidget::bytearrayToQString(QByteArray num)
{
    //如无别的转换之后将删除
    return QString(num);
}







