#include "mediawidget.h"
#include "ui_mediawidget.h"


MediaWidget::MediaWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MediaWidget)
{

    ui->setupUi(this);


    VedioPhoneCall = new QPushButton(this);
    VedioPhoneCall->setGeometry(420,70,81,31);
    VedioPhoneCall->setText(QStringLiteral("视频呼叫"));
    stopVideo = new QPushButton(this);
    stopVideo->setGeometry(640,70,81,31);
    stopVideo->setText(QStringLiteral("挂断"));
    phoneNumber = new QLineEdit(this);
    phoneNumber->setGeometry(185,75,150,26);


    phoneCallCount = new QLabel(this);
    phoneCallCount->setGeometry(520,75,71,20);

    showConnectedOrNot = new QLabel(this);
    showConnectedOrNot->setGeometry(740,70,81,31);
    showConnectedOrNot->setText(QStringLiteral("未链接"));

    QFont size;
    size.setPointSize(12);
    showConnectedOrNot->setFont(size);
}

MediaWidget::~MediaWidget()
{
    delete ui;
}
