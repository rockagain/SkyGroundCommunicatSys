#include "msatellitewidget.h"
#include "ui_msatellitewidget.h"

mSatelliteWidget::mSatelliteWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::mSatelliteWidget)
{
    ui->setupUi(this);
    init();
}

mSatelliteWidget::~mSatelliteWidget()
{
    delete ui;
}


void mSatelliteWidget::init()
{
    QHBoxLayout *Layout = new QHBoxLayout;
    Layout->setContentsMargins(0,0,0,0);
    this->setLayout(Layout);
    //QTabWidget* mSatellite_Widget = new QTabWidget;
    //Layout->addWidget(mSatellite_Widget);
    watch_Widget = new QWebEngineView;
    Layout->addWidget(watch_Widget);


    QSettings* settings = new QSettings(QCoreApplication::applicationDirPath()+"/sky_config.ini", QSettings::IniFormat);
    QVariant watch = settings->value("WEB/watch_url");
    watch_Widget->load(watch.toString());

}
















































































