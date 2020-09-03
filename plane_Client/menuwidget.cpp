
#include "menuwidget.h"
#include "menuwidget.h"
#include "ui_menuwidget.h"

MenuWidget::MenuWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MenuWidget)
{
    ui->setupUi(this);
    connect(ui->media,&QPushButton::clicked,this,&MenuWidget::ChangetoMediaWidget);
    connect(ui->data,&QPushButton::clicked,this,&MenuWidget::ChangetoDataWidget);
    connect(ui->senddata,&QPushButton::clicked,this,&MenuWidget::ChangetoSendDataWidget);
    connect(ui->mSatellite,&QPushButton::clicked,this,&MenuWidget::ChangetoMSatelliteWidget);
    connect(ui->aboutButton,&QPushButton::clicked,this,&MenuWidget::showTheABout);
    connect(ui->shutDownButton,&QPushButton::clicked,this,&MenuWidget::sendCloseSignal);
    connect(ui->send429,&QPushButton::clicked,this,&MenuWidget::Changeto429Widget);
    connect(ui->haishi_Button,&QPushButton::clicked,this,&MenuWidget::ChangetoHaishiWidget);
    connect(ui->Init_429,&QPushButton::clicked,this,&MenuWidget::init429Signals);

    ui->media->setCheckable(true);
    ui->data->setCheckable(true);
    ui->senddata->setCheckable(true);
    ui->mSatellite->setCheckable(true);
    ui->send429->setCheckable(true);
    //ui->aboutButton->setCheckable(true);
    //ui->shutDownButton->setCheckable(true);
    ui->media->setChecked(true);
    ui->haishi_Button->setCheckable(true);


    ui->media->setAutoExclusive(true);
    ui->data->setAutoExclusive(true);
    ui->senddata->setAutoExclusive(true);
    ui->mSatellite->setAutoExclusive(true);
    ui->send429->setAutoExclusive(true);
    ui->aboutButton->setAutoExclusive(true);
    ui->shutDownButton->setAutoExclusive(true);
    ui->haishi_Button->setAutoExclusive(true);

    ui->media->setStyleSheet("QPushButton{background-color: rgb(0, 100, 245);color: white;}"
                             "QPushButton:pressed{background-color:#FFAA33}"
                             "QPushButton:checked{background-color:#FFAA33}"
                             );
    ui->data->setStyleSheet("QPushButton{background-color: rgb(0, 100, 245);color: white;}"
                            "QPushButton:pressed{background-color:#FFAA33}"
                            "QPushButton:checked{background-color:#FFAA33}"
                            );
    ui->senddata->setStyleSheet("QPushButton{background-color: rgb(0, 100, 245);color: white;}"
                                "QPushButton:pressed{background-color:#FFAA33}"
                                "QPushButton:checked{background-color:#FFAA33}"
                                );
    ui->mSatellite->setStyleSheet("QPushButton{background-color: rgb(0, 100, 245);color: white;}"
                                  "QPushButton:pressed{background-color:#FFAA33}"
                                  "QPushButton:checked{background-color:#FFAA33}"
                                  );
    ui->aboutButton->setStyleSheet("QPushButton{background-color: rgb(0, 100, 245);color: white;}"
                                   "QPushButton:pressed{background-color:#FFAA33}"
                                   "QPushButton:checked{background-color:#FFAA33}"
                                   );
    ui->shutDownButton->setStyleSheet("QPushButton{background-color: rgb(0, 100, 245);color: white;}"
                                      "QPushButton:pressed{background-color:#FFAA33}"
                                      "QPushButton:checked{background-color:#FFAA33}"
                                      );
    ui->send429->setStyleSheet("QPushButton{background-color: rgb(0, 100, 245);color: white;}"
                               "QPushButton:pressed{background-color:#FFAA33}"
                               "QPushButton:checked{background-color:#FFAA33}"
                               );

    ui->haishi_Button->setStyleSheet("QPushButton{background-color: rgb(0, 100, 245);color: white;}"
                                     "QPushButton:pressed{background-color:#FFAA33}"
                                     "QPushButton:checked{background-color:#FFAA33}"
                                     );

    ui->Init_429->setStyleSheet("QPushButton{background-color: rgb(0, 100, 245);color: white;}"
                                "QPushButton:pressed{background-color:#FFAA33}"
                                "QPushButton:checked{background-color:#FFAA33}"
                                );
}

