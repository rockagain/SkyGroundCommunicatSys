/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.10.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QTextEdit *phoneNumber;
    QLabel *label;
    QPushButton *VedioPhoneCall;
    QPushButton *stopVideo;
    QPushButton *mSatelliteButton;
    QPushButton *AudioPhoneCall;
    QTextEdit *receiveMsgBox;
    QTextEdit *sendMsgBox;
    QLabel *label_2;
    QLabel *label_3;
    QPushButton *sendMsgButton;
    QPushButton *exitButton;
    QPushButton *JtongSelfCheck;
    QPushButton *mScontrolButton;
    QLabel *MSLSlabel;
    QLabel *MSLClabel;
    QPushButton *ftpButton;
    QPushButton *saveAudioButton;
    QPushButton *playAudioButton;
    QPushButton *sendAudioButton;
    QListWidget *filelistWidget;
    QLabel *label_7;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(1491, 698);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        phoneNumber = new QTextEdit(centralWidget);
        phoneNumber->setObjectName(QStringLiteral("phoneNumber"));
        phoneNumber->setGeometry(QRect(220, 60, 291, 41));
        label = new QLabel(centralWidget);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(-30, 50, 241, 61));
        QFont font;
        font.setPointSize(15);
        label->setFont(font);
        VedioPhoneCall = new QPushButton(centralWidget);
        VedioPhoneCall->setObjectName(QStringLiteral("VedioPhoneCall"));
        VedioPhoneCall->setGeometry(QRect(520, 60, 91, 41));
        stopVideo = new QPushButton(centralWidget);
        stopVideo->setObjectName(QStringLiteral("stopVideo"));
        stopVideo->setGeometry(QRect(750, 60, 91, 41));
        mSatelliteButton = new QPushButton(centralWidget);
        mSatelliteButton->setObjectName(QStringLiteral("mSatelliteButton"));
        mSatelliteButton->setGeometry(QRect(20, 420, 261, 31));
        AudioPhoneCall = new QPushButton(centralWidget);
        AudioPhoneCall->setObjectName(QStringLiteral("AudioPhoneCall"));
        AudioPhoneCall->setGeometry(QRect(630, 60, 91, 41));
        receiveMsgBox = new QTextEdit(centralWidget);
        receiveMsgBox->setObjectName(QStringLiteral("receiveMsgBox"));
        receiveMsgBox->setGeometry(QRect(1000, 90, 451, 181));
        sendMsgBox = new QTextEdit(centralWidget);
        sendMsgBox->setObjectName(QStringLiteral("sendMsgBox"));
        sendMsgBox->setGeometry(QRect(1000, 290, 451, 171));
        label_2 = new QLabel(centralWidget);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(1000, 70, 71, 16));
        label_3 = new QLabel(centralWidget);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(1000, 270, 72, 15));
        sendMsgButton = new QPushButton(centralWidget);
        sendMsgButton->setObjectName(QStringLiteral("sendMsgButton"));
        sendMsgButton->setGeometry(QRect(1040, 470, 93, 28));
        exitButton = new QPushButton(centralWidget);
        exitButton->setObjectName(QStringLiteral("exitButton"));
        exitButton->setGeometry(QRect(1370, 20, 61, 41));
        JtongSelfCheck = new QPushButton(centralWidget);
        JtongSelfCheck->setObjectName(QStringLiteral("JtongSelfCheck"));
        JtongSelfCheck->setGeometry(QRect(20, 380, 261, 31));
        mScontrolButton = new QPushButton(centralWidget);
        mScontrolButton->setObjectName(QStringLiteral("mScontrolButton"));
        mScontrolButton->setGeometry(QRect(20, 460, 261, 31));
        MSLSlabel = new QLabel(centralWidget);
        MSLSlabel->setObjectName(QStringLiteral("MSLSlabel"));
        MSLSlabel->setGeometry(QRect(560, 460, 161, 101));
        MSLClabel = new QLabel(centralWidget);
        MSLClabel->setObjectName(QStringLiteral("MSLClabel"));
        MSLClabel->setGeometry(QRect(750, 470, 191, 91));
        ftpButton = new QPushButton(centralWidget);
        ftpButton->setObjectName(QStringLiteral("ftpButton"));
        ftpButton->setGeometry(QRect(20, 340, 261, 31));
        saveAudioButton = new QPushButton(centralWidget);
        saveAudioButton->setObjectName(QStringLiteral("saveAudioButton"));
        saveAudioButton->setGeometry(QRect(20, 300, 261, 31));
        playAudioButton = new QPushButton(centralWidget);
        playAudioButton->setObjectName(QStringLiteral("playAudioButton"));
        playAudioButton->setGeometry(QRect(20, 260, 261, 31));
        sendAudioButton = new QPushButton(centralWidget);
        sendAudioButton->setObjectName(QStringLiteral("sendAudioButton"));
        sendAudioButton->setGeometry(QRect(20, 220, 261, 31));
        filelistWidget = new QListWidget(centralWidget);
        filelistWidget->setObjectName(QStringLiteral("filelistWidget"));
        filelistWidget->setGeometry(QRect(380, 160, 441, 231));
        label_7 = new QLabel(centralWidget);
        label_7->setObjectName(QStringLiteral("label_7"));
        label_7->setGeometry(QRect(220, 140, 131, 31));
        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1491, 26));
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MainWindow->setStatusBar(statusBar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", nullptr));
        label->setText(QApplication::translate("MainWindow", "    \350\257\267\350\276\223\345\205\245\346\202\250\347\232\204\347\224\265\350\257\235:", nullptr));
        VedioPhoneCall->setText(QApplication::translate("MainWindow", "\350\247\206\351\242\221\345\221\274\345\217\253", nullptr));
        stopVideo->setText(QApplication::translate("MainWindow", "\346\214\202  \346\226\255", nullptr));
        mSatelliteButton->setText(QApplication::translate("MainWindow", "\346\265\267\344\272\213\345\215\253\346\230\237\351\223\276\350\267\257\347\212\266\346\200\201\346\237\245\350\257\242", nullptr));
        AudioPhoneCall->setText(QApplication::translate("MainWindow", "\350\257\255\351\237\263\345\221\274\345\217\253", nullptr));
        label_2->setText(QApplication::translate("MainWindow", "\346\224\266\344\277\241\346\240\217", nullptr));
        label_3->setText(QApplication::translate("MainWindow", "\345\217\221\344\277\241\346\240\217", nullptr));
        sendMsgButton->setText(QApplication::translate("MainWindow", "\345\217\221\351\200\201\344\277\241\346\201\257", nullptr));
        exitButton->setText(QApplication::translate("MainWindow", "\345\205\263\346\234\272", nullptr));
        JtongSelfCheck->setText(QApplication::translate("MainWindow", "\346\234\272\351\200\232\350\207\252\346\243\200", nullptr));
        mScontrolButton->setText(QApplication::translate("MainWindow", "\346\265\267\344\272\213\345\215\253\346\230\237\351\223\276\350\267\257\346\216\247\345\210\266\346\214\207\344\273\244", nullptr));
        MSLSlabel->setText(QString());
        MSLClabel->setText(QString());
        ftpButton->setText(QApplication::translate("MainWindow", "FTP\346\226\207\344\273\266", nullptr));
        saveAudioButton->setText(QApplication::translate("MainWindow", "\350\257\255\351\237\263\347\225\231\350\250\200", nullptr));
        playAudioButton->setText(QApplication::translate("MainWindow", "\351\207\215\346\222\255\350\257\255\351\237\263\347\225\231\350\250\200", nullptr));
        sendAudioButton->setText(QApplication::translate("MainWindow", "\345\217\221\351\200\201\351\237\263\351\242\221", nullptr));
        label_7->setText(QApplication::translate("MainWindow", "\345\267\262\345\207\206\345\244\207\345\245\275\347\232\204\346\226\207\344\273\266\357\274\232", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
