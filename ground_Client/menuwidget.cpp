#include "menuwidget.h"
#include "ui_menuwidget.h"

MenuWidget::MenuWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MenuWidget)
{
    ui->setupUi(this);
    connect(ui->media,&QPushButton::clicked,this,&MenuWidget::ChangetoMediaWidget);
    connect(ui->data,&QPushButton::clicked,this,&MenuWidget::ChangetoDataWidget);
    connect(ui->showWeb,&QPushButton::clicked,this,&MenuWidget::ChangetoSendDataWidget);
    connect(ui->aboutButton,&QPushButton::clicked,this,&MenuWidget::showTheABout);

    ui->media->setCheckable(true);

    ui->media->setChecked(true);

    ui->data->setCheckable(true);
    //ui->aboutButton->setCheckable(true);
    ui->showWeb->setCheckable(true);

    ui->media->setAutoExclusive(true);
    ui->data->setAutoExclusive(true);
    ui->aboutButton->setAutoExclusive(true);
    ui->showWeb->setAutoExclusive(true);


    ui->media->setStyleSheet("QPushButton{background-color: rgb(0, 100, 245);color: white;}"
                             "QPushButton:pressed{background-color:#FFAA33}"
                             "QPushButton:checked{background-color:#FFAA33}"
                             );
    ui->data->setStyleSheet("QPushButton{background-color: rgb(0, 100, 245);color: white;}"
                            "QPushButton:pressed{background-color:#FFAA33}"
                            "QPushButton:checked{background-color:#FFAA33}"
                            );

    ui->showWeb->setStyleSheet("QPushButton{background-color: rgb(0, 100, 245);color: white;}"
                               "QPushButton:pressed{background-color:#FFAA33}"
                               "QPushButton:checked{background-color:#FFAA33}"
                               );

    ui->aboutButton->setStyleSheet("QPushButton{background-color: rgb(0, 100, 245);color: white;}"
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

void MenuWidget::ChangetoAboutWidget()
{
    emit changeWidgetSignals(3);
    qDebug()<<QStringLiteral("About模块");
}




void MenuWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter p(this);
    p.setPen(Qt::NoPen);
    p.setBrush(QBrush(QColor(0x00,0x87,0xca)));
    p.drawRect(rect());
}



void MenuWidget::showTheABout()
{
    QMessageBox overBox(QMessageBox::NoIcon,
                        QStringLiteral("机载通信控制软件"),
                        QStringLiteral("<br><b><font size = 4>机载通信控制软件地面模拟端</b><br><br><br><br><br><br><br>版权所有@8357<br>版本 v2.2.18"),
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










