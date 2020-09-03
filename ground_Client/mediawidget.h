#ifndef MEDIAWIDGET_H
#define MEDIAWIDGET_H

#include <QWidget>
#include <QDebug>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>


namespace Ui {
class MediaWidget;
}

class MediaWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MediaWidget(QWidget *parent = 0);
    ~MediaWidget();

private:
    Ui::MediaWidget *ui;
public:
    QPushButton* VedioPhoneCall;
    QPushButton* stopVideo;
    QLineEdit* phoneNumber;
    QLabel* showConnectedOrNot;
    QLabel* phoneCallCount;
};

#endif // MEDIAWIDGET_H
