#include "senddatawidget.h"
#include "ui_senddatawidget.h"

sendDataWidget::sendDataWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::sendDataWidget)
{
    ui->setupUi(this);

}

sendDataWidget::~sendDataWidget()
{
    delete ui;
}