MenuWidget::~MenuWidget()
{
    delete ui;
}

void MenuWidget::ChangetoMediaWidget()
{
    emit changeWidgetSignals(0);
    qDebug()<<QStringLiteral("音视频模块");
}

void MenuWidget::ChangetoDataWidget()
{
    emit changeWidgetSignals(1);
    qDebug()<<QStringLiteral("数据交互模块");
}

void MenuWidget::ChangetoSendDataWidget()
{
    emit changeWidgetSignals(2);
    qDebug()<<QStringLiteral("发送数据模块");
}

void MenuWidget::ChangetoMSatelliteWidget()
{
    emit changeWidgetSignals(3);
    qDebug()<<QStringLiteral("海事卫星模块");
}

// 已废弃  之后需要删除
void MenuWidget::ChangetoAboutWidget()
{
    emit changeWidgetSignals(4);
    qDebug()<<QStringLiteral("About模块");
}

void MenuWidget::Changeto429Widget()
{
    emit changeWidgetSignals(4);
    qDebug()<<QStringLiteral("发送429/422");
}

void MenuWidget::ChangetoHaishiWidget()
{
    emit changeWidgetSignals(5);
    qDebug()<<QStringLiteral("海事卫星状态显示");
}


void MenuWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter p(this);
    p.setPen(Qt::NoPen);
    p.setBrush(QBrush(QColor(0x00,0x87,0xca)));
    p.drawRect(rect());
}

void MenuWidget::sendCloseSignal()
{
    QMessageBox messagebox(QMessageBox::NoIcon,QStringLiteral("提示"),QStringLiteral("<b>     <br> <br><br>  <font size = 4>    是否发送关机指令 <font><br><br><br>    </b> "));
    messagebox.setStandardButtons(QMessageBox::Yes|QMessageBox::No);
    messagebox.setButtonText(QMessageBox::Yes,QStringLiteral("确认关闭"));
    messagebox.setButtonText(QMessageBox::No,QStringLiteral("取消"));
    messagebox.setWindowIcon(QIcon(":/plane.png"));

    QSpacerItem* horizontalSpacer = new QSpacerItem(320, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    QGridLayout* layout = (QGridLayout*)messagebox.layout();
    layout->addItem(horizontalSpacer, layout->rowCount(), 0, 1, layout->columnCount());

    int reply = messagebox.exec();
    qDebug()<<reply;

    if(reply == QMessageBox::Yes)
    {
        qDebug()<<QStringLiteral("发送关闭指令");
        emit  shutDownTheSystem();

    } else  if(reply == QMessageBox::No)
    {
        qDebug()<<QStringLiteral("取消发送关闭指令");
    }

}


void MenuWidget::showTheABout()
{
    QMessageBox overBox(QMessageBox::NoIcon,
                        QStringLiteral("机载通信控制软件"),
                        QStringLiteral("<br><b><font size = 4>机载通信控制软件飞机模拟端</b><br><br><br><br><br><br><br>版权所有@8357<br>版本 v2.2.18"),
                        QMessageBox::Ok);
    QSpacerItem* horizontalSpacer = new QSpacerItem(560, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    overBox.setButtonText(QMessageBox::Ok, QStringLiteral("确定"));
    //overBox.setSize(200,200);
    QGridLayout* layout = (QGridLayout*)overBox.layout();
    layout->addItem(horizontalSpacer, layout->rowCount(), 0, 1, layout->columnCount());
    overBox.setWindowIcon(QIcon(":/plane.png"));
    overBox.exec();
    return;
}










