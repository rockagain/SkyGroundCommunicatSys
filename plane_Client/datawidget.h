#ifndef DATAWIDGET_H
#define DATAWIDGET_H

#include <QWidget>
#include <QDebug>
#include <QListWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QTextEdit>
#include "config.h"



namespace Ui {
class dataWidget;
}

class dataWidget : public QWidget
{
    Q_OBJECT

public:
    explicit dataWidget(QWidget *parent = 0);
    ~dataWidget();

private:

    Ui::dataWidget *ui;
    int Haishi_Or_Beidou;
public:
    QListWidget* filelistWidget;
    QListWidget* AudiolistWidget;
    //QPushButton* saveAudioButton;
    QLineEdit* MsgNumber;
    QPushButton* sendAudioButton;
    QPushButton* sendMsgButton;
    QPushButton* ftpButton;
    //QPushButton* playAudioButton; //播放上调语言留言
    QTextEdit* receiveMsgBox;
    QTextEdit* sendMsgBox;
    QTextEdit* sendedMsgBox;

    int getHaishi_Or_Beidou() const;

private slots:
    //发送海事  北斗卫星
    void setHaishi_Msg();
    void setBeidou_Msg();
};

#endif // DATAWIDGET_H
