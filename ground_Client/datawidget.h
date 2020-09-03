#ifndef DATAWIDGET_H
#define DATAWIDGET_H

#include <QWidget>
#include <QDebug>
#include <QListWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QTextEdit>

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

public:

    QListWidget* filelistWidget;
    QPushButton* saveAudioButton;
    QLineEdit* MsgNumber;
    //QPushButton* sendAudioButton;
    QPushButton* sendMsgButton;
    QPushButton* ftpButton;
    QPushButton* playAudioButton; //播放本条语言留言
    QPushButton* changeRateButton;
    QTextEdit* receiveMsgBox;
    QTextEdit* sendMsgBox;
    QTextEdit* sendedMsgBox;

private slots:
    void emitChangeRate();

signals:
    void changeRate(int r);

};

#endif // DATAWIDGET_H
