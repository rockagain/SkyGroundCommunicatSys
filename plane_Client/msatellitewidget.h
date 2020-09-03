#ifndef MSATELLITEWIDGET_H
#define MSATELLITEWIDGET_H

#include <QWidget>
#include <QHBoxLayout>
#include <QWebEngineView>
#include <QSettings>

namespace Ui {
class mSatelliteWidget;
}

class mSatelliteWidget : public QWidget
{
    Q_OBJECT

public:
    explicit mSatelliteWidget(QWidget *parent = 0);
    ~mSatelliteWidget();

private:
    Ui::mSatelliteWidget *ui;
    //init函数
    void init();
    //显示web
    QWebEngineView* watch_Widget;
    //QWebEngineView* parameter_Widget;
    //QWebEngineView* system_Widget;
};

#endif // MSATELLITEWIDGET_H
